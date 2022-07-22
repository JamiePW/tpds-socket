//A is only a client
#include<stdio.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string>
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

//the hash of A's target, SHA256 of "D"
char target[128] = "3f39d5c348e5b79d06e842c114e6cc571583bbf44e4b0ebfda1a01ec05745d43";

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

    cout << "enter anything to start..." << endl; //mark the start time
    cin >> temp;
    gettimeofday(&start, NULL); //start time

    /*
    **request B for key of D
    */

    int res = connect(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
    if (res == -1) {
        cout << "bind connection failed!" << endl;
        exit(-1);
    }
    cout << "bind connection success!" << endl;

    //char message[1024] = "hello Jamie!";
    Message message;
    memcpy(message.type, "request", sizeof("request"));
    memcpy(message.content, target, sizeof(target));
    write(socket_fd, (char*)&message, sizeof(Message));

    Message reply;
    char buffer[2048];
    int size = read(socket_fd, buffer, sizeof(buffer));
    //cout << buffer << endl;
    memcpy(&reply, buffer, sizeof(buffer));
    cout << "the key information are below:" << endl;
    cout << reply.type << endl;
    cout << reply.address << endl;
    cout << reply.port << endl;

    close(socket_fd);
    /*
    **connect server D
    */
    int socket_fd2 = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd2 == -1) {
        cout << "socket2 init failed!";
        exit(-1);
    }

    addr.sin_port = htons(10041);
    addr.sin_addr.s_addr = inet_addr("10.0.2.15");

    res = connect(socket_fd2, (struct sockaddr*)&addr, sizeof(addr));
    if (res == -1) {
        cout << "bind connection failed!" << endl;
        exit(-1);
    }
    cout << "bind connection success!" << endl;

    memcpy(message.type, "object", sizeof("object"));
    memcpy(message.content, "I need the object", sizeof("I need the object"));
    write(socket_fd2, (char*)&message, sizeof(Message));

    size = read(socket_fd2, buffer, sizeof(buffer));
    memcpy(&reply, buffer, sizeof(buffer));
    cout << "the object information are below:" << endl;
    cout << reply.type << endl;
    cout << reply.content << endl;

    close(socket_fd2);
    
    gettimeofday(&end, NULL); // end time
    cout << "Total time: " << ((end.tv_usec - start.tv_usec) + (end.tv_sec - start.tv_sec) * 1000000) << "us" << endl;

    return 0;
}