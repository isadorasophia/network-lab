#include <time.h>
#include <stdio.h>

int main() {
	time_t t;

	time(&t);
	printf("%ld\n", t);
	return 0;
}