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
    Watch me collide with other cars and die!\n", 
            art);
}

void wait() {
    fprintf(stdout, "-> ");
}

time_t time_passed(time_t t) {
    return time(0) - t;
}

int main(int argc, char *argv[])
{
    time_t current_time, wait_time, update_time, send_time;
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

    /* Get first input */
    Car my_car;
    int n_inputs, inputs_read = 1;

    scanf("%d %d %d %d", &n_inputs, &my_car.size, &my_car.x, &my_car.y);
    scanf("%d %d %ld", &my_car.vx, &my_car.vy, &wait_time);

    int32_t counter = 0;
    time(&current_time);
    time(&update_time);
    time(&send_time);
    for ever {
        // Update parameters
        my_car.cur_time = time(0);
        if(time_passed(update_time) >= 1) {
            update_car(&my_car);
            time(&update_time);
        }
        if(inputs_read < n_inputs && time_passed(current_time) >= wait_time) {
            inputs_read++;

            time(&current_time);
            scanf("%d %d %ld", &my_car.vx, &my_car.vy, &wait_time);
        }

        // After X seconds, send information
        if(time_passed(send_time) >= 1) {
            time(&send_time);

            /* send message */
            valid(send(s, (char *)&my_car, sizeof(my_car), 0),
                    "Failed to send any message from my connection!\n");

            /* receive command */
            int command;
            int32_t len = recv(s, &command, sizeof(int), 0);
            valid(len, "Failed to receive any messages from my connection!\n");

            if (len == 0) {
                fprintf(stdout, "Connection was closed!\n");
                break;
            }

            fprintf(stdout, "-> Time: %ld, X = %d, Y = %d, VX = %d, VY = %d\n", time(0), my_car.x, my_car.y, my_car.vx, my_car.vy);

            if(command == BREAK) {
                my_car.vx = 0;
                my_car.vy = 0;

                /* print response on screen */
                fprintf(stdout, "<- %s\n", "*TRYING NOT TO DIE*");
            }
            else if(command == ACCELERATE) {
                fprintf(stdout, "<- %s\n", "Everything is ok");   
            }
            else if(command == AMBULANCE) {
                my_car.vx = 0;
                my_car.vy = 0;

                fprintf(stdout, "<- %s\n", "*DED*");

                break;
            }
        }

    }

    close(s);
    return 0;
}