//B is the new instance, a client

#include "common.h"

using namespace std;

char target[128] = "3f39d5c348e5b79d06e842c114e6cc571583bbf44e4b0ebfda1a01ec05745d43";

int main() {
    timeval start, end;
    string tmp;
    char buffer[2049];
    int size;

    cout << "enter anything to start..." << endl;
    cin >> tmp;
    gettimeofday(&start, NULL);

    //connect to A
    int socket_fd = _connectSock(localAddr, 10030);
    if (socket_fd == -1) exit(-1);

    //send request to A
    _sendMessage2(socket_fd, messageType[0], target, NULL, NULL);

    //receive response from A
    size = read(socket_fd, buffer, sizeof(buffer));
    Update update;
    memcpy(&update, buffer, sizeof(buffer));
    //cout << update.type << endl;
    //cout << update.content << endl;

    //write(socket_fd, "confirm!", sizeof("confirm!"));

    //receive notification from A
    size = read(socket_fd, buffer, sizeof(buffer));
    memcpy(&update, buffer, sizeof(buffer));
    //cout << update.type << endl;
    //cout << update.content << endl;

    //send request to A again
    _sendMessage2(socket_fd, messageType[0], target, NULL, NULL);

    //receive object from A
    Object object;
    size = read(socket_fd, buffer, sizeof(buffer));
    memcpy(&object, buffer, sizeof(buffer));
    cout << "size: " << size << endl;
    cout << object.type << endl;
    cout << object.content << endl;

    gettimeofday(&end, NULL);
    cout << "Total time: " << ((end.tv_usec - start.tv_usec) + (end.tv_sec - start.tv_sec) * 1000000) << "us" << endl;

    close(socket_fd);
    return 0;
}