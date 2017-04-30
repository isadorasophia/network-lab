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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#define LISTEN_PORT 12345
#define MAX_PENDING 5
#define MAX_LINE 256

#define ERROR -1

#define ever (;;)

/* valid a status and report any errors found */
void valid(int status, const char* msg) {
    if (status == ERROR) {
        fprintf(stderr, "%s", msg);
        exit(EXIT_FAILURE);
    }
}

int main()
{
    struct sockaddr_in socket_addr;
    
    char buff[MAX_LINE];
    uint32_t len;
    int32_t s;

    /* initialize data address */
    bzero((char*) &socket_addr, sizeof(socket_addr));
    socket_addr.sin_family      = AF_INET;              /* ipv4 addresses */
    socket_addr.sin_port        = htons(LISTEN_PORT);
    socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);

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
        int conn = accept(s, (struct sockaddr*) NULL, NULL);
        valid(conn, "Failed to establish a connection from socket.\n");

        for ever {
            int res = recv(conn, buff, MAX_LINE, 0);
            valid(res, 
                "Failed to receive any message from my connection!\n");

            /* check if connection was terminated */
            if (res == 0) {
                fprintf(stdout, "Connection closed!\n");
                break; /* wait for another client */
            }

            /* terminate buff based on size */
            buff[res] = '\0';

            /* print text on screen */
            fprintf(stdout, "%s\n", buff);

            /* send back to client */
            if (send(conn, buff, res, 0) == ERROR) {
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