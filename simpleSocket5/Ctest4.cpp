//C is the old instance, a server
//port: 10040

#include "common.h"

using namespace std;

int main() {
    char buffer[2049];
    int size;
    string tmp = strRand(1024);
    char object[1025];
    strcpy(object, tmp.c_str());

    int socket_fd = _bindSock(localAddr, 10040);
    if (socket_fd == -1) exit(-1);

    int fd = _listenSock(socket_fd);

    //receive object request from A
    size = read(fd, buffer, sizeof(buffer));
    cout << buffer << endl;

    //send object to A
    _sendMessage2(fd, messageType[3], object, NULL, NULL);

    close(fd);
    close(socket_fd);
    return 0;
}