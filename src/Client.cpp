//
// Created by scott on 19.05.2025.
//
#include <winsock2.h>
#include <iostream>
#include <ws2tcpip.h>
#include <thread>
#include <conio.h>
#include <sstream>

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
    receiveFromServer(); //Generated gamer id should now be in the client buffer on the form id:xx, as well as other clients serialized
    return parseIdGenerationMessage();
}

int Client::parseIdGenerationMessage() {
    vector<string> lines = splitOnNewLine(buffer);//Values are seperated by newlines

    string gamerId = lines[0].substr(3);//generated id shall be on the first line

    for (int i = 1; i < lines.size(); ++i) { //possible existing clients on the next lines
        Player player = Player::deserialize(lines[i]);
        {
            lock_guard<mutex> lock(localStateMutex);
            localState.getState()->addPlayer(player);
        }
    }

    try {
        return stoi(gamerId);
    } catch (const invalid_argument& e) {
        cerr << "Invalid argument: input is not a valid integer. The input is: " << gamerId << "\n";
        throw runtime_error("invalid gamer id");
    } catch (const out_of_range& e) {
        cerr << "Out of range: input is too large or too small for an int. The input is: " << gamerId << "\n";
        throw runtime_error("invalid gamer id");
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
    cout << "Setting gamer id to " << gamerId <<endl;
    localState.setGamerId(gamerId);//TODO should lock
    cout << "Set the id\n";
}

void Client::sendMessageToServer(string message) {
    sendto(socketFileDescriptor, message.c_str(),
           message.length(), 0,
           (const struct sockaddr*)&serverAddress, sizeof(serverAddress));

    cout << "Sent: " <<message << " to server\n";
}

void Client::receiveFromServer() {
    int serverAddressLength = sizeof(serverAddress);
    int receivedBytes = recvfrom(socketFileDescriptor, buffer, bufferSize,
                                 0, (struct sockaddr*)&serverAddress, &serverAddressLength);

    if (receivedBytes >= 1023) {
        throw runtime_error("Too many bytes received, buffer overflow");
    }

    buffer[receivedBytes]  = '\0';

    cout << "Received: " << buffer << endl;
    if (buffer[0] == 'i' && buffer[1] == 'd' && buffer[2] == ':') {
        //This is just the generated id and not actually a position update from the server
        return;
    }
    string message = buffer;
    thread(&Client::checkState, this, message).detach();
}

Client::~Client() {
    closesocket(socketFileDescriptor);
    WSACleanup();
}

void Client::runGameEventLoop() {
    Player *player;
    while (true) {
        char ch = _getch();
        switch (ch) {
            case 'w':
            case 'W': {
                cout << "W\n";
                unique_lock<mutex> lock(localStateMutex);
                cout << "locked\n";
                player = localState.getMyPlayer();
                cout << "player fetched\n";
                player->updateYSpeed(-1);
                cout << "speed updated\n";
                string message = player->serialize();
                cout << "player serialized\n";
                lock.unlock();
                cout << "unlocked\n";
                sendMessageToServer(message);
                cout << "sending update to server\n";
                break;
            }
            case 'a':
            case 'A':{
                cout << "A\n";
                unique_lock<mutex> lock(localStateMutex);
                player = localState.getMyPlayer();
                player->updateXSpeed(-1);
                string message = player->serialize();
                lock.unlock();
                sendMessageToServer(message);
                break;
            }
            case 's':
            case 'S':{
                cout << "S\n";
                unique_lock<mutex> lock(localStateMutex);
                player = localState.getMyPlayer();
                player->updateYSpeed(1);
                string message = player->serialize();
                lock.unlock();
                sendMessageToServer(message);
                break;
            }
            case 'd':
            case 'D':{
                cout << "D\n";
                unique_lock<mutex> lock(localStateMutex);
                player = localState.getMyPlayer();
                player->updateXSpeed(1);
                string message = player->serialize();
                lock.unlock();
                sendMessageToServer(message);
                break;
            }
            case 'q':
            case 'Q':
                cout << "Quit\n";
                return;
            default:
                cout<<"nothing\n";
                continue;
        }
    }
}

void Client::runReceiveLoop() {
    while (true) {
        receiveFromServer();
    }
}

void Client::runEventLoop() {
    vector<thread> threads;

    threads.emplace_back(&Client::runGameEventLoop, this);
    threads.emplace_back(&Client::runReceiveLoop, this);
    threads.emplace_back(&Client::runDrawLoop, this);

    for (auto &thread : threads) {
        thread.join();
    }
}

void Client::runDrawLoop() {
    while (true) {
        State* stateCopy;
        {
            lock_guard<mutex> lock(localStateMutex);
            localState.getState()->updateState();
            stateCopy = localState.getState();//copy of the current state
        }
        stateCopy->drawState();
        this_thread::sleep_for(chrono::milliseconds(1000)); //1fps
    }
}

void Client::checkState(string message) {
    Player playerUpdate = Player::deserialize(message);
    State* stateCopy;
    {
        std::lock_guard<std::mutex> lock(localStateMutex);
        stateCopy = localState.getState();//copy of the current state
    }

    Player* predictedPlayer = stateCopy->getPlayerWithId(playerUpdate.getId());
    //Check if the update is a new player, then add the player
    if (predictedPlayer == nullptr) {
        {
            lock_guard<mutex> lock(localStateMutex);
            localState.getState()->addPlayer(playerUpdate);
        }
        return;
    }
    if (predictedPlayer->getXPos() != playerUpdate.getXPos() || predictedPlayer->getYPos() != playerUpdate.getYPos()) {
        //wrong prediciton, need to rollback
        cout << "Rolling back\n";
        {
            lock_guard<mutex> lock(localStateMutex);
            localState.getState()->updatePlayer(playerUpdate); //The rollback
        }
    }
}

vector<string> Client::splitOnNewLine(const string &input) {
    vector<string> lines;
    istringstream stream(input);
    string line;

    while (getline(stream, line)) {
        lines.push_back(line);
    }

    return lines;
}

