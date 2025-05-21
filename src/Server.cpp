//
// Created by scott on 19.05.2025.
//
#include <winsock2.h>
#include <iostream>
#include <thread>
#include <sstream>


#pragma comment(lib, "ws2_32.lib")
#include "../include/Server.h"

using namespace std;

void Server::start() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

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
}

Server::Server(int bufferSize, int serverPort) {
    if (bufferSize < 1 || bufferSize > 1024) {
        throw runtime_error("Buffersize cannot be smaller than 1 or greater than 1024");
    }
    if (serverPort < 1 || serverPort > 65536) {
        throw runtime_error("Invalid serverport number");
    }
    this->bufferSize = bufferSize;
    this->serverPort = serverPort;
    this->buffer = new char[bufferSize];
}

void Server::sendMessageToClient(sockaddr_in clientSocketAddress, string message) {
    sendto(socketFileDescriptor, message.c_str(), message.length(),
           0, (const struct sockaddr *) &clientSocketAddress,
           sizeof(clientSocketAddress));

    cout<<"message: "<<message<<". sent from server to client."<<endl;
}

sockaddr_in Server::receiveMessage() {
    struct sockaddr_in clientAddress{};
    memset(&clientAddress, 0, sizeof(clientAddress));
    int clientAddressLength = sizeof(clientAddress);


    int receivedBytes = recvfrom(socketFileDescriptor, buffer, bufferSize,
                                 0, (struct sockaddr *)&clientAddress,
                                 &clientAddressLength);


    if (receivedBytes == SOCKET_ERROR) {
        cerr << "recvfrom failed with error: " << WSAGetLastError() << endl;
        closesocket(socketFileDescriptor);
        WSACleanup();
        throw runtime_error("Failed to receive bytes");
    }

    if (receivedBytes >= 1023) {
        throw runtime_error("Too many bytes received, buffer overflow");
    }

    addClient(clientAddress);

    buffer[receivedBytes] = '\0';

    cout << "Received: " << buffer << endl;
    return clientAddress;
}

void Server::addClient(sockaddr_in client) {
    lock_guard<mutex> lock(clientAddressMutex);
    clientAddresses.insert(client);
}

void Server::idGenerationResponse(sockaddr_in sender) {
    string answer="id:";
    int playerId;
    {
        //In case clients request an id at the same time
        lock_guard<mutex> lock(playerIdLock);
        playerId = ++nextPLayerId;

    }
    answer += to_string(playerId);
    State stateCopy;
    Player *newPlayer;
    {
        newPlayer = new Player(playerId, 10, 10); //The new client that requested id generation
        lock_guard<mutex> lock(stateLock);
        authoritativeState.addPlayer(*newPlayer);
        stateCopy = authoritativeState;
    }
    for (auto player : stateCopy.players) {
        if (player.getId() != playerId) {
            //All other connected clients
            answer += "\n";
            answer += player.serialize();
        }
    }
    sendMessageToClient(sender, answer);
    broadcastToClients(newPlayer->serialize(), sender); //Let all other clients know that a new client is connected
    return;
}

void Server::broadcastToClients(string message, sockaddr_in sender) {
    sockaddr_in_comparator socketAddressComparator;
    if (message == "idgen") {
        //The first message a client sends and the client is requesting the server to generate a gamer id
        idGenerationResponse(sender);
    }
    cout << "Updating the player with the given update: " << message << "\n";
    Player playerUpdate = Player::deserialize(message);
    cout << "Deserialized message and got player: " <<
    playerUpdate.getId() << playerUpdate.getXPos() << playerUpdate.getYPos() <<
    playerUpdate.getXSpeed() << playerUpdate.getYSpeed() << "\n";
    {
        lock_guard<mutex> lock(stateLock);
        authoritativeState.updatePlayer(playerUpdate);
    }

    cout << "Updated the auth state\n";
    unique_lock<mutex> lock(clientAddressMutex);
    auto clientAddressesCopy = clientAddresses;
    lock.unlock();
    cout << "Copied client address to the set\n";
    for(auto client : clientAddressesCopy) {
        if (!socketAddressComparator(client, sender) && !socketAddressComparator(sender, client)) {
            //Client equals sender, doesn't need to get its own update
            continue;
        }
        sendMessageToClient(client, message);
    }
    cout << "Broadcasted update sucess\n";
}

void Server::runEventLoop() {
    start();
    thread(&Server::drawLoop, this).detach();

    while (true) {
        sockaddr_in sender = receiveMessage(); //Message lies in buffer
        string message = buffer;//This should be a copy of the message from a client
        cout << "Message copy: " <<message<<"\n";
        thread(&Server::broadcastToClients, this, message, sender).detach();
    }
}

void Server::drawLoop() {
    while (true) {
        State stateCopy;
        {
            lock_guard<mutex> lock(stateLock);
            authoritativeState.updateState();
            stateCopy = authoritativeState;//copy of the current state
        }
        stateCopy.drawState();
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

Server::~Server() {
    closesocket(socketFileDescriptor);
    WSACleanup();
}
