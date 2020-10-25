// This program first displays the current time, and then measures the CPU time spent in a loop.

#include <stdio.h>
#include <time.h>

int show_current_time(void)
{
	time_t t = time(NULL);
	if (t == (time_t)-1) {
		fprintf(stderr, "Getting time failed.\n");
		return -1;
	}

	struct tm *utc = gmtime(&t);
	if (utc == NULL) {
		fprintf(stderr, "Converting time failed.\n");
		return -1;
	}
	printf("UTC:   %s", asctime(utc));

	struct tm *local = localtime(&t);
	if (local == NULL) {
		fprintf(stderr, "Converting time failed.\n");
		return -1;
	}
	printf("Local: %s", asctime(local));

	return 0;
}

int measure_cpu_time(void)
{
	printf("Iterating...\n");

	clock_t c1 = clock();
	if (c1 == (clock_t)-1) {
		fprintf(stderr, "Getting CPU time failed.\n");
		return -1;
	}

	volatile int sum = 0;
	for (int i=0; i<1000*1000*1000; ++i)
		sum += 1;

	clock_t c2 = clock();
	if (c2 == (clock_t)-1) {
		fprintf(stderr, "Getting CPU time failed.\n");
		return -1;
	}

	double duration = (double)(c2-c1) / CLOCKS_PER_SEC;
	printf("CPU time used in the iteration: %.3fs.\n", duration);

	return 0;
}

int main(void)
{
	show_current_time();
	measure_cpu_time();
	return 0;
}
