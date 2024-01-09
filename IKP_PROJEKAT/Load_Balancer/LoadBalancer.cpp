// lb.c
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "conio.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define PORT 5059
#define BUFFER_SIZE 512

int main() {

	sockaddr_in serverAddress;

	WSADATA wsaData;

	int iResult;

	char dataBuffer[BUFFER_SIZE];

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
		return 1;
	}

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr("192.168.0.104");
	serverAddress.sin_port = htons(PORT);

	SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (serverSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %d\n", WSAGetLastError());
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

	printf("Load Balancer je spreman i ceka na zahteve klijenata za skladistenje, kao i na prijavljivanje workera...");
	
	while (true)
	{
		sockaddr_in clientAddress;
		memset(&clientAddress, 0, sizeof(clientAddress));

		memset(dataBuffer, 0, BUFFER_SIZE);

		int sockAddrLen = sizeof(clientAddress);

		iResult = recvfrom(serverSocket, dataBuffer, BUFFER_SIZE, 0, (SOCKADDR*)&clientAddress, &sockAddrLen);
		if (iResult == SOCKET_ERROR)
		{
			printf("recvfrom failed with error: %d\n", WSAGetLastError());
			continue;
		}

		printf("\nKlijent je poslao poruku: %s", dataBuffer);

	}

	WSACleanup();

	return 0;
}
