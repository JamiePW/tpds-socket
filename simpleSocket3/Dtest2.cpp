#include "common.h"

using namespace std;

char obj[OBJLEN];

map<string, string> myMap;

int main() {
    char buffer[32768];

    myMap["3f39d5c348e5b79d06e842c114e6cc571583bbf44e4b0ebfda1a01ec05745d43"] = strRand(OBJLEN-1);

    int socket_fd = _bindSock(localAddr, 10041);
    if (socket_fd == -1) exit(-1);

    int fd = _listenSock(socket_fd);

    //receive object request from A
    ObjRequest objrequest;
    int size = read(fd, buffer, sizeof(buffer));
    memcpy(&objrequest, buffer, sizeof(buffer));
    //cout << objrequest.target << endl;

    if (strcmp(objrequest.type, messageType[1]) == 0 && myMap.find(objrequest.target) != myMap.end()) {
        //send object information to A
        strcpy(obj, myMap[objrequest.target].c_str());
        _sendMessage2(fd, messageType[3], obj, NULL, NULL);
    }

    close(socket_fd);

    return 0;
}
