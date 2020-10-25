// This program receives a signal and exits.

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef __cplusplus
#include <threads.h>
#else
#include <thread>
#endif

volatile sig_atomic_t signal_status = 0;

#ifdef __cplusplus
extern "C"
#endif
void signal_handler(int signal)
{
	signal_status = signal;
}

int main(void)
{
	void (*res)(int) = signal(SIGINT, signal_handler);
	if (res == SIG_ERR) {
		fprintf(stderr, "Setting signal handler failed.\n");
		exit(-1);
	}

	while (signal_status == 0) {
		printf(".");
		fflush(stdout);

// Sleep
#ifndef __cplusplus
		if (thrd_sleep(&(struct timespec){.tv_sec = 0, .tv_nsec = 50L*1000L*1000L}, NULL) < -1) {
			fprintf(stderr, "Thread sleep failed.\n");
			exit(-1);
		}
#else
		using namespace std::literals::chrono_literals;
		std::this_thread::sleep_for(50ms);
#endif
	}

	printf("\nSignal %d received.\n", signal_status);

	return 0;
}
