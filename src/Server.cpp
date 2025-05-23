//
// Created by scott on 19.05.2025.
//
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>       // for close()
#include <fcntl.h>        // for non-blocking sockets if needed
#include <errno.h>        // for error numbers
#include <iostream>
#include <thread>
#include <sstream>

#include "../include/Server.h"

using namespace std;

void Server::start() {
    socketFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0); // IPv4 UDP socket

    if (socketFileDescriptor < 0) {
        perror("Couldn't create server socket");
        exit(EXIT_FAILURE);
    }

    serverAddress.sin_family = AF_INET; // IPv4
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(serverPort);

    if (bind(socketFileDescriptor, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Couldn't bind socket");
        exit(EXIT_FAILURE);
    }

    cout << "Socket bind success\n";

    // Set receive timeout 1 second
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    if (setsockopt(socketFileDescriptor, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("Failed to set socket timeout");
    }
}


Server::Server(int bufferSize, int serverPort) : workers(4) {
    if (bufferSize < 1 || bufferSize > 1024) {
        throw runtime_error("Buffersize cannot be smaller than 1 or greater than 1024");
    }
    if (serverPort < 1 || serverPort > 65536) {
        throw runtime_error("Invalid serverport number");
    }
    workers.start();
    this->bufferSize = bufferSize;
    this->serverPort = serverPort;
    this->buffer = new char[bufferSize];
}

void Server::sendMessageToClient(sockaddr_in clientSocketAddress, string message) {
    sendto(socketFileDescriptor, message.c_str(), message.length(),
           0, (const struct sockaddr *) &clientSocketAddress,
           sizeof(clientSocketAddress));

    cout << "message: " << message << ". sent from server to client." << endl;
}

sockaddr_in Server::receiveMessage() {
    struct sockaddr_in clientAddress{};
    memset(&clientAddress, 0, sizeof(clientAddress));
    int clientAddressLength = sizeof(clientAddress);


    int receivedBytes = recvfrom(socketFileDescriptor, buffer, bufferSize,
                                 0, (struct sockaddr *) &clientAddress,
                                 &clientAddressLength);


    if (receivedBytes < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // timeout
        } else {
            perror("recvfrom failed");
            close(socketFileDescriptor);
            throw runtime_error("Failed to receive bytes");
        }
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
    string answer = "id:";
    int playerId;
    {
        //In case clients request an id at the same time
        lock_guard<mutex> lock(playerIdLock);
        playerId = ++nextPLayerId;

    }
    answer += to_string(playerId) +"\n";
    State stateCopy;
    Player *newPlayer;
    {
        newPlayer = new Player(playerId, 10, 10); //The new client that requested id generation
        lock_guard<mutex> lock(stateLock);
        authoritativeState.addPlayer(*newPlayer);
        stateCopy = authoritativeState;
    }
    answer += serializeAllPlayers(stateCopy);
    sendMessageToClient(sender, answer);
    broadcastToClients(newPlayer->serialize(), sender); //Let all other clients know that a new client is connected
    return;
}

string Server::serializeAllPlayers(State state) {
    string answer;
    for (auto player: state.players) {
        answer += player.serialize();
        answer += "\n";
    }
    return answer;
}

void Server::broadcastToClients(string message, sockaddr_in sender) {
    sockaddr_in_comparator socketAddressComparator;
    if (message == "idgen") {
        //The first message a client sends and the client is requesting the server to generate a gamer id
        idGenerationResponse(sender);
        return;
    }
    Player playerUpdate = Player::deserialize(message);
    State stateCopy;
    {
        lock_guard<mutex> lock(stateLock);
        authoritativeState.updatePlayer(playerUpdate);
        stateCopy = authoritativeState;
    }

    cout << "Updated the auth state\n";
    unique_lock<mutex> lock(clientAddressMutex);
    auto clientAddressesCopy = clientAddresses;
    lock.unlock();
    cout << "Copied client address to the set\n";
    this_thread::sleep_for(chrono::milliseconds(500)); //to introduce lag to so show the rollback feature
    message = serializeAllPlayers(stateCopy);
    for (auto client: clientAddressesCopy) {
        sendMessageToClient(client, message);
    }
    cout << "Broadcasted update sucess\n";
}

void Server::runEventLoop() {
    start();
    vector<thread> threads;
    threads.emplace_back(&Server::drawLoop, this);
    threads.emplace_back(&Server::listenForQuitCommand, this);

    while (runServer) {
        sockaddr_in sender = receiveMessage(); //Message lies in buffer

        if (sender.sin_addr.s_addr == 0) continue; //Socket timeout

        string message = buffer;//This should be a copy of the message from a client
        cout << "Message copy: " << message << "\n";
        workers.post([this, message, sender]() {
            this->broadcastToClients(message, sender);
        });
    }


    for (auto &thread: threads) {
        thread.join();
    }

    workers.join();
}

void Server::listenForQuitCommand() {
    while (runServer) {
        int ch = getch();
        switch (ch) {
            case 'q':
            case 'Q':
                runServer = false;
                return;
        }

    }
}

void Server::drawLoop() {
    while (runServer) {
        State stateCopy;
        {
            lock_guard<mutex> lock(stateLock);
            authoritativeState.updateState();
            stateCopy = authoritativeState;//copy of the current state
        }
        stateCopy.drawState();
        this_thread::sleep_for(chrono::milliseconds(500));
    }
}

Server::~Server() {
    close(socketFileDescriptor);
}

int Server::getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
