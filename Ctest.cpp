//C is only a server
//address: localhost
//port: 10040
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<iostream>

#include<sys/time.h> 

using namespace std;

struct Message {
    char type[16];
    char content[1030];
    char address[20];
    char port[8];
};

int main() {
    string temp;

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        cout << "socket init failed!" << endl;
        exit(-1);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(10040);
    addr.sin_addr.s_addr = inet_addr("10.0.2.15");

    int res = bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
    if (res == -1) {
        cout << "bind failed!" << endl;
        exit(-1);
    }
    cout << "bind success, waiting for client..." << endl;

    listen(socket_fd, 30);

    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    int fd = accept(socket_fd, (struct sockaddr*)&client, &len);
    if (fd == -1) {
        cout << "accept error!" << endl;
        exit(-1);
    }

    char *ip = inet_ntoa(client.sin_addr);
    int port = ntohs(client.sin_port);
    cout << "client: [" << ip << ":" << port << "] successfully connected!" << endl;

    
    char buffer[1024];
    Message message;
    int size = read(fd, buffer, sizeof(buffer));
    memcpy(&message, buffer, sizeof(buffer));
    
    cout << "bytes received: " << size << endl;
    cout << "type: " << message.type << endl;
    cout << "content: " << message.content << endl;

    if (strcmp(message.type, "request") == 0) {
        //write(fd, "the type is request, processing...", 34);
        //search hashmap
        Message key;
        memcpy(key.type, "key", sizeof("key"));
        memcpy(key.content, "this is a Message from C", sizeof("this is a Message from C"));
        memcpy(key.address, "10.0.2.15", sizeof("10.0.2.15"));
        memcpy(key.port, "10041", sizeof("10041"));
        write(fd, (char*)&key, sizeof(Message));
    }

    close(fd);
    close(socket_fd);
    return 0;
}