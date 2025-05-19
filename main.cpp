#include <iostream>
#include <cstdint>
#include <string>

using namespace std;

const string serverIp = "1234";
const uint16_t serverPort = 8080;

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
    } else if (programType == "s") {
        //run server
        cout << "making server" <<endl;
    } else {
        cerr << "Invalid program type given from command line, expected 's' or 'c', but got " << programType << endl;
        return 1;
    }

    return 0;
}
