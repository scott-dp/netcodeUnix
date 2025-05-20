//
// Created by scott on 19.05.2025.
//

#ifndef EKSAMEN_CLIENT_H
#define EKSAMEN_CLIENT_H

#include "Game/LocalState.h"

using namespace std;

class Client {
public:
    Client(int bufferSize, int serverPort, string serverIp);
    ~Client();
    void runGameEventLoop();
    void runReceiveThread();
private:
    LocalState localState;
    int start();
    void sendMessageToServer(string message);
    void receiveFromServer();
    int socketFileDescriptor;
    struct sockaddr_in serverAddress;
    int bufferSize;
    int serverPort;
    string serverIp;
    char* buffer;
};


#endif //EKSAMEN_CLIENT_H
