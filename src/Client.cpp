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
    //add timeout to the socket
    DWORD timeout = 5000;
    if (setsockopt(socketFileDescriptor, SOL_SOCKET, SO_RCVTIMEO,
                   (const char*)&timeout, sizeof(timeout)) == SOCKET_ERROR) {
        cerr << "Failed to set socket timeout: " << WSAGetLastError() << endl;
    }

    sendMessageToServer("idgen");//requesting a generated id from the server
    receiveFromServer(); //Generated gamer id should now be in the client buffer on the form id:xx, as well as other clients serialized
    return parseIdGenerationMessage();
}

int Client::parseIdGenerationMessage() {
    vector<string> lines = splitOnNewLine(buffer);//Values are seperated by newlines

    string gamerIdString = lines[0].substr(3);//generated id shall be on the first line
    int gamerId = stoi(gamerIdString);

    LocalState stateCopy;
    {
        lock_guard<mutex> lock(localStateMutex);
        stateCopy = localState;
    }

    for (int i = 1; i < lines.size(); ++i) { //possible existing clients on the next lines
        Player player = Player::deserialize(lines[i]);

        if (player.getId() == gamerId) continue; //Dont add my player
        {
            lock_guard<mutex> lock(localStateMutex);
            localState.getState()->addPlayer(player);
            cout << "Added player " << player.getId() << endl;
        }
    }

    return gamerId;
}

Client::Client(int bufferSize, int serverPort, string serverIp) : localState(0), workers(4) {
    if (bufferSize < 1 || bufferSize > 1024) {
        throw runtime_error("Buffersize cannot be smaller than 1 or greater than 1024");
    }
    if (serverPort < 0 || serverPort > 65536) {
        throw runtime_error("Invalid serverport number");
    }
    workers.start();
    this->bufferSize = bufferSize;
    this->serverPort = serverPort;
    this->serverIp = serverIp;
    this->buffer = new char[bufferSize];
    int gamerId = start();
    cout << "Setting gamer id to " << gamerId << endl;
    {
        lock_guard<mutex> lock(localStateMutex);
        localState.setGamerId(gamerId);
    }
    cout << "Set the id\n";
}

void Client::sendMessageToServer(string message) {
    sendto(socketFileDescriptor, message.c_str(),
           message.length(), 0,
           (const struct sockaddr *) &serverAddress, sizeof(serverAddress));

    cout << "Sent: " << message << " to server\n";
}

void Client::receiveFromServer() {
    int serverAddressLength = sizeof(serverAddress);
    int receivedBytes = recvfrom(socketFileDescriptor, buffer, bufferSize,
                                 0, (struct sockaddr *) &serverAddress, &serverAddressLength);

    if (receivedBytes == SOCKET_ERROR) {
        int error = WSAGetLastError();
        if (error == WSAETIMEDOUT) {
            //socket timeout
            return;
        }
        cerr << "recvfrom failed with error: " << error << endl;
        closesocket(socketFileDescriptor);
        WSACleanup();
        throw runtime_error("Failed to receive bytes");
    }

    if (receivedBytes >= 1023) {
        throw runtime_error("Too many bytes received, buffer overflow");
    }

    buffer[receivedBytes] = '\0';

    cout << "Received: " << buffer << endl;
    if (buffer[0] == 'i' && buffer[1] == 'd' && buffer[2] == ':') {
        //This is just the generated id and not actually a position update from the server
        return;
    }
    string message = buffer;
    workers.post([this, message]() {
        this->checkState(message);
    });
}

Client::~Client() {
    closesocket(socketFileDescriptor);
    WSACleanup();
}

void Client::runGameEventLoop() {
    Player *player;
    while (runClient) {
        char ch = _getch();
        switch (ch) {
            case 'w':
            case 'W': {
                unique_lock<mutex> lock(localStateMutex);
                player = localState.getMyPlayer();
                player->updateYSpeed(-1);
                string message = player->serialize();
                lock.unlock();
                sendMessageToServer(message);
                break;
            }
            case 'a':
            case 'A': {
                unique_lock<mutex> lock(localStateMutex);
                player = localState.getMyPlayer();
                player->updateXSpeed(-1);
                string message = player->serialize();
                lock.unlock();
                sendMessageToServer(message);
                break;
            }
            case 's':
            case 'S': {
                unique_lock<mutex> lock(localStateMutex);
                player = localState.getMyPlayer();
                player->updateYSpeed(1);
                string message = player->serialize();
                lock.unlock();
                sendMessageToServer(message);
                break;
            }
            case 'd':
            case 'D': {
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
                runClient = false;
                return;
            default:
                cout << "nothing\n";
                continue;
        }
    }
}

void Client::runReceiveLoop() {
    while (runClient) {
        receiveFromServer();
    }
}

void Client::runEventLoop() {
    vector<thread> threads;

    threads.emplace_back(&Client::runGameEventLoop, this);
    threads.emplace_back(&Client::runReceiveLoop, this);
    threads.emplace_back(&Client::runDrawLoop, this);

    for (auto &thread: threads) {
        thread.join();
    }
    workers.join();
    //TODO make server remove this client after this client stops the game
}

void Client::runDrawLoop() {
    while (runClient) {
        State *stateCopy;
        {
            lock_guard<mutex> lock(localStateMutex);
            localState.getState()->updateState();
            stateCopy = localState.getState();//copy of the current state
        }
        stateCopy->drawState();
        this_thread::sleep_for(chrono::milliseconds(500)); //2fps
    }
}

void Client::checkState(string message) {
    vector<Player> playersUpdates = parsePlayerUpdates(
            std::move(message)); //The players from server authoritative state
    State *stateCopy;
    {
        std::lock_guard<std::mutex> lock(localStateMutex);
        stateCopy = localState.getState();//copy of the current state
    }

    /**
     *     Loop that checks every player and checks if updating of local state is needed compared to the server state
     *     This almost certainly will always rollback, because the network delay will cause differences.
     */
    for (auto player: playersUpdates) {
        Player *predictedPlayer = stateCopy->getPlayerWithId(player.getId());
        if (predictedPlayer == nullptr) { //player is new, this client's has never seen it before
            {//Add the player to the local state
                lock_guard<mutex> lock(localStateMutex); //TODO Let a worker thread do this?
                localState.getState()->addPlayer(player);
            }
        } else if (predictedPlayer->getXPos() != player.getXPos() || predictedPlayer->getYPos() != player.getYPos()
                   || predictedPlayer->getYSpeed() != player.getYSpeed() ||
                   predictedPlayer->getXSpeed() != player.getXSpeed()) {
            lock_guard<mutex> lock(localStateMutex);//TODO worker threads do this?
            localState.getState()->updatePlayer(player); //The rollback
        }
    }
}

vector<string> Client::splitOnNewLine(const string &input) {
    vector<string> lines;
    istringstream stream(input);
    string line;

    while (getline(stream, line)) {
        if (!line.empty()) lines.push_back(line); //Dont push empty lines
    }

    return lines;
}

vector<Player> Client::parsePlayerUpdates(string serverUpdate) {
    vector<string> playersSerialized = splitOnNewLine(serverUpdate);
    vector<Player> playersUpdated;
    for (const string &player: playersSerialized) {
        playersUpdated.push_back(Player::deserialize(player));
    }
    return playersUpdated;
}

