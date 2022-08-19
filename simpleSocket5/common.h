#ifndef _COMMON_H
#define _COMMON_H

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

#include <string>
#include <random>

using std::string;
using std::random_device;
using std::default_random_engine;

const int MAXOBJ = 513;  //amount of states to be transfered, should be smaller than 2048
const int MAXDATA = 1025;  //amount of data flows be transfered, should be smaller than 2048
const int OBJLEN = 1025;  //length of a state object
const int DATALEN = 513;  //length of a data flow

struct Message {
    char type[16];
    char content[1030];
    char address[20];
    char port[8];
};

struct KeyRequest {  //messageType[0]
    char type[16];
    char target[128];  //equal to "content"
};

struct ObjRequest {  //messageType[1]
    char type[16];
};


struct Key {  //messageType[2]
    char type[16];
    char address[20];
    char port[8];
};

struct Object {  //messageType[3]
    char type[16];
    char content[2049];
};

struct Update {  //messageType[4]
    char type[16];
    char content[30];
    char address[20];
    char port[8];
};

struct EnableBlock {  //messageType[5]
    char type[16];
    char content[16];
};

struct TransRequest { //messageType[6]
    char type[16];
};

struct DataRequest {  //messageType[7]
    char type[16];
};

struct Data {  //messageType[8]
    char type[16];
    char content[DATALEN];
};

struct Addr {
    char address[20];
    int port;
};

char localAddr[16] = "202.112.237.64";

char messageType[10][16] = {
    "keyrequest",
    "objrequest",
    "key",
    "object",
    "update",
    "enableblock",
    "transrequest",
    "datarequest",
    "data",
    "test"
};

int _bindSock(char *address, int port) { //bind function for server
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        std::cout << "socket init failed!" << std::endl;
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(address);

    int res = bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
    if (res == -1) {
        std::cout << "bind failed!" << std::endl;
        return -1;
    }

    //std::cout << "bind success!" << std::endl;

    return socket_fd;
}

int _listenSock (int socket_fd) { //listen function for server
    //std::cout << "waiting for client..." << std::endl;
    listen(socket_fd, 10);

    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    int fd = accept(socket_fd, (struct sockaddr*)&client, &len);
    if (fd == -1) {
        std::cout << "accept failed!" << std::endl;
        return -1;
    }
    char *ip = inet_ntoa(client.sin_addr);
    //std::cout << "client: [" << ip << "] successfully connected!" << std::endl;

    return fd;
}

