#include "common.h"

using namespace std;

//1KB string  to be sent
char temp[1025] = "Here\'s to the crazy ones. The misfits. The rebels. The troublemakers. The round pegs in the square holes. The ones who see things differently. They\'re not fond of rules. And they have no respect for the status quo. You can quote them, disagree with them, glorify or vilify them. But the only thing you can\'t do is ignore them. Because they change things. They invent. They imagine. They heal. They explore. They create. They inspire. They push the human race forward. Maybe they have to be crazy. How else can you stare at an empty canvas and see a work of art? Or sit in silence and hear a song that\'s never been written? Or gaze at a red planet and see a laboratory on wheels? We make tools for these kinds of people. While some see them as the crazy ones, we see genius. Because the people who are crazy enough to think they can change the world, are the ones who do. Here\'s to the crazy ones. The misfits. The rebels. The troublemakers. The round pegs in the square holes. The ones who see things differently. They\'re n";

int main() {
    char buffer[2048];

    int socket_fd = _bindSock(localAddr, 10041);
    if (socket_fd == -1) exit(-1);

    int fd = _listenSock(socket_fd);

    int size = read(fd, buffer, sizeof(buffer));
    Message message;
    memcpy(&message, buffer, sizeof(buffer));

    if (strcmp(message.type, messageType[1]) == 0) {
        _sendMessage(fd, messageType[3], temp, NULL, NULL);
    }

    close(socket_fd);

    return 0;
}