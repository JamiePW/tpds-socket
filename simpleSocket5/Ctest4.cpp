//C is the old instance, a server
//port: 10040

#include "common.h"

using namespace std;

map<string, string> myMap;

int main() {
    char buffer[2049];
    int size;
    string tmp = strRand(1024);
    myMap["3f39d5c348e5b79d06e842c114e6cc571583bbf44e4b0ebfda1a01ec05745d43"] = tmp;
    char object[1025];

    int socket_fd = _bindSock(localAddr, 10040);
    if (socket_fd == -1) exit(-1);

    int fd = _listenSock(socket_fd);

    //receive key request from A
    KeyRequest keyrequest;
    size = read(fd, buffer, sizeof(buffer));
    memcpy(&keyrequest, buffer, sizeof(buffer));
    //cout << keyrequest.target << endl;

    if (myMap.find(keyrequest.target) != myMap.end()) {
        //send object to A
        strcpy(object, myMap[keyrequest.target].c_str());
        _sendMessage2(fd, messageType[3], object, NULL, NULL);
    }

    close(fd);
    close(socket_fd);
    return 0;
}