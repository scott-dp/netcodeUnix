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
    void start();
    void receiveMessage();
    void addClient(sockaddr_in client);
    /**
     * This set can be written to by the receive message thread. At the same time
     * it can be read from by the thread that needs to broadcast to all the clients.
     */
    set<sockaddr_in, sockaddr_in_comparator> clientAddresses; //TODO Needs a mutex
    int bufferSize;
    int socketFileDescriptor;
    struct sockaddr_in serverAddress;
    int serverPort;
    /**
     * The buffer can be written to when the server has received a message form the receive message thread,
     * and can at the same time be read from by the broadcasting thread. Needs to be thread safe.
     */
    char* buffer; //TODO needs a mutex
    void sendMessageToClient(sockaddr_in clientAddress, string message);
};

#endif //EKSAMEN_SERVER_H
