//A is the controller, both server and client
//port: 10030

#include "common.h"

using namespace std;

map<string, string> myMap;

int main() {
    char buffer[2049];
    char stateBuffer[1025];
    int size;
    Object object;

    int socket_fd = _bindSock(localAddr, 10030);
    if (socket_fd == -1) exit(-1);

    int fd = _listenSock(socket_fd);

    //receive request from B
    size = read(fd, buffer, sizeof(buffer));
    KeyRequest keyrequest;
    memcpy(&keyrequest, buffer, sizeof(buffer));
    //cout << keyrequest.type << endl;
    //cout << keyrequest.target << endl;

    if (myMap.find(keyrequest.target) == myMap.end()) {
        //send waiting message to B
        char msg1[30] = "wait for register"; 
        _sendMessage2(fd, messageType[4], msg1, NULL, NULL);

        //read(fd, buffer, sizeof(buffer));;
        //cout << buffer << endl;

        //connect to C and send object request
        int socket_fd2 = _connectSock(localAddr, 10040);
        if (socket_fd2 == -1) exit(-1);

        _sendMessage2(socket_fd2, messageType[0], keyrequest.target, NULL, NULL);

        //receive object message from C
        size = read(socket_fd2, buffer, sizeof(buffer));
        memcpy(&object, buffer, sizeof(buffer));

        close(socket_fd2);

        myMap[keyrequest.target] = object.content;
        //cout << myMap[keyrequest.target] << endl;
    }

    //send notification to B
    char msg2[30] = "register finished"; 
    _sendMessage2(fd, messageType[4], msg2, NULL, NULL);

    //receive new request from B
    size = read(fd, buffer, sizeof(buffer));
    memcpy(&keyrequest, buffer, sizeof(buffer));
    //cout << keyrequest.type << endl;
    //cout << keyrequest.target << endl;

    if (myMap.find(keyrequest.target) != myMap.end()) {
        //send object to B
        strcpy(stateBuffer, myMap[keyrequest.target].c_str());
        _sendMessage2(fd, messageType[3], stateBuffer, NULL, NULL);
    }

    close(fd);
    close(socket_fd);
    return 0;
}