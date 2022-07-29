#include "common.h"

using namespace std;

map<string, Addr> myMap;

int main() {
    char buffer[2048];

    int socket_fd = _bindSock(localAddr, 10039);
    if (socket_fd == -1) exit(-1);

    int fd = _listenSock(socket_fd);

    //receive key request
    int size = read(fd, buffer, sizeof(buffer));
    KeyRequest keyrequest;
    memcpy(&keyrequest, buffer, sizeof(buffer));
    //cout << "bytes received: " << size << endl;
    //cout << "type: " << keyrequest.type << endl;
    //cout << "content: " << keyrequest.target << endl;

    if (strcmp(keyrequest.type, messageType[0]) == 0) {
        string temp = keyrequest.target;

        /*
        **connect to C to get new information
        */
        int socket_fd2 = _connectSock(localAddr, 10040);
        if (socket_fd2 == -1) exit(-1);

        //send key request of A to C
        _sendMessage2(socket_fd2, messageType[0], keyrequest.target, NULL, NULL);

        //receive key information from C
        Key key;
        read(socket_fd2, buffer, sizeof(buffer));
        memcpy(&key, buffer, sizeof(buffer));

        /*
        cout << "type: " << message.type << endl;
        cout << "content: " << message.content << endl;
        cout << "address: " << message.address << endl;
        cout << "port: " << message.port << endl;
        */

        close(socket_fd2);

        //send key information to A
        _sendMessage2(fd, key.type, NULL, key.address, key.port);

        //receive update information from A
        Update update;
        read(fd, buffer, sizeof(buffer));
        memcpy(&update, buffer, sizeof(buffer));

        if (strcmp(update.type, messageType[4]) == 0) {
            Addr addr;
            memcpy(addr.address, update.address, sizeof(update.address));
            addr.port = atoi(update.port);
            myMap[temp] = addr;

            //send update succcess message to A
            char temp2[30] = "hashtable update succcess!";
            _sendMessage2(fd, messageType[4], temp2, NULL, NULL);
        }
    }

    close(fd);
    close(socket_fd);

    return 0;
}
