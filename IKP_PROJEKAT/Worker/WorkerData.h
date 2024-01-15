#pragma once
#define BUFFER_SIZE 256
typedef struct {
	int receivedMessages;
	char ime[50];
	char prezime[50];
	double plata;
	char buffer[BUFFER_SIZE];
}WorkerData;