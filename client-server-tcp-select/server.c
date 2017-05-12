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

int main() {
    struct sockaddr_in socket_addr;
    char buff[MAX_LINE];
    unsigned int len;
    int i, s, new_s, sockfd, cliente_num, maxfd, nready, clientes[FD_SETSIZE];
    fd_set todos_fds, novo_set;
    char so_addr[INET_ADDRSTRLEN];
    char ip[INET_ADDRSTRLEN+1];

    welcome();
    wait();

    /* initialize data address */
    bzero((char*) &socket_addr, sizeof(socket_addr));
    socket_addr.sin_family      = AF_INET;              /* ipv4 addresses */
    socket_addr.sin_port        = htons(LISTEN_PORT);
    socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);    /* localhost */

    /* create passive socket */
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
    cliente_num = -1;
    for (i = 0; i < FD_SETSIZE; i++)
        clientes[i] = -1;
    
    FD_ZERO(&todos_fds);
    FD_SET(s, &todos_fds);

    /* wait for connections and do my job! */
    for ever {

        novo_set = todos_fds;
        nready = select(maxfd+1, &novo_set, NULL, NULL, NULL);
        valid(nready, "Select function failed\n");

        if(FD_ISSET(s, &novo_set)) {
            len = sizeof(socket_addr);
            valid((new_s = accept(s, (struct sockaddr *)&socket_addr, &len)), 
                  "Failed to establish a connection from socket.\n");

            /* Connect to client and show IP and Port of the connection */
            struct sockaddr_in client;
            socklen_t client_size = sizeof(client);

        
            int res = getpeername(new_s, (struct sockaddr *)&client, &client_size);

            inet_ntop(AF_INET, &(client.sin_addr), ip, INET_ADDRSTRLEN);
            ip[INET_ADDRSTRLEN] = '\0';

            /* print text on screen */
            fprintf(stdout, "<- IP: %s PORT: %d Just Connected!\n", ip, ntohs(client.sin_port));

            for (i = 0; i < FD_SETSIZE; i++) {
                if (clientes[i] < 0) {
                    clientes[i] = new_s;    //guarda descritor
                    break;
                }
            }
            
            if (i == FD_SETSIZE) {
                perror("Numero maximo de clientes atingido.");
                        exit(1);
            }
            FD_SET(new_s, &todos_fds);      // adiciona novo descritor ao conjunto
            if (new_s > maxfd)
                maxfd = new_s;          // para o select
            if (i > cliente_num)
                cliente_num = i;        // Ã­ndice mÃ¡ximo no vetor clientes[]
            if (--nready <= 0)
                continue;           // nÃ£o existem mais descritores para serem lidos
        }

        for (i = 0; i <= cliente_num; i++) {    // verificar se hÃ¡ dados em todos os clientes
            if ( (sockfd = clientes[i]) < 0)
                continue;
            if (FD_ISSET(sockfd, &novo_set)) {
                len = recv(sockfd, buff, sizeof(buff), 0);
                valid(len, 
                      "Failed to receive any message from my connection!\n");

                if ( len == 0) {
                    //conexÃ£o encerrada pelo cliente

                    fprintf(stdout, "Connection closed!\n");
                    close(sockfd);
                    FD_CLR(sockfd, &todos_fds);
                    clientes[i] = -1;
                } else {
                    /* imprime ip e porta do cliente e envia texto de volta */
                    /* set end on buff based on size */
                    buff[len] = '\0';

                    /* print message on screen and Destination IP*/
                    fprintf(stdout, "<- %s   Sent To: %s\n", buff, ip);

                    /* send back to client */
                    if (send(new_s, buff, len, 0) == ERROR) {
                        printf("Failed to send echo to client!\n");
                        break;
                    }
                }
                if (--nready <= 0)
                    break;              // nÃ£o existem mais descritores para serem lidos
            }
        }

    }

    /* kthanxbye! */
    close(s);
    return 0;
}