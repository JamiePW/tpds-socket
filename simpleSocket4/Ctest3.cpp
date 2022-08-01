//C is the old instance and a server
//address: localhost
//port: 10040

#include "common.h"

using namespace std;

int main() {
    int size;
    char buffer[2048];
    char state[11];
    string tmp;

    int socket_fd = _bindSock(localAddr, 10040);
    if (socket_fd == -1) exit(-1);

    int fd = _listenSock(socket_fd);

    //receive request to enable block from A
    size = read(fd, buffer, sizeof(buffer));
    EnableBlock enableblock;
    memcpy(&enableblock, buffer, sizeof(buffer));
    cout << "bytes received: " << size << endl;
    cout << "type: " << enableblock.type << endl;
    cout << "content: " << enableblock.content << endl;

    //enable block function...

    //send confirm message for enable block to A
    char confirm[16] = "block enabled";
    _sendMessage2(fd, messageType[5], confirm, NULL, NULL);

    //receive transfer request from A
    size = read(fd, buffer, sizeof(buffer));
    TransRequest transrequest;
    memcpy(&transrequest, buffer, sizeof(buffer));
    cout << "bytes received: " << size << endl;
    cout << "type: " << transrequest.type << endl;

    //send all state objects to A
    if (strcmp(transrequest.type, messageType[6]) == 0) {
        //for (int i=0;i<10;i++) {
            memset(&state, 0, sizeof(state));

            tmp = strRand(10);
            cout << tmp << endl;
            tmp.copy(state, 10, 0);
            *(state + 10) = 0;
            printf("%s\n", state);

            _sendMessage2(fd, messageType[3], state, NULL, NULL);
        //}
    }


    close(fd);
    close(socket_fd);

    return 0;
}