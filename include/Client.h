//
// Created by scott on 19.05.2025.
//

#ifndef EKSAMEN_CLIENT_H
#define EKSAMEN_CLIENT_H

#include <mutex>
#include "Game/LocalState.h"

using namespace std;

class Client {
public:
    Client(int bufferSize, int serverPort, string serverIp);
    ~Client();
    void runEventLoop();
private:
    void runDrawLoop();
    void runGameEventLoop();
    void runReceiveLoop();
    void checkState(string message);
    mutex localStateMutex;
    /**
     * this localState can be read from by the receive message thread(to check if the state is correct),
     * and maybe even modified by it, as well as the game thread modifying it for every input a user makes.
     */
    LocalState localState;
    int start();
    void sendMessageToServer(string message);
    void receiveFromServer();
    int socketFileDescriptor;
    struct sockaddr_in serverAddress;
    int bufferSize;
    int serverPort;
    string serverIp;
    char* buffer; //Dont need to lock
};


#endif //EKSAMEN_CLIENT_H
