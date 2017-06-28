/* *
 *  basic api!
 *      written by: Isadora Sophia Garcia Rodopoulos       158018
 *                  Matheus Mortatti Diamantino            156740
 *
 *      api:        enable some helper functions and parameters for
 *                  client and server both.     
 *
 * */

#include <ncurses.h>
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
#define HORIZONTAL	4
#define VERTICAL	5

#define CLOCK    1000   /* speed will be updated here */

#define ERROR -1
#define ever (;;)

#define true 1
#define false 0
#define bool int

typedef enum { LOW=9, HIGH=100 } Latency;   /* latency of each packet */
typedef enum { SECURITY=1, ENTERTAINMENT=2, COMFORT=3, OTHER=4 } Type;

typedef struct {
	struct timespec cur_time;
    int size, x, y, vx, vy, dirx, diry;

    Type type;
} Car;

int sign(int a) {
    if (a > 0)  return 1;
    if (a == 0) return 0;
    if (a < 0)  return -1;
}

void update_car(Car *car) {
    car->x += car->vx;
    car->y += car->vy;
}

/* * update car for /n/ iterations */
void update_car_n(Car *car, int n) {
    car->x += n*car->vx;
    car->y += n*car->vy;
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
