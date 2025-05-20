//
// Created by scott on 19.05.2025.
//
#include <winsock2.h>
#include <vector>
#include <set>
#include "../src/sockaddr_in_comparator.h"

#ifndef EKSAMEN_SERVER_H
#define EKSAMEN_SERVER_H

using namespace std;

class Server {
public:
    Server(int bufferSize, int serverPort);
    ~Server();
    void runEventLoop();
private:
    void broadcastToClients(string message);
    int cleanup();
    void start();
    void receiveMessage();
    void addClient(sockaddr_in client);
    set<sockaddr_in, sockaddr_in_comparator> clientAddresses;
    int bufferSize;
    int socketFileDescriptor;
    struct sockaddr_in serverAddress;
    int serverPort;
    char* buffer;
    void sendMessageToClient(sockaddr_in clientAddress, string message);
};

#endif //EKSAMEN_SERVER_H
