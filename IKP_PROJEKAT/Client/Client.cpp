// client.c
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
#define SERVER_IP "192.168.0.18"  
#define BUFFER_SIZE 512

int main() {

    printf("========================================= CLIENT ============================================\n\n");

    int client_socket;
    
    struct sockaddr_in server_addr;

    WSADATA wsaData;

    char dataBuffer[BUFFER_SIZE];

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WsaStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    WorkerData workerInfo;

    while (true)
    {
        printf("\nUnesite ime radnika: ");
        gets_s(workerInfo.ime, sizeof(workerInfo.ime));

        printf("Unesite prezime radnika: ");
        gets_s(workerInfo.prezime, sizeof(workerInfo.prezime));

        printf("Unesite platu radnika: ");
        scanf_s("%lf", &workerInfo.plata);
        getchar();

        printf("\n");

        int iResult = sendto(client_socket, (const char*)&workerInfo, sizeof(WorkerData), 0, (SOCKADDR*)&server_addr, sizeof(server_addr));
        if (iResult == SOCKET_ERROR)
        {
            printf("sendto failed with error: %d\n", WSAGetLastError());
            closesocket(client_socket);
            WSACleanup();
            return 1;
        }

        printf("\n\n");
    }

    return 0;
}
