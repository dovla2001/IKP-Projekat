#pragma once
#define BUFFER_SIZE 512
typedef struct {
	int receivedMessages;
	char ime[50];
	char prezime[50];
	double plata;
}WorkerData;