int _connectSock (char *address, int port) { //connect function for client
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        std::cout << "socket init failed!" << std::endl;
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(address);

    int res = connect(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
    if (res == -1) {
        std::cout << "connect failed!" << std::endl;
        return -1;
    }

    //std::cout << "connect success!" << std::endl;
    return socket_fd;
}

void _sendMessage(int socket_fd, char *type, char *content, char *address, char *port) { //function for sending a mesage
    Message message;
    memset(&message, 0, sizeof(Message)); //init of struct is very important!
    if (type != NULL) memcpy(message.type, type, strlen(type));
    if (content != NULL) memcpy(message.content, content, strlen(content));
    if (address != NULL) memcpy(message.address, address, strlen(address));
    if (port != NULL) memcpy(message.port, port, strlen(port));
    int number = write(socket_fd, (char*)&message, sizeof(Message));
    if (number == -1) {
        std::cout << "write failed!" << std::endl; 
    }
}

void _sendMessage2(int socket_fd, char *type, char *content, char *address, char *port) {  //function for sending a mesage in different types
    if (strcmp(type, messageType[0]) == 0) {  //keyrequest
        KeyRequest keyrequest;
        memset(&keyrequest, 0, sizeof(KeyRequest));
        
        if (type != NULL) memcpy(keyrequest.type, type, strlen(type));
        if (content != NULL) memcpy(keyrequest.target, content, strlen(content));

        int number = write(socket_fd, (char *)&keyrequest, sizeof(KeyRequest));
        if (number == -1) {
            std::cout << "write failed!" << std::endl;
        }
    } else if (strcmp(type, messageType[1]) == 0) {  //objrequest
        ObjRequest objrequest;
        memset(&objrequest, 0, sizeof(ObjRequest));

        if (type != NULL) memcpy(objrequest.type, type, strlen(type));

        int number = write(socket_fd, (char *)&objrequest, sizeof(ObjRequest));
        if (number == -1) {
            std::cout << "write failed!" << std::endl;
        }
    } else if (strcmp(type, messageType[2]) == 0) {  //key
        Key key;
        memset(&key, 0, sizeof(Key));

        if (type != NULL) memcpy(key.type, type, strlen(type));
        if (address != NULL) memcpy(key.address, address, strlen(address));
        if (port != NULL) memcpy(key.port, port, strlen(port));

        int number = write(socket_fd, (char *)&key, sizeof(Key));
        if (number == -1) {
            std::cout << "write failed!" << std::endl;
        }
    } else if (strcmp(type, messageType[3]) == 0) {  //object
        Object object;
        memset(&object, 0, sizeof(Object));

        if (type != NULL) memcpy(object.type, type, strlen(type));
        if (content != NULL) memcpy(object.content, content, strlen(content));

        int number = write(socket_fd, (char *)&object, sizeof(Object));
        if (number == -1) {
            std::cout << "write failed!" << std::endl;
        }
    } else if (strcmp(type, messageType[4]) == 0) {  //update
        Update update;
        memset(&update, 0, sizeof(Update));

        if (type != NULL) memcpy(update.type, type, strlen(type));
        if (content != NULL) memcpy(update.content, content, strlen(content));
        if (address != NULL) memcpy(update.address, address, strlen(address));
        if (port != NULL) memcpy(update.port, port, strlen(port));

        int number = write(socket_fd, (char *)&update, sizeof(Update));
        if (number == -1) {
            std::cout << "write failed!" << std::endl;
        }
    } else if (strcmp(type, messageType[5]) == 0) { //enableblock
        EnableBlock enableblock;
        memset(&enableblock, 0, sizeof(EnableBlock));

        if (type != NULL) memcpy(enableblock.type, type, strlen(type));
        if (content != NULL) memcpy(enableblock.content, content, strlen(content));

        int number = write(socket_fd, (char *)&enableblock, sizeof(EnableBlock));
        if (number == -1) {
            std::cout << "write failed!" << std::endl;
        }
    } else if (strcmp(type, messageType[6]) == 0) { //transrequest
        TransRequest transrequest;
        memset(&transrequest, 0, sizeof(TransRequest));

        if (type != NULL) memcpy(transrequest.type, type, strlen(type));

        int number = write(socket_fd, (char *)&transrequest, sizeof(TransRequest));
        if (number == -1) {
            std::cout << "write failed!" << std::endl;
        }
    } else if (strcmp(type, messageType[7]) == 0) {  //datarequest
        DataRequest datarequest;
        memset(&datarequest, 0, sizeof(DataRequest));

        if (type != NULL) memcpy(datarequest.type, type, strlen(type));

        int number = write(socket_fd, (char *)&datarequest, sizeof(DataRequest));
        if (number == -1) {
            std::cout << "write failed!" << std::endl;
        }
    } else if (strcmp(type, messageType[8]) == 0) {  //data
        Data data;
        memset(&data, 0, sizeof(Data));

        if (type != NULL) memcpy(data.type, type, strlen(type));
        if (content != NULL) memcpy(data.content, content, strlen(content));

        int number = write(socket_fd, (char *)&data, sizeof(Data));
        if (number == -1) {
            std::cout << "write failed!" << std::endl;
        }
    } else {
        std::cout << "message type error, write failed!" << std::endl;
    }
}

string strRand(int length) {  //function to generate random strings 
    char tmp;
    string buffer;

    random_device rd;
    default_random_engine random(rd());

    for (int i=0;i<length;i++) {
        tmp = random() % 36;  //0-9, A-Z, 36 kinds of chars
        if (tmp < 10) {  //arabic number 
            tmp += '0';
        } else {  //capital letter
            tmp -= 10;
            tmp += 'A';
        }
        buffer += tmp;
    }

    return buffer;
}

#endif