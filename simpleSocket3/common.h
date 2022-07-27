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

char localAddr[16] = "10.0.2.15";

char messageType[6][16] = {
    "keyrequest",
    "objrequest",
    "key",
    "object",
    "update",
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

    std::cout << "bind success!" << std::endl;

    return socket_fd;
}

int _listenSock (int socket_fd) { //listen function for server
    std::cout << "waiting for client..." << std::endl;
    listen(socket_fd, 10);

    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    int fd = accept(socket_fd, (struct sockaddr*)&client, &len);
    if (fd == -1) {
        std::cout << "accept failed!" << std::endl;
        return -1;
    }
    char *ip = inet_ntoa(client.sin_addr);
    std::cout << "client: [" << ip << "] successfully connected!" << std::endl;

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

    std::cout << "connect success!" << std::endl;
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

#endif