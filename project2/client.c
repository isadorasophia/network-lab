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

/* * macros! * */
#define READ(str, res)  {                               \
                            printf("\t-> %s:: ", str);  \
                            scanf("%d", &res);          \
                        }
#define READS(str, res) {                               \
                            printf("\t-> %s:: ", str);  \
                            scanf("%ld", &res);         \
                            res = res * 1000;           \
                        }
#define READENUM(str, res) {                            \
                            printf("\t-> %s:: ", str);  \
                            int dummy;                  \
                            scanf("%d", &dummy);        \
                            res = dummy;                \
                           }

#define MILLISEC(d) (int64_t)(d.tv_sec*1000 + d.tv_nsec / 1.0e6)

/* * helpers! * */
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

/* * return time passed since t, in millissec * */
int64_t time_passed(struct timespec start, struct timespec end) {
    return MILLISEC(end) - MILLISEC(start);
}

/* * returns latency of a given car * */
int latency(Type type) {
    return type == SECURITY ? HIGH : LOW;
}

int main(int argc, char *argv[])
{
    int64_t update_counter = 0;
    struct timespec update_t, sync_t, last_sent_security, last_sent_other;

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

    /* get socket IP and Port and show to client */
    struct sockaddr_in client;
    socklen_t client_size = sizeof(client);

    int res = getsockname(s, (struct sockaddr *)&client, &client_size);

    /* get first input */
    Car my_car;
    int n_inputs = 0, inputs_read = 0;

    /* technical */
    READ("# of changes", n_inputs)

    /* if we want to make changes... */
    if (n_inputs > 0) {
        READS("# of time before update (s)", update_counter);
    }

    READ("Car length", my_car.size)
    READ("Position x", my_car.x)
    READ("Position y", my_car.y)

    READ("Velocity+direction x", my_car.vx)
    READ("Velocity+direction y", my_car.vy)

    /* * connect and go! * */ 
    char ip[INET_ADDRSTRLEN+1];
    inet_ntop(AF_INET, &(client.sin_addr), ip, INET_ADDRSTRLEN);
    ip[INET_ADDRSTRLEN] = '\0';

    fprintf(stdout, "<- IP: %s PORT: %d Connected!\n", ip, 
        ntohs(client.sin_port));

    /* set our timings */
    int32_t counter = 0;
    clock_gettime(CLOCK_REALTIME, &sync_t);
    clock_gettime(CLOCK_REALTIME, &update_t);
    clock_gettime(CLOCK_REALTIME, &last_sent_security);
    clock_gettime(CLOCK_REALTIME, &last_sent_other);

    for ever {
        /* update parameters! */
        clock_gettime(CLOCK_REALTIME, &my_car.cur_time);

        if (inputs_read < n_inputs && 
                time_passed(update_t, my_car.cur_time) >= update_counter) {
            update_t = my_car.cur_time;

            inputs_read++;

            READ("Velocity x", my_car.vx)
            READ("Velocity y", my_car.vy)
            READS("# of time before update (s)", update_counter)

            fprintf(stdout, "\t[[You got %d updates left!]]\n", n_inputs-inputs_read);
        }

        /** clock syncs **/

        if (time_passed(sync_t, my_car.cur_time) >= CLOCK) {
            sync_t = my_car.cur_time;

            update_car(&my_car);
        }

        /* check for security packets */
        if (time_passed(last_sent_security, my_car.cur_time) >= latency(SECURITY)) {
            last_sent_security = my_car.cur_time;
            my_car.type = SECURITY;

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

            fprintf(stdout, "-> Time: %lds, X = %d, Y = %d, VX = %d, VY = %d\n", 
                MILLISEC(last_sent_security), my_car.x, my_car.y, my_car.vx, my_car.vy);

            if (command == BREAK) {
                my_car.vx = 0;
                my_car.vy = 0;

                /* print response on screen */
                fprintf(stdout, "<- %s\n", "*TRYING NOT TO DIE*");

            } else if (command == ACCELERATE) {
                fprintf(stdout, "<- %s\n", "Everything is ok");

            } else if (command == AMBULANCE) {
                my_car.vx = 0;
                my_car.vy = 0;

                fprintf(stdout, "<- %s\n", "*DED*");

                break;
            }
        }

        /* some other useless updates */
        if (time_passed(last_sent_other, my_car.cur_time) >= latency(OTHER)) {
            last_sent_other = my_car.cur_time;

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

            /* whatever */
        }
    }

    /* clean up our mess */
    close(s);

    return 0;
}