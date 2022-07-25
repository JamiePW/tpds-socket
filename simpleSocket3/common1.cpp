/*
**This is a common code for both server and client
**enter 1 for server, 2 for client
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<iostream>

#include<sys/time.h> 
#include<map>

using namespace std;

char localAddr[16] = "10.0.2.15";

struct Message {
    char type[16];
    char content[1030];
    char address[20];
    char port[8];
};

struct Addr {
    char address[20];
    int port;
};

map<string, Addr> myMap;

int main() {
    int mode;
    char buffer[2048];
    cout << "please enter the mode you want, 1 for server, 2 for client:" << endl;
    cin >> mode;
    if (mode == 1) { //server mode
        cout << "now is server mode" << endl;

        //test information
        Addr objOwner;
        strcpy(objOwner.address, localAddr);
        objOwner.port = 10041;
        myMap.insert(pair<string, Addr>("3f39d5c348e5b79d06e842c114e6cc571583bbf44e4b0ebfda1a01ec05745d43", objOwner));

        //create a socket
        int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd == -1) {
            cout << "socket init failed!" << endl;
            exit(-1);
        }

        int port;
        cout << "please enter the port you want to bind:" << endl;
        cin >> port;
        if (port < 1024 || port > 65535) {
            cout << "inappropriate port!" << endl;
            exit(-1);
        }

        //prepare address for connection
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(localAddr);

        int res = bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
        if (res == -1) {
            cout << "bind failed!" << endl;
            exit(-1);
        }
        cout << "bind success, waiting for client..." << endl;

        listen(socket_fd, 10);

        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        int fd = accept(socket_fd, (struct sockaddr*)&client, &len);
        if (fd == -1) {
            cout << "accept error!" << endl;
            exit(-1);
        }

        char *ip = inet_ntoa(client.sin_addr);
        cout << "client: [" << ip << "] successfully connected!" << endl;

        //write(fd, "welcome", sizeof("welcome"));
        Message key;
        char _port[20];
        sprintf(_port, "%d", objOwner.port);
        memcpy(key.type, "key", sizeof("key"));
        memcpy(key.content, "this is a Message from C", sizeof("this is a Message from C"));
        memcpy(key.address, objOwner.address, sizeof(objOwner.address));
        memcpy(key.port, _port, sizeof(_port));
        write(fd, (char *)&key, sizeof(Message));

        int size = read(fd, buffer, sizeof(buffer));
        Message message;
        memcpy(&message, buffer, sizeof(buffer));

        cout << "bytes received: " << size << endl;
        cout << "type: " << message.type << endl;
        cout << "content: " << message.content << endl;

        close(fd);
        close(socket_fd);


    } else if (mode == 2) { //client mode
        cout << "now is client mode" << endl;

        timeval start, end;
        string temp;

        int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd == -1) {
            cout << "socket init failed!" << endl;
            exit(-1);
        }

        int port;
        cout << "please enter the port of key owner you want to connect:";
        cin >> port;
        if (port < 1024 || port > 65535) {
            cout << "inappropriate port!" << endl;
            exit(-1);
        }

        //prepare address for connection
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(localAddr);

        cout << "enter anything to start..." << endl; //mark the start time
        cin >> temp;
        gettimeofday(&start, NULL); //start time

        int res = connect(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
        if (res == -1) {
            cout << "bind connection failed!" << endl;
            exit(-1);
        }
        cout << "bind connection success!" << endl;

        write(socket_fd, "hello Jamie!", sizeof("hello Jamie!"));

        int size = read(socket_fd, buffer, sizeof(buffer));
        //cout << buffer << endl;
        Message reply;
        memcpy(&reply, buffer, sizeof(buffer));
        cout << "type: " << reply.type << endl;
        cout << "content: " << reply.content << endl;
        cout << "address: " << reply.address << endl;
        cout << "port: " << atoi(reply.port) << endl; 

        gettimeofday(&end, NULL); // end time
        cout << "Total time: " << ((end.tv_usec - start.tv_usec) + (end.tv_sec - start.tv_sec) * 1000000) << "us" << endl;

        close(socket_fd);



    } else {
        cout << "wrong format of mode!" << endl;
        exit(-1);
    }
    return 0;
}

