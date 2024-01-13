// lb.c
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "conio.h"
#include "../Worker/WorkerData.h"


#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define PORT 5060
#define PORT_WORKER 5058
#define BUFFER_SIZE 512
#define MAX_WORKERS 10

typedef struct {
	sockaddr_in address;
	WorkerData data;
}WorkerInfo;

int findWorkerWithMinData(WorkerInfo workers[], int numWorkers) {
	int minData = INT_MAX;
	int minIndex = -1;

	for (int i = 0; i < numWorkers; i++) {
		if (workers[i].data.receivedMessages < minData) {
			minData = workers[i].data.receivedMessages;
			minIndex = i;
		}
	}

	printf("Selektovan je worker: %d\n", minIndex);

	return minIndex;
}


int main() {

	printf("======================================= LOAD BALANCER ========================================\n\n");

	sockaddr_in serverAddress;

	sockaddr_in workerAddress;

	WSADATA wsaData;

	int iResult;

	char dataBuffer[BUFFER_SIZE];
	char dataBuffer1[BUFFER_SIZE];

	WorkerInfo workers[MAX_WORKERS];
	int numWorkers = 0;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
		return 1;
	}

	memset((char*)&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(PORT);

	memset((char*)&workerAddress, 0, sizeof(workerAddress));
	workerAddress.sin_family = AF_INET;
	workerAddress.sin_addr.s_addr = INADDR_ANY;
	workerAddress.sin_port = htons(PORT_WORKER);

	SOCKET workerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (workerSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (serverSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	
	iResult = bind(workerSocket, (SOCKADDR*)&workerAddress, sizeof(workerAddress));

	if (iResult == SOCKET_ERROR)
	{
		printf("bind 1 failed with error: %d\n", WSAGetLastError());
		closesocket(workerSocket);
		WSACleanup();
		return 1;
	}

	iResult = bind(serverSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress));

	if (iResult == SOCKET_ERROR)
	{
		printf("bind 1 failed with error: %d\n", WSAGetLastError());
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	printf("Load Balancer je spreman i ceka na zahteve klijenata za skladistenje, kao i na prijavljivanje workera...\n");
	
	unsigned long mode = 1;
	if (ioctlsocket(serverSocket, FIONBIO, &mode) != 0 || ioctlsocket(workerSocket, FIONBIO, &mode) != 0)
	{
		printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
		closesocket(serverSocket);
		closesocket(workerSocket);
		WSACleanup();
		return 1;
	}

	int workerToBeRemoved = -1;

	int receivedMessagesCount[MAX_WORKERS] = { 0 };

	int nextWorkerIndex = 0;

	WorkerData receivedData;
	
	while (true)
	{
		sockaddr_in clientAddress;

		sockaddr_in workerAddr;

		memset(&clientAddress, 0, sizeof(clientAddress));

		memset(&workerAddr, 0, sizeof(workerAddr));

		memset(dataBuffer, 0, BUFFER_SIZE);

		memset(dataBuffer1, 0, BUFFER_SIZE);

		int sockAddrLen = sizeof(clientAddress);
		int sockWorkLen = sizeof(workerAddr);

		iResult = recvfrom(serverSocket, (char*)&receivedData, sizeof(WorkerData), 0, (SOCKADDR*)&clientAddress, &sockAddrLen);

		if (iResult != SOCKET_ERROR) {

			//dataBuffer[iResult] = '\0';

			printf("\nPrimljena poruka:\n");
			printf("Ime: %s\n", receivedData.ime);
			printf("Prezime: %s\n", receivedData.prezime);
			printf("Plata: %.2lf\n", receivedData.plata);

			int workerIndex = nextWorkerIndex;
			nextWorkerIndex = (nextWorkerIndex + 1) % numWorkers;
			receivedMessagesCount[workerIndex]++;

			iResult = sendto(workerSocket, (char*)&receivedData, sizeof(receivedData), 0, (SOCKADDR*)&workers[workerIndex].address, sizeof(workers[workerIndex].address));
			if (iResult == SOCKET_ERROR) {
				printf("sendto failed with error: %d\n", WSAGetLastError());
			
			}

		}
		else {
			if (WSAGetLastError() == WSAEWOULDBLOCK) {

			}
			else {
				break;
			}
		}

		iResult = recvfrom(workerSocket, dataBuffer1, BUFFER_SIZE, 0, (SOCKADDR*)&workerAddr, &sockWorkLen);

		if (iResult != SOCKET_ERROR) {

			dataBuffer1[iResult] = '\0';

			if (strcmp(dataBuffer1, "PRIJAVA") == 0 && numWorkers < MAX_WORKERS) {
				workers[numWorkers].address = workerAddr;
				workers[numWorkers].data.receivedMessages = 0;
				numWorkers++;
				printf("\nPrijavio se worker br. %d\n", numWorkers);
			}
			else if (strcmp(dataBuffer1, "ODJAVA") == 0) {
				for (int i = 0; i < numWorkers; i++) {
					if (memcmp(&workers[i].address, &workerAddr, sizeof(workerAddr)) == 0) {
						printf("\nOdjavio se worker br. %d\n", i + 1);

						workerToBeRemoved = i;

						break;
					}
				}
			}
			else {
				printf("\n%s\n", dataBuffer1);
			}
		}
		else {
			if (WSAGetLastError() == WSAEWOULDBLOCK) {

			}
			else {
				break;
			}
		}

		if (workerToBeRemoved != -1) {
			for (int i = workerToBeRemoved; i < numWorkers - 1; i++) {
				workers[i] = workers[i + 1];
			}
			numWorkers--;
			workerToBeRemoved = -1;
			}
	}
	WSACleanup();

	return 0;
}
