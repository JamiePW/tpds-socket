#include "common.h"

using namespace std;

map<string, Addr> myMap;

int main() {
    char buffer[2048];

    int socket_fd = _bindSock(localAddr, 10039);
    if (socket_fd == -1) exit(-1);

    int fd = _listenSock(socket_fd);

    int size = read(fd, buffer, sizeof(buffer));
    Message message;
    memcpy(&message, buffer, sizeof(buffer));
    cout << "bytes received: " << size << endl;
    cout << "type: " << message.type << endl;
    cout << "content: " << message.content << endl;

    if (strcmp(message.type, messageType[0]) == 0) {
        string temp = message.content;

        /*
        **connect to C to get new information
        */
        int socket_fd2 = _connectSock(localAddr, 10040);
        if (socket_fd2 == -1) exit(-1);

        _sendMessage(socket_fd2, messageType[0], message.content, NULL, NULL);

        read(socket_fd2, buffer, sizeof(buffer));
        memcpy(&message, buffer, sizeof(buffer));

        /*
        cout << "type: " << message.type << endl;
        cout << "content: " << message.content << endl;
        cout << "address: " << message.address << endl;
        cout << "port: " << message.port << endl;
        */

        close(socket_fd2);

        _sendMessage(fd, message.type, message.content, message.address, message.port);

        read(fd, buffer, sizeof(buffer));
        memcpy(&message, buffer, sizeof(buffer));

        if (strcmp(message.type, messageType[4]) == 0) {
            Addr addr;
            memcpy(addr.address, message.address, sizeof(message.address));
            addr.port = atoi(message.port);
            myMap[temp] = addr;

            char temp2[30] = "hashtable update succcess!";
            _sendMessage(fd, messageType[4], temp2, NULL, NULL);
        }
    }

    close(fd);
    close(socket_fd);

    return 0;
}
