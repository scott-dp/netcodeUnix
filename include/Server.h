//
// Created by scott on 19.05.2025.
//
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <set>
#include <mutex>
#include <atomic>
#include "sockaddr_in_comparator.h"
#include "Game/State.h"
#include "worker_threads/Workers.h"

#ifndef EKSAMEN_SERVER_H
#define EKSAMEN_SERVER_H

using namespace std;

class Server {
public:
    Server(int bufferSize, int serverPort);
    ~Server();
    void runEventLoop();
private:
    int getch();
    string serializeAllPlayers(State state);
    Workers workers;
    void listenForQuitCommand();
    void idGenerationResponse(sockaddr_in sender);
    void drawLoop();
    mutex clientAddressMutex;
    mutex playerIdLock;
    mutex stateLock;
    State authoritativeState;
    int nextPLayerId = 1;
    void broadcastToClients(string message, sockaddr_in sender);
    void start();
    sockaddr_in receiveMessage();
    void addClient(sockaddr_in client);
    /**
     * This set can be written to by the receive message thread. At the same time
     * it can be read from by the thread that needs to broadcast to all the clients.
     */
    set<sockaddr_in, sockaddr_in_comparator> clientAddresses;
    int bufferSize;
    atomic<boolean> runServer = true;
    int socketFileDescriptor;
    struct sockaddr_in serverAddress;
    int serverPort;
    /**
     * The buffer can be written to when the server has received a message form the receive message thread,
     * and can at the same time be read from by the broadcasting thread. Needs to be thread safe.
     */
    char* buffer;
    void sendMessageToClient(sockaddr_in clientAddress, string message);
};

#endif //EKSAMEN_SERVER_H
