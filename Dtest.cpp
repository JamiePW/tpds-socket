//D is only a server
//address: localhost
//port: 10041
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

//1KB string  to be sent
char temp[1025] = "Here\'s to the crazy ones. The misfits. The rebels. The troublemakers. The round pegs in the square holes. The ones who see things differently. They\'re not fond of rules. And they have no respect for the status quo. You can quote them, disagree with them, glorify or vilify them. But the only thing you can\'t do is ignore them. Because they change things. They invent. They imagine. They heal. They explore. They create. They inspire. They push the human race forward. Maybe they have to be crazy. How else can you stare at an empty canvas and see a work of art? Or sit in silence and hear a song that\'s never been written? Or gaze at a red planet and see a laboratory on wheels? We make tools for these kinds of people. While some see them as the crazy ones, we see genius. Because the people who are crazy enough to think they can change the world, are the ones who do. Here\'s to the crazy ones. The misfits. The rebels. The troublemakers. The round pegs in the square holes. The ones who see things differently. They\'re n";

int main() {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        cout << "socket init failed!" << endl;
        exit(-1);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(10041);
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

    if (strcmp(message.type, "object") == 0) {
        //write(fd, "the type is request, processing...", 34);
        //search hashmap
        Message key;
        memcpy(key.type, "object", sizeof("object"));
        memcpy(key.content, temp, sizeof(temp));
        write(fd, (char*)&key, sizeof(Message));
    }

    close(fd);
    close(socket_fd);
    return 0;
}