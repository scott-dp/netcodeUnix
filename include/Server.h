//
// Created by scott on 19.05.2025.
//
#include <winsock2.h>

#ifndef EKSAMEN_SERVER_H
#define EKSAMEN_SERVER_H

class Server {
public:
    Server(int bufferSize, int serverPort);
    void start();
private:
    int bufferSize;
    int socketFileDescriptor;
    struct sockaddr_in serverAddress, clientAddress;
    int serverPort;
    int cleanup(int socketFileDescriptor);
    char* buffer;
};

#endif //EKSAMEN_SERVER_H
