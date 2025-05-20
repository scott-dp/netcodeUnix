//
// Created by scott on 19.05.2025.
//
#include <winsock2.h>

#ifndef EKSAMEN_SERVER_H
#define EKSAMEN_SERVER_H

using namespace std;

class Server {
public:
    Server(int bufferSize, int serverPort);
    void start();
    void receiveMessage();
    void sendMessageToClient(sockaddr_in clientAddress, string message);
    int cleanup();
private:
    int bufferSize;
    int socketFileDescriptor;
    struct sockaddr_in serverAddress, clientAddress;
    int serverPort;
    char* buffer;
};

#endif //EKSAMEN_SERVER_H
