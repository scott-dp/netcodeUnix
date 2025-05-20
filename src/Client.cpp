//
// Created by scott on 19.05.2025.
//
#include <winsock2.h>
#include <iostream>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include "../include/Client.h"

using namespace std;

int Client::start() {
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
        throw runtime_error("Invalid IP address");
    }

    sendMessageToServer("idgen");//requesting a generated id from the server
    receiveFromServer(); //Generated gamer id should now be in the client buffer
    string gamerIdString(buffer);
    try {
        return stoi(gamerIdString);
    } catch (const invalid_argument& e) {
        cerr << "Invalid argument: input is not a valid integer. The input is: " << gamerIdString << "\n";
    } catch (const out_of_range& e) {
        cerr << "Out of range: input is too large or too small for an int. The input is: " << gamerIdString << "\n";
    }
}

Client::Client(int bufferSize, int serverPort, string serverIp) : localState(0) {
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
    int gamerId = start();
    localState.setGamerId(gamerId);
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

Client::~Client() {
    closesocket(socketFileDescriptor);
    WSACleanup();
}

void Client::runGameEventLoop() {
    //TODO poll or listen to keyboard for updates, then send to server and update local state (per tick) and render frame

}

void Client::runReceiveThread() {
    //TODO run the receive from server method that blocks.
}

