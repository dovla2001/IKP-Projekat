// client.c
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

#define PORT 5060
#define SERVER_IP "192.168.0.105"  
#define BUFFER_SIZE 512

int main() {

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

    while (true)
    {
        printf("Unesite poruku koju zelite da skladistite:\n");
        gets_s(dataBuffer, BUFFER_SIZE);

        int iResult = sendto(client_socket, dataBuffer, strlen(dataBuffer), 0, (SOCKADDR*)&server_addr, sizeof(server_addr));
        if (iResult == SOCKET_ERROR)
        {
            printf("sendto failed with error: %d\n", WSAGetLastError());
            closesocket(client_socket);
            WSACleanup();
            return 1;
        }
    }

    return 0;
}
