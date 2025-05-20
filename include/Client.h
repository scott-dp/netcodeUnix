//
// Created by scott on 19.05.2025.
//

#ifndef EKSAMEN_CLIENT_H
#define EKSAMEN_CLIENT_H

using namespace std;

class Client {
public:
    Client(int bufferSize, int serverPort, string serverIp);
    void start();
    void sendMessageToServer(string message);
    void receiveFromServer();
    int cleanup();
private:
    int socketFileDescriptor;
    struct sockaddr_in serverAddress;
    int bufferSize;
    int serverPort;
    string serverIp;
    char* buffer;
};


#endif //EKSAMEN_CLIENT_H
