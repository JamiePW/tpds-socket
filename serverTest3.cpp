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
    timeval start, end;
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

    listen(socket_fd, 30);

    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    int fd = accept(socket_fd, (struct sockaddr*)&client, &len);
    //gettimeofday(&start, NULL); //start time
    if (fd == -1) {
        cout << "accept error!" << endl;
        exit(-1);
    }

    char *ip = inet_ntoa(client.sin_addr);
    cout << "client: [" << ip << "] successfully connected!" << endl;

    write(fd, "welcome", 7);
    
    char buffer[1024];
    Message message;
    int size = read(fd, buffer, sizeof(buffer));
    memcpy(&message, buffer, sizeof(buffer));
    //gettimeofday(&end, NULL); // end time
    
    cout << "bytes received: " << size << endl;
    cout << "content: " << message.type << endl;
    cout << "content: " << message.content << endl;
    //cout << "Total time: " << ((end.tv_usec - start.tv_usec) + (end.tv_sec - start.tv_sec) * 1000000) << "us" << endl;

    close(fd);
    close(socket_fd);
    return 0;
}