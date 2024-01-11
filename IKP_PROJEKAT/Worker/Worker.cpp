#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "conio.h"
#include "WorkerData.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define SERVER_IP "192.168.0.105" 
#define PORT 5058
#define BUFFER_SIZE 512

//typedef struct {
//	int receivedMessages;
//}WorkerData;

int main() {

	sockaddr_in worker_addr;

	int workAddrLen = sizeof(worker_addr);

	WSADATA wsaData;

	int iResult;

	char dataBuffer[BUFFER_SIZE];

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
		return 1;
	}

	worker_addr.sin_family = AF_INET;
	worker_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	worker_addr.sin_port = htons(PORT);

	SOCKET worker_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (worker_socket == INVALID_SOCKET)
	{
		printf("socket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	printf("Unesite poruku (PRIJAVA/ODJAVA): ");
	gets_s(dataBuffer, BUFFER_SIZE);

	iResult = sendto(worker_socket, dataBuffer, strlen(dataBuffer), 0, (SOCKADDR*)&worker_addr, sizeof(worker_addr));
	if (iResult == SOCKET_ERROR)
	{
		printf("sendto failed with error: %d\n", WSAGetLastError());
		closesocket(worker_socket);
		WSACleanup();
		return 1;
	}

	if (strcmp(dataBuffer, "ODJAVA") == 0) {
		exit(0);
	}

	WorkerData workerData;
	workerData.receivedMessages = 0;

	while (true)
	{
		iResult = recvfrom(worker_socket, dataBuffer, BUFFER_SIZE, 0, (SOCKADDR*)&worker_addr, &workAddrLen);
		if (iResult == SOCKET_ERROR)
		{
			printf("recvfrom failed with error: %d\n", WSAGetLastError());
			closesocket(worker_socket);
			WSACleanup();
			return 1;
		}

		if (iResult != SOCKET_ERROR) {
			dataBuffer[iResult] = '\0';

			if (workerData.receivedMessages < BUFFER_SIZE) {
				//strcpy_s(workerData.receivedMessages, dataBuffer);
				workerData.receivedMessages++;

				printf("Primljena poruka: %s\n", dataBuffer);

				char poruka[] = "Zavrseno skladistenje podataka.";
				iResult = sendto(worker_socket, poruka, strlen(poruka), 0, (SOCKADDR*)&worker_addr, sizeof(worker_addr));
				if (iResult == SOCKET_ERROR)
				{
					printf("sendto failed with error: %d\n", WSAGetLastError());
					closesocket(worker_socket);
					WSACleanup();
					return 1;
				}
			}

			printf("Unesite poruku (NASTAVI/ODJAVA): ");
			gets_s(dataBuffer, BUFFER_SIZE);
			
			if (strcmp(dataBuffer, "ODJAVA") == 0)
			{
				char poruka[] = "ODJAVA";
				iResult = sendto(worker_socket, poruka, strlen(poruka), 0, (SOCKADDR*)&worker_addr, sizeof(worker_addr));
				if (iResult == SOCKET_ERROR)
				{
					printf("sendto failed with error: %d\n", WSAGetLastError());
					closesocket(worker_socket);
					WSACleanup();
					return 1;
				}

 				printf("Odjavljeni ste. Zatvaram program.\n");
				closesocket(worker_socket);
				WSACleanup();
				exit(0);
			}
			else if (strcmp(dataBuffer, "NASTAVI") == 0)
			{
				continue;
			}
		}
	}
}