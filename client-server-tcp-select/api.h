/* *
 *  basic api!
 *      written by: Isadora Sophia Garcia Rodopoulos       158018
 *                  Matheus Mortatti Diamantino            156740
 *
 *      api:        enable some helper functions and parameters for
 *                  client and server both.     
 *
 * */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#define MAX_LINE 256

#define ERROR -1
#define ever (;;)

void error(const char* msg) {
    fprintf(stderr, "\t[ERROR] %s", msg);
    exit(EXIT_FAILURE);
}

/* valid a status and report any errors found */
void valid(int status, const char* msg) {
    if (status == ERROR) {
        error(msg);
    }
}
