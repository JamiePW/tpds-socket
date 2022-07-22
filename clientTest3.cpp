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

    cout << "enter anything to start..." << endl;
    cin >> temp;
    gettimeofday(&start, NULL); //start time

    int res = connect(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
    if (res == -1) {
        cout << "bind connection failed!" << endl;
        exit(-1);
    }
    cout << "bind connection success!" << endl;

    //char message[1024] = "hello Jamie!";
    Message message;
    memcpy(message.type, "test", sizeof("test"));
    memcpy(message.content, "hello Jamie!", sizeof("hello Jamie!"));
    write(socket_fd, (char*)&message, sizeof(Message));

    char buffer[1024] = {};
    int size = read(socket_fd, buffer, sizeof(buffer));
    cout << buffer << endl;
    gettimeofday(&end, NULL); // end time

    cout << "Total time: " << ((end.tv_usec - start.tv_usec) + (end.tv_sec - start.tv_sec) * 1000000) << "us" << endl;

    close(socket_fd);

    return 0;
}