//B is client + server
//address: localhost
//port: 10039
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
    addr.sin_port = htons(10039);
    addr.sin_addr.s_addr = inet_addr("10.0.2.15");

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
    int port = ntohs(client.sin_port);
    cout << "client: [" << ip << ":" << port << "] successfully connected!" << endl;

    
    
    char buffer[2048];
    Message message;
    int size = read(fd, buffer, sizeof(buffer));
    memcpy(&message, buffer, sizeof(buffer));
    
    cout << "bytes received: " << size << endl;
    cout << "type: " << message.type << endl;
    cout << "content: " << message.content << endl;

    //write(fd, "welcome", 7);
    if (strcmp(message.type, "request") == 0) {
        //write(fd, "the type is request, processing...", 34);
        int socket_fd2 = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd2 == -1) {
            cout << "socket2 init failed!" << endl;
            exit(-1);
        }

        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(10040);
        server.sin_addr.s_addr = inet_addr("10.0.2.15");

        int res2 = connect(socket_fd2, (struct sockaddr*)&server, sizeof(server));
        if (res2 == -1) {
            cout << "bind server failed!" << endl;
            exit(-1);
        }

        cout << "bind server success!" << endl;

        write(socket_fd2, (char*)&message, sizeof(Message));

        Message reply;
        size = read(socket_fd2, buffer, sizeof(buffer));
        memcpy(&reply, buffer, sizeof(buffer));
        cout << reply.type << endl;
        cout << reply.content << endl;
        cout << reply.address << endl;
        cout << reply.port << endl;

        write(fd, (char*)&reply, sizeof(Message));

        close(socket_fd2);

    }

    close(fd);
    close(socket_fd);
    return 0;
}