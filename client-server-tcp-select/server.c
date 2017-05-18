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

#define LISTEN_PORT 12345
#define MAX_PENDING 5
#define MAX_LINE 256

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

typedef struct {
    int descriptor;
    int port;
    int used;
} Client;

int main() {
    struct sockaddr_in socket_addr;
    char buff[MAX_LINE];
    unsigned int len;
    int i, s, new_s, sockfd, total_clients, maxfd, nready;
    Client clients[FD_SETSIZE];
    fd_set all_fds, new_set;
    char so_addr[INET_ADDRSTRLEN];
    char ip[INET_ADDRSTRLEN+1];

    welcome();
    wait();

    /* initialize data address */
    bzero((char*) &socket_addr, sizeof(socket_addr));
    socket_addr.sin_family      = AF_INET;              /* ipv4 addresses */
    socket_addr.sin_port        = htons(LISTEN_PORT);
    socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);    /* localhost */

    /* create active socket */
    s = socket(AF_INET, SOCK_STREAM, 0);
    valid(s, "Failed to create an endpoint for communication!\n");

    valid(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)),
          "setsockopt(SO_REUSEADDR) failed");
    valid(setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &(int){ 1 }, sizeof(int)),
          "setsockopt(SO_REUSEPORT) failed");

    /* assign socket to a descriptor */
    valid(bind(s, (struct sockaddr*) &socket_addr, 
             sizeof(socket_addr)),
            "Failed when binding socket to a name!\n");

    /* allow socket to accept connections */
    valid(listen(s, MAX_PENDING),
            "Failed to listen for socket connections.\n");

    maxfd = s;
    total_clients = -1;
    for (i = 0; i < FD_SETSIZE; i++) {
        clients[i].used = false;
    }
    
    FD_ZERO(&all_fds);
    FD_SET(s, &all_fds);

    /* wait for connections and do my job! */
    for ever {
        new_set = all_fds;
        nready = select(maxfd+1, &new_set, NULL, NULL, NULL);
        valid(nready, "Select function failed!\n");

        if (FD_ISSET(s, &new_set)) {
            len = sizeof(socket_addr);
            valid((new_s = accept(s, (struct sockaddr *)&socket_addr, &len)), 
                  "Failed to establish a connection from socket.\n");

            /* connect to client and show IP and Port of the connection */
            struct sockaddr_in client;
            socklen_t client_size = sizeof(client);

            int res = getpeername(new_s, (struct sockaddr *)&client, &client_size);
            valid(res, "Couldn't get address of the peer connected to socket.\n");

            res = inet_ntop(AF_INET, &(client.sin_addr), ip, INET_ADDRSTRLEN);
            valid(res, "Could not convert address between binary and text form.\n");

            ip[INET_ADDRSTRLEN] = '\0';

            /* print text on screen */
            fprintf(stdout, "<- IP: %s PORT: %d Just Connected!\n", ip, 
                ntohs(client.sin_port));

            /* check available spot */
            for (i = 0; i < FD_SETSIZE; i++) {
                if (!clients[i].used) {
                    clients[i].descriptor = new_s;
                    clients[i].port = ntohs(client.sin_port);
                    clients[i].used = true;

                    break;
                }
            }
            
            if (i == FD_SETSIZE) {
                error("Max. number of clients reached!");
            }
            
            /* add new descriptor to our set */
            FD_SET(new_s, &all_fds);
            
            /* set select */
            if (new_s > maxfd) {
                maxfd = new_s;
            }
            
            if (i > total_clients) {
                total_clients = i; 
            }
            
            /* no more descriptors to be read */
            if (--nready <= 0) {
                continue;
            }
        }

        for (i = 0; i <= total_clients; i++) {  /* verify if there is data for all clients */
            if (!clients[i].used) {
                continue;
            }

            sockfd = clients[i].descriptor;
            if (FD_ISSET(sockfd, &new_set)) {
                len = recv(sockfd, buff, sizeof(buff), 0);
                valid(len, 
                      "Failed to receive any message from my connection!\n");

                if (len == 0) {
                    fprintf(stdout, "Connection closed!\n");
                    close(sockfd);
                    FD_CLR(sockfd, &all_fds);

                    clients[i].used = false;
                } else {
                    /* print ip and port for each client, and send text back! */
                    /* set end on buff based on size */
                    buff[len] = '\0';

                    /* print message on screen and Destination IP*/
                    fprintf(stdout, "<- %s\tsent to IP: %s at port: %d\n", buff, 
                        ip, clients[i].port);

                    /* send back to client */
                    if (send(sockfd, buff, len, 0) == ERROR) {
                        fprintf(stdout, "Failed to send echo to client!\n");
                        break;
                    }
                }

                if (--nready <= 0) break; /* it isn't expecting any more 
                                           * descriptor to be read */ 
            }
        }

    }

    /* kthanxbye! */
    close(s);
    return 0;
}