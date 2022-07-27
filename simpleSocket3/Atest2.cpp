#include "common.h"

using namespace std;

char target[128] = "3f39d5c348e5b79d06e842c114e6cc571583bbf44e4b0ebfda1a01ec05745d43";

int main() {
    char buffer[2048];
    string temp;
    timeval start, end;

    int socket_fd = _connectSock(localAddr, 10039);
    if (socket_fd == -1) exit(-1);

    cout << "enter anything to start..." << endl; //mark the start time
    cin >> temp;
    gettimeofday(&start, NULL); //start time

    _sendMessage(socket_fd, messageType[0], target, NULL, NULL);

    int size = read(socket_fd, buffer, sizeof(buffer));
    Message message;
    memcpy(&message, buffer, sizeof(buffer));

    if (strcmp(message.type, messageType[2]) == 0) {
        /*
        **connect server D
        */
        int socket_fd2 = _connectSock(message.address, atoi(message.port));
        if (socket_fd2 == -1) exit(-1);

        char test[20] = "I need the object";
        _sendMessage(socket_fd2, messageType[1], test, NULL, NULL);

        Message reply;
        read(socket_fd2, buffer, sizeof(buffer));
        memcpy(&reply, buffer, sizeof(buffer));

        cout << "type: " << reply.type << endl;
        cout << "content: " << reply.content << endl;

        close(socket_fd2);

        /*
        **send update message for object owner to B
        */
        _sendMessage(socket_fd, messageType[4], NULL, message.address, message.port);

        read(socket_fd, buffer, sizeof(buffer));
        memcpy(&reply, buffer, sizeof(buffer));
        cout << "type: " << reply.type << endl;
        cout << "content: " << reply.content << endl;
    }

    gettimeofday(&end, NULL); // end time
    cout << "Total time: " << ((end.tv_usec - start.tv_usec) + (end.tv_sec - start.tv_sec) * 1000000) << "us" << endl;

    close(socket_fd);

    return 0;
}