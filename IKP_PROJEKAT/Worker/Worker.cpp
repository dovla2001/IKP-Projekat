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

#define SERVER_IP "192.168.0.104" 
#define PORT 5058
#define BUFFER_SIZE 512

int main() {

	sockaddr_in worker_addr;

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

	while (true)
	{
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
	}
}