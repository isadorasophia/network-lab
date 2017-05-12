/* *
 *  client side!
 *      written by: Isadora Sophia Garcia Rodopoulos       158018
 *                  Matheus Mortatti Diamantino            156740
 *
 *      server:     sends messages requests to server
 *
 * */

#include "api.h"

#define SERVER_PORT 12345

void welcome() {
    char* art = 
"                                     /~\\ \n\
                                    |oo )  \n\
                                    _\\=/_ \n\
                    ___        #   /  _  \\ \n\
                   / ()\\        \\//|/.\\|\\ \n\
                 _|_____|_       \\/  \\_/  || \n\
                | | === | |         |\\ /| || \n\
                |_|  O  |_|         \\_ _/ # \n\
                 ||  O  ||          | | | \n\
                 ||__*__||          | | | \n\
                |~ \\___/ ~|         []|[] \n\
                /=\\ /=\\ /=\\         | | | \n\
________________[_]_[_]_[_]________/_]_[_\\________________\n";

    fprintf(stdout, "%s\n-> Welcome to the client side of our application! \n\
    You should type for messages and our server will get answers for you!\n", 
            art);
}

void wait() {
    fprintf(stdout, "-> ");
}

int main(int argc, char *argv[])
{
    struct hostent *host_address;
    char *host;

    struct sockaddr_in socket_addr;
    int32_t s;

    char buff[MAX_LINE];

    /* make sure arguments are fine */
    if (argc != 2) {
        error("Incorrect usage! Please, call me by:  \n\
        ./client <hostname>\n");
    } else {
        host = argv[1];
    }

    welcome();

    /* get ip address */
    host_address = gethostbyname(host);
    if (host_address == NULL) {
        error("Invalid host name!\n");
    }

    /* initialize data address */
    bzero((char*) &socket_addr, sizeof(socket_addr));
    socket_addr.sin_family      = AF_INET;              /* ipv4 addresses */
    socket_addr.sin_port        = htons(SERVER_PORT);
    socket_addr.sin_addr = *(struct in_addr*)host_address->h_addr;

    /* create active socket */
    s = socket(AF_INET, SOCK_STREAM, 0);

    valid(connect(s, (struct sockaddr*) &socket_addr, 
                            sizeof(socket_addr)), 
            "Failed to establish a connection from socket.\n");

    /* Get socket IP and Port and show to client */
    struct sockaddr_in client;
    socklen_t client_size = sizeof(client);

    int res = getsockname(s, (struct sockaddr *)&client, &client_size);

    char ip[INET_ADDRSTRLEN+1];
    inet_ntop(AF_INET, &(client.sin_addr), ip, INET_ADDRSTRLEN);
    ip[INET_ADDRSTRLEN] = '\0';

    /* print text on screen */
    fprintf(stdout, "<- IP: %s PORT: %d Connected!\n", ip, ntohs(client.sin_port));

    int32_t counter = 0;
    for ever {
        wait();

        /* read text lines from user */
        fgets(buff, MAX_LINE, stdin);

        /* send message */
        valid(send(s, buff, strlen(buff), 0),
                "Failed to send any message from my connection!\n");

        /* receive message */
        int32_t len = recv(s, buff, MAX_LINE, 0);
        valid(len, "Failed to receive any messages from my connection!\n");

        if (len == 0) {
            fprintf(stdout, "Connection was closed!\n");
            break;
        }

        /* set end on buff based on size */
        buff[len] = '\0';

        /* print response on screen */
        fprintf(stdout, "<- %s", buff);
    }

    close(s);
    return 0;
}