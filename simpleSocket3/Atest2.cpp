#include "common.h"

using namespace std;

char target[128] = "3f39d5c348e5b79d06e842c114e6cc571583bbf44e4b0ebfda1a01ec05745d43";

int main() {
    char buffer[32768];
    string temp;
    timeval start, end, start2, end2;

    cout << "enter anything to start..." << endl; //mark the start time
    cin >> temp;
    gettimeofday(&start, NULL); //start time

    int socket_fd = _connectSock(localAddr, 10039);
    if (socket_fd == -1) exit(-1);

    //send key request
    _sendMessage2(socket_fd, messageType[0], target, NULL, NULL);

    //receive key information from B
    int size = read(socket_fd, buffer, sizeof(buffer));
    Key key;
    memcpy(&key, buffer, sizeof(buffer));

    if (strcmp(key.type, messageType[2]) == 0) {
        /*
        **connect server D using key information from B
        */
        int socket_fd2 = _connectSock(key.address, atoi(key.port));
        if (socket_fd2 == -1) exit(-1);

        gettimeofday(&start2, NULL);

        //send object request to D
        _sendMessage2(socket_fd2, messageType[1], target , NULL, NULL);

        //receive object information from D
        Object object;
        read(socket_fd2, buffer, sizeof(buffer));
        memcpy(&object, buffer, sizeof(buffer));
        //cout << "type: " << object.type << endl;
        cout << "content: " << object.content << endl;

        gettimeofday(&end2, NULL);

        close(socket_fd2);

        /*
        **send update message for object owner to B
        */
        _sendMessage2(socket_fd, messageType[4], NULL, key.address, key.port);

        //receive update success message from B
        Update update;
        read(socket_fd, buffer, sizeof(buffer));
        memcpy(&update, buffer, sizeof(buffer));
        cout << "type: " << update.type << endl;
        cout << "content: " << update.content << endl;
    }

    gettimeofday(&end, NULL); // end time
    cout << "Total time: " << ((end.tv_usec - start.tv_usec) + (end.tv_sec - start.tv_sec) * 1000000) << "us" << endl;
    cout << "Object trans time: " << ((end2.tv_usec - start2.tv_usec) + (end2.tv_sec - start2.tv_sec) * 1000000) << "us" << endl;

    close(socket_fd);

    return 0;
}