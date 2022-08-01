//A is the controller and a client
#include "common.h"

using namespace std;

char objBuffer[2048][1025];

int main() {
    string tmp;
    timeval start, end;
    char buffer[2048];
    int size;

    cout << "enter anything to start..." << endl; //mark the start time
    cin >> tmp;
    gettimeofday(&start, NULL); //start time

    int socket_fd = _connectSock(localAddr, 10040);
    if (socket_fd == -1) exit(-1);

    //ask C to enable block
    _sendMessage2(socket_fd, messageType[5], NULL, NULL, NULL);

    //receive confirm message from C
    size = read(socket_fd, buffer, sizeof(buffer));
    EnableBlock enableblock;
    memcpy(&enableblock, buffer, sizeof(buffer));
    cout << "bytes received: " << size << endl;
    cout << "type: " << enableblock.type << endl;
    cout << "content: " << enableblock.content << endl;

    //send transfer request to C
    _sendMessage2(socket_fd, messageType[6], NULL, NULL, NULL);

    //receive and store all state objects from C
    //for (int i=0;i<10;i++) {
        read(socket_fd, buffer, sizeof(buffer));
        Object object;
        memcpy(&object, buffer, sizeof(buffer));
        cout << object.type << endl;
        cout << object.content << endl;
    //}

    gettimeofday(&end, NULL); // end time
    cout << "Total time: " << ((end.tv_usec - start.tv_usec) + (end.tv_sec - start.tv_sec) * 1000000) << "us" << endl;

    close(socket_fd);

    return 0;
}