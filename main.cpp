#include <iostream>
#include <cstdint>
#include <string>
#include "include/Server.h"
#include "include/Client.h"
#include "include/State.h"

using namespace std;

const string serverIp = "127.0.0.1";
const int serverPort = 8080;
const int bufferSize = 1024;

void runServer();
void runClient();

int main(int argc, char *argv[]) {
    string programType;
    if (argc == 2) {//correctly formatted command line
        programType = argv[1];//"c" for client, "s" for server
    } else {
        cerr << "No program type given, shutting down..." <<endl;
        return 1;
    }

    if (programType == "c") {
        //run a client
        cout << "making client" <<endl;
        runClient();
    } else if (programType == "s") {
        //run server
        cout << "making server" <<endl;
        runServer();
    } else {
        cerr << "Invalid program type given from command line, expected 's' or 'c', but got " << programType << endl;
        return 1;
    }

    return 0;
}

void runServer() {
    Server server(bufferSize, serverPort);
    server.runEventLoop();
}

void runClient() {
    State localState;
    Client client(bufferSize, serverPort, serverIp);
    //TODO start these in different threads
    client.runGameEventLoop();
    client.runReceiveThread();
}
