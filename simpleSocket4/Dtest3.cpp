//D is the new instance and a server
//address: localhost
//port: 10041
#include "common.h"

using namespace std;

char state[MAXOBJ][OBJLEN];  //this must be a global variable

int main() {
    int size;
    char buffer[32768];

    int socket_fd = _bindSock(localAddr, 10041);
    if (socket_fd == -1) exit(-1);

    int fd = _listenSock(socket_fd);

    //receive all state objects from A
    for (int i=0;i<MAXOBJ;i++) {
        read(fd, buffer, sizeof(buffer));
        strcpy(state[i], buffer+16);
        //cout << state[i] << endl;
        write(fd, "confirm!", sizeof("confirm!"));
    }

    for (int i=0;i<MAXOBJ;i++) {
        if (strlen(state[i]) != OBJLEN-1) {
            cout << "state trans failed!" << endl;
            cout << "length of state[" << i << "] is: " << strlen(state[i]) << endl;
            break;
        }
    }
    cout << "state trans success!" << endl;

    //send confirm of receiving state to A
    char confirm[30] = "state trans success";
    _sendMessage2(fd, messageType[4], confirm, NULL, NULL);

    close(fd);
    close(socket_fd);

    return 0;
}