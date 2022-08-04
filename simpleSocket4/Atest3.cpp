//A is the controller and a client
#include "common.h"

using namespace std;

char objBuffer[MAXOBJ][OBJLEN];
char dataBuffer[MAXDATA][DATALEN];

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
    //cout << "bytes received: " << size << endl;
    //cout << "type: " << enableblock.type << endl;
    //cout << "content: " << enableblock.content << endl;

    //send transfer request to C
    _sendMessage2(socket_fd, messageType[6], NULL, NULL, NULL);

    //receive and store all state objects from C
    for (int i=0;i<MAXOBJ;i++) {
        read(socket_fd, buffer, sizeof(buffer));
        strcpy(objBuffer[i], buffer+16);
        //cout << objBuffer[i] << endl;
        write(socket_fd, "confirm!", sizeof("confirm!")); //this step is necessary, or it will be out of sync!
    }

    /*
    for (int i=0;i<MAXOBJ;i++) {
        if (strlen(objBuffer[i]) != 1024) {
            cout << "message receive error!" << endl;
            break;
        }
    }
    cout << "message receive success!" << endl;
    */

    //connect and send all state objects to D
    int socket_fd2 = _connectSock(localAddr, 10041);
    if (socket_fd2 == -1) exit(-1);

    for (int i=0;i<MAXOBJ;i++) {
        _sendMessage2(socket_fd2, messageType[3], objBuffer[i], NULL, NULL);
        read(socket_fd2, buffer, sizeof(buffer));
        //cout << buffer << endl;
    }

    //receive confirm message from D
    Update update;
    size = read(socket_fd2, buffer, sizeof(buffer));
    memcpy(&update, buffer, sizeof(buffer));
    //cout << "bytes received: " << size << endl;
    //cout << "type: " << update.type << endl;
    //cout << "content: " << update.content << endl;

    close(socket_fd2);
    
    //send request for data packets to C
    _sendMessage2(socket_fd, messageType[7], NULL, NULL, NULL);

    //receive all data flows from C
    for (int i=0;i<MAXDATA;i++) {
        read(socket_fd, buffer, sizeof(buffer));
        strcpy(dataBuffer[i], buffer+16);
        //cout << dataBuffer[i] << endl;
        write(socket_fd, "confirm!", sizeof("confirm!"));
    }

    for (int i=0;i<MAXDATA;i++) {
        if (strlen(dataBuffer[i]) != 512) {
            cout << "message receive error!" << endl;
            break;
        }
    }
    cout << "message receive success!" << endl;

    close(socket_fd);

    //connect and send update information for route table to B
    int socket_fd3 = _connectSock(localAddr, 10039);
    if (socket_fd3 == -1) exit(-1);

    char Dport[8] = "10041";
    _sendMessage2(socket_fd3, messageType[4], NULL, localAddr, Dport);

    //receive update success message from B
    read(socket_fd3, buffer, sizeof(buffer));
    memcpy(&update, buffer, sizeof(buffer));
    cout << update.content << endl;

    //send all data flows to B
    for (int i=0;i<MAXDATA;i++) {
        _sendMessage2(socket_fd3, messageType[8], dataBuffer[i], NULL, NULL);
        read(socket_fd3, buffer, sizeof(buffer));
        //cout << buffer << endl;
    }

    //receive confirm message from B
    usleep(100);
    size = read(socket_fd3, buffer, sizeof(buffer));
    memcpy(&update, buffer, sizeof(buffer));
    cout << "bytes received: " << size << endl;
    cout << "type: " << update.type << endl;
    cout << "content: " << update.content << endl;

    gettimeofday(&end, NULL); // end time
    cout << "Total time: " << ((end.tv_usec - start.tv_usec) + (end.tv_sec - start.tv_sec) * 1000000) << "us" << endl;

    close(socket_fd3);

    return 0;
}