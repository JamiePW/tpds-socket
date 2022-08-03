//B is the switch and a server
//address: localhost
//port: 10039
#include "common.h"

using namespace std;

char dataBuffer[2048][513];

map<string, Addr> myMap;

int main () {
    int size;
    char buffer[2048];

    int socket_fd = _bindSock(localAddr, 10039);
    if (socket_fd == -1) exit(-1);

    int fd = _listenSock(socket_fd);

    size = read(fd, buffer, sizeof(buffer));
    Update update;
    memcpy(&update, buffer, sizeof(buffer));

    if (strcmp(update.type, messageType[4]) == 0) {
        Addr addr;
        memcpy(addr.address, update.address, sizeof(update.address));
        addr.port = atoi(update.port);
        myMap["3f39d5c348e5b79d06e842c114e6cc571583bbf44e4b0ebfda1a01ec05745d43"] = addr;

        //send update succcess message to A
        char temp[30] = "route table update succcess!";
        _sendMessage2(fd, messageType[4], temp, NULL, NULL);
    }

    //receive all data flows from A
    for (int i=0;i<MAXDATA;i++) {
        read(fd, buffer, sizeof(buffer));
        Data data;
        memcpy(&data, buffer, sizeof(buffer));
        strcpy(dataBuffer[i], data.content);
        write(fd, "confirm!", sizeof("confirm!"));
    }

    for (int i=0;i<MAXDATA;i++) {
        if (strlen(dataBuffer[i]) != 512) {
            cout << "data tranfer failed!" << endl;
            break;
        }
    }
    cout << "data tranfer success!" << endl;

    //send confirm message to A
    char confirm[30] = "data tranfer success!";
    _sendMessage2(fd, messageType[4], confirm, NULL, NULL);

    close(fd);
    close(socket_fd);

    return 0;
}