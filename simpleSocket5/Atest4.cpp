//A is the controller, both server and client
//port: 10039

#include "common.h"

using namespace std;

int main() {
    char buffer[2049];
    char stateBuffer[1025];
    int size;

    int socket_fd = _bindSock(localAddr, 10039);
    if (socket_fd == -1) exit(-1);

    int fd = _listenSock(socket_fd);

    //receive request from B
    size = read(fd, buffer, sizeof(buffer));
    KeyRequest keyrequest;
    memcpy(&keyrequest, buffer, sizeof(buffer));
    cout << keyrequest.type << endl;
    cout << keyrequest.target << endl;

    //send waiting message to B
    char msg1[30] = "wait for register"; 
    _sendMessage2(fd, messageType[4], msg1, NULL, NULL);

    read(fd, buffer, sizeof(buffer));;
    cout << buffer << endl;

    //connect to C and send object request
    int socket_fd2 = _connectSock(localAddr, 10040);
    if (socket_fd2 == -1) exit(-1);

    _sendMessage2(socket_fd2, messageType[1], NULL, NULL, NULL);

    //receive object message from C
    size = read(socket_fd2, buffer, sizeof(buffer));
    Object object;
    memcpy(&object, buffer, sizeof(buffer));
    strcpy(stateBuffer, object.content);
    cout << stateBuffer << endl;

    close(socket_fd2);

    //send notification to B
    char msg2[30] = "we got the object"; 
    _sendMessage2(fd, messageType[4], msg2, NULL, NULL);

    //receive new request from B
    size = read(fd, buffer, sizeof(buffer));
    memcpy(&keyrequest, buffer, sizeof(buffer));
    cout << keyrequest.type << endl;
    cout << keyrequest.target << endl;

    //send object to B
    _sendMessage2(fd, messageType[3], stateBuffer, NULL, NULL);

    close(fd);
    close(socket_fd);
    return 0;
}