/* *
 *  server side!
 *      written by: Isadora Sophia Garcia Rodopoulos       158018
 *                  Matheus Mortatti Diamantino            156740
 *
 *      server:     receives messages requests from a client;
 *                  prints message on screen;
 *                  sends received message requests back to client.
 *
 * */

#include "api.h"

#define CLIENT_PORT 12345
#define MAX_PENDING 5

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

    fprintf(stdout, "%s\n-> Welcome to the server side of our application! \n\
    Please enjoy as you watch our client contacting with us!\n\
    We are currently available at: localhost.\n", 
            art);
}

int main() {
    struct sockaddr_in socket_addr;
    
    char buff[MAX_LINE];
    int32_t s;

    welcome();
    wait();

    /* initialize data address */
    bzero((char*) &socket_addr, sizeof(socket_addr));
    socket_addr.sin_family      = AF_INET;              /* ipv4 addresses */
    socket_addr.sin_port        = htons(CLIENT_PORT);
    socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);    /* localhost */

    /* create passive socket */
    s = socket(AF_INET, SOCK_STREAM, 0);
    valid(s, "Failed to create an endpoint for communication!\n");

    /* assign socket to a descriptor */
    valid(bind(s, (struct sockaddr*) &socket_addr, 
             sizeof(socket_addr)),
            "Failed when binding socket to a name!\n");

    /* allow socket to accept connections */
    valid(listen(s, MAX_PENDING),
            "Failed to listen for socket connections.\n");

    /* wait for connections and do my job! */
    for ever {
        int32_t conn = accept(s, (struct sockaddr*) NULL, NULL);
        valid(conn, "Failed to establish a connection from socket.\n");

        for ever {
            int32_t len = recv(conn, buff, MAX_LINE, 0);
            valid(len, 
                "Failed to receive any message from my connection!\n");

            /* check if connection was terminated */
            if (len == 0) {
                fprintf(stdout, "Connection closed!\n");
                break; /* wait for another client */
            }

            /* set end on buff based on size */
            buff[len] = '\0';

            /* print text on screen */
            fprintf(stdout, "<- %s", buff);

            /* send back to client */
            if (send(conn, buff, len, 0) == ERROR) {
                printf("Failed to send echo to client!\n");
                break;
            }
        }

        close(conn);
    }

    /* kthanxbye! */
    close(s);
    return 0;
}