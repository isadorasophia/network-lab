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
#include <time.h>

#define MAX_LINE 256

#define BREAK       1
#define ACCELERATE  2
#define AMBULANCE   3

#define ERROR -1
#define ever (;;)

#define true 1
#define false 0
#define bool int

typedef struct {
	time_t cur_time;
    int size, x, y, vx, vy;
} Car;

void update_car(Car *car) {
    car->x += car->vx;
    car->y += car->vy;
}

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
