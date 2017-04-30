#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#define SERVER_PORT 12345
#define MAX_LINE 256

int main(int argc, char * argv[])
{
    struct hostent *host_address;
    struct sockaddr_in socket_address;
    char *host;
    char buf[MAX_LINE];
    int s;
    int len;

    // /* verificação de argumentos */
    // (...)

    // /* tradução de nome para endereço IP */
    // (...)

    // /* criação da estrutura de dados de endereço */
    // bzero((char *)&socket_address, sizeof(socket_address));
    // (...)

    //     /* criação de socket ativo*/
    // (...)

    // /* estabelecimento da conexão */
    // (...)

    //     /* ler e enviar linhas de texto, receber eco */
    // (...)

    return 0;
}