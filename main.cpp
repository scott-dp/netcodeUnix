#include <iostream>
#include <string>
#include "include/Server.h"

using namespace std;

const int serverPort = 8080;
const int bufferSize = 1024;

void runServer();

int main() {
    runServer();

    return 0;
}

void runServer() {
    Server server(bufferSize, serverPort);
    server.runEventLoop();
}
