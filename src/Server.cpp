//
// Created by scott on 19.05.2025.
//
#include <winsock2.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#include "../include/Server.h"

using namespace std;

void Server::start() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    int clientAddressLength = sizeof(clientAddress);

    socketFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0); //ipv4 udp socket

    if (socketFileDescriptor < 0) {
        cerr << "couldn't create server socket, try again" << endl;
        exit(EXIT_FAILURE);
    }

    serverAddress.sin_family    = AF_INET; // IPv4
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(serverPort); //server runs on port 8080

    if (bind(socketFileDescriptor, (const struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        cerr << "Couldnt bind socket, please try again" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Socket bind success\n";


    int receivedBytes = recvfrom(socketFileDescriptor, buffer, bufferSize,
                 0, ( struct sockaddr *)&clientAddress,
                 &clientAddressLength); //Client address is set here so that we can send to client aswell.
    if (receivedBytes == SOCKET_ERROR) {
        cerr << "recvfrom failed with error: " << WSAGetLastError() << endl;
        closesocket(socketFileDescriptor);
        WSACleanup();
        return;
    }
    cout << "Received bytes: " << receivedBytes <<"\n";
    buffer[receivedBytes] = '\0';

    printf("Client sent : %s\n", buffer);

    //Send a message to the client
    sendto(socketFileDescriptor, (const char *)"hello", strlen("hello"),
           0, (const struct sockaddr *) &clientAddress,
           clientAddressLength);

    cout<<"Hello message sent from server to client."<<endl;

    cleanup(socketFileDescriptor);
}

int Server::cleanup(int socketFileDescriptor) {
    closesocket(socketFileDescriptor);
    WSACleanup();
    return 0;
}

Server::Server(int bufferSize, int serverPort) {
    if (bufferSize < 1 || bufferSize > 1024) {
        throw runtime_error("Buffersize cannot be smaller than 1 or greater than 1024");
    }
    if (serverPort < 0 || serverPort > 65536) {
        throw runtime_error("Invalid serverport number");
    }
    this->bufferSize = bufferSize;
    this->serverPort = serverPort;
    this->buffer = new char[bufferSize];
}
