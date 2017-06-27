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
    Please enjoy as you watch the cars explode!!\n\
    We are currently available at: localhost.\n", 
            art);
}

typedef struct {
    int descriptor;
    int port;
    int used;
} Client;

int sign(int a) {
    if(a > 0)  return 1;
    if(a == 0) return 0;
    if(a < 0)  return -1;
}

/* Check for car collision and return command */
int check_collision(Client clients[], int s, Car cars[], int size) {
    int command;
    int i, j;

    /* Update cars */
    for(i = 0; i < size; i++) {
        time_t t = time(0) - cars[i].cur_time;
        for(j = 0; j < t; j++)
            update_car(&cars[i]);
    }

    /* Check for collision */
    for(i = 0; i < size; i++) {
        if(!clients[i].used || i == s) continue;

        Car target, source;

        if(cars[s].vx == 0 && cars[i].vy == 0) {
            target = cars[i];
            source = cars[s];
        }
        else if (cars[s].vy == 0 && cars[i].vx == 0) {
            target = cars[s];
            source = cars[i];
        }
        else continue; // if they are going in the same direction

        int x = source.x, y = target.y, cond_s, cond_t;

        // Check if already collided
        if(source.vy > 0)
            cond_s = source.y >= y && source.y <= y + source.size;
        else
            cond_s = source.y <= y && source.y >= y + -1 * source.size;

        if(target.vx > 0)
            cond_t = target.x >= x && target.x <= x + target.size;
        else
            cond_t = target.x <= x && target.x >= x + -1 * target.size;

        if(cond_s && cond_t)
            return AMBULANCE;

        // Check if they will collide

        // Source
        int dy = y - source.y;
        time_t time_in_s = source.vy == 0 ? 0 : dy / source.vy;

        dy = (y + sign(source.vy)*source.size) - source.y;
        time_t time_out_s = source.vy == 0 ? 0 : dy / source.vy;

        // Target
        int dx = x - target.x;
        time_t time_in_t = target.vx == 0 ? 0 : dx / target.vx;

        dx = (x + sign(target.vx)*target.size) - target.x;
        time_t time_out_t = target.vx == 0 ? 0 : dx / target.vx;

        // fprintf(stdout, "*******  %d %d %d %d \n", 
        //                 time_in_s, time_out_s, time_in_t, time_out_t);
        // Will collide
        if(time_in_s <= time_out_t && time_in_t <= time_out_s) 
            return BREAK;
    }

    return ACCELERATE;
}

int main() {
    struct sockaddr_in socket_addr;
    char buff[MAX_LINE];
    unsigned int len;
    int i, s, new_s, sockfd, total_clients, maxfd, nready;
    Client clients[FD_SETSIZE];
    Car cars[FD_SETSIZE];
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

                    /* Set new car's size to zero so it doesn't collide *
                     * while it doesn't send first information          */
                    cars[i].size = 0;

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
                len = recv(sockfd, &cars[i], sizeof(cars[i]), 0);
                valid(len, 
                      "Failed to receive any message from my connection!\n");

                if (len == 0) {
                    fprintf(stdout, "Connection closed!\n");
                    close(sockfd);
                    FD_CLR(sockfd, &all_fds);

                    clients[i].used = false;
                } else {

                    int command = check_collision(clients, i, cars, total_clients);

                    /* print message on screen and Destination IP*/
                    fprintf(stdout, "<- %d\tsent to IP: %s at port: %d\n", command, 
                        ip, clients[i].port);

                    /* send back to client */
                    if (send(sockfd, (char *)&command, sizeof(int), 0) == ERROR) {
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
