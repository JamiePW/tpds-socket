//C is the old instance and a server
//address: localhost
//port: 10040

#include "common.h"

using namespace std;

char state[2048][1025];
char data[2048][513];

int main() {
    int size;
    char buffer[2048];

    int socket_fd = _bindSock(localAddr, 10040);
    if (socket_fd == -1) exit(-1);

    int fd = _listenSock(socket_fd);

    //receive request to enable block from A
    size = read(fd, buffer, sizeof(buffer));
    EnableBlock enableblock;
    memcpy(&enableblock, buffer, sizeof(buffer));
    //cout << "bytes received: " << size << endl;
    //cout << "type: " << enableblock.type << endl;
    //cout << "content: " << enableblock.content << endl;

    //enable block function...

    //send confirm message for enable block to A
    char confirm[16] = "block enabled";
    _sendMessage2(fd, messageType[5], confirm, NULL, NULL);

    //receive transfer request from A
    size = read(fd, buffer, sizeof(buffer));
    TransRequest transrequest;
    memcpy(&transrequest, buffer, sizeof(buffer));
    //cout << "bytes received: " << size << endl;
    //cout << "type: " << transrequest.type << endl;

    //send all state objects to A
    if (strcmp(transrequest.type, messageType[6]) == 0) {
        for (int i=0;i<MAXOBJ;i++) {
            string tmp = strRand(1024);  //this must be initialized inside the loop, or there will be segmentation fault!
            strcpy(state[i], tmp.c_str());
            //cout << state[i] << endl;
            _sendMessage2(fd, messageType[3], state[i], NULL, NULL);
            read(fd, buffer, sizeof(buffer)); //this step is necessary, or it will be out of sync!
            //cout << buffer << endl;
        }
    }

    //receive data request from A
    size = read(fd, buffer, sizeof(buffer));
    DataRequest datarequest;
    memcpy(&datarequest, buffer, sizeof(buffer));
    //cout << datarequest.type << endl;

    //send all data flows to A
    if (strcmp(datarequest.type, messageType[7]) == 0) {
        for (int i=0;i<MAXDATA;i++) {
            string tmp = strRand(512);
            strcpy(data[i], tmp.c_str());
            //cout << data[i] << endl;
            _sendMessage2(fd, messageType[8], data[i], NULL, NULL);
            read(fd, buffer, sizeof(buffer));
            //cout << buffer << endl;
        }
    }

    close(fd);
    close(socket_fd);

    return 0;
}