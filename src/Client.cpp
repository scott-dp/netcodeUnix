//
// Created by scott on 19.05.2025.
//
#include <winsock2.h>
#include <iostream>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include "../include/Client.h"

using namespace std;

void Client::start() {
    cout << "Starting client\n";
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    cout << "Creating socket\n";
    socketFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);

    if (socketFileDescriptor < 0) {
        cerr << "couldn't create client socket, try again" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Client socket created\n";

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort); //SERvER port
    int ipConvertResult = inet_pton(AF_INET, serverIp.c_str(), &serverAddress.sin_addr); //Server IP, here localhost

    if (ipConvertResult <= 0) {
        if (ipConvertResult == 0)
            cerr << "Invalid IP address format: " << serverIp << endl;
        else
            cerr << "IP address conversion error: " << WSAGetLastError() << endl;
        closesocket(socketFileDescriptor);
        WSACleanup();
        return;
    }
}

int Client::cleanup() {
    closesocket(socketFileDescriptor);
    WSACleanup();
    return 0;
}

Client::Client(int bufferSize, int serverPort, string serverIp) {
    if (bufferSize < 1 || bufferSize > 1024) {
        throw runtime_error("Buffersize cannot be smaller than 1 or greater than 1024");
    }
    if (serverPort < 0 || serverPort > 65536) {
        throw runtime_error("Invalid serverport number");
    }
    this->bufferSize = bufferSize;
    this->serverPort = serverPort;
    this->serverIp = serverIp;
    this->buffer = new char[bufferSize];
}

void Client::sendMessageToServer(string message) {
    sendto(socketFileDescriptor, message.c_str(),
           message.length(), 0,
           (const struct sockaddr*)&serverAddress, sizeof(serverAddress));

    cout << "Sent: " <<message << " to server\n";
}

void Client::receiveFromServer() {
    int serverAddressLength = sizeof(serverAddress);
    int receivedBytes = recvfrom(socketFileDescriptor, buffer, bufferSize, 0, (struct sockaddr*)&serverAddress, &serverAddressLength);

    //TODO Check that received bytes doesnt overflow
    buffer[receivedBytes]  = '\0';

    cout << "Received: " << buffer << endl;
}
