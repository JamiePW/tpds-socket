#include "common.h"

using namespace std;

map<string, Addr> myMap;

int main() {
    char buffer[2048];
    Addr addr;
    memcpy(addr.address, localAddr, sizeof(localAddr));
    addr.port = 10041;
    myMap["3f39d5c348e5b79d06e842c114e6cc571583bbf44e4b0ebfda1a01ec05745d43"] = addr;

    int socket_fd = _bindSock(localAddr, 10040);
    if (socket_fd == -1) exit(-1);

    int fd = _listenSock(socket_fd);

    //receive key request of A from B
    int size = read(fd, buffer, sizeof(buffer));
    KeyRequest keyrequest;
    memcpy(&keyrequest, buffer, sizeof(buffer));

    if (strcmp(keyrequest.type, messageType[0]) == 0 && myMap.find(keyrequest.target) != myMap.end()) {
        char _port[8];
        sprintf(_port, "%d", myMap[keyrequest.target].port);
        //send key information to B
        _sendMessage2(fd, messageType[2], NULL, myMap[keyrequest.target].address, _port);
    }

    close(fd);
    close(socket_fd);

    return 0;
}