#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

bool enabled;
struct itimerval itimer;
volatile int signal_count = 0;
struct timeval prev_time;

void preempt_disable(void)
{
	enabled = false;
	sigset_t mask;
	sigemptyset(&mask);					 // Initialize the signal mask
	sigaddset(&mask, SIGVTALRM);		 // Add SIGVTALRM to the mask
	sigprocmask(SIG_BLOCK, &mask, NULL); // Block the signal
	
}

void preempt_enable(void)
{
	enabled = true;
	sigset_t mask;
	sigemptyset(&mask);					   // Initialize the signal mask
	sigaddset(&mask, SIGVTALRM);		   // Add SIGVTALRM to the mask
	sigprocmask(SIG_UNBLOCK, &mask, NULL); // Unblock the signal
	
}

void signal_handler()
{
	struct timeval current_time;
	gettimeofday(&current_time, NULL);
	// tester to verify time interval working properly
	// smaller intervals were less consistent but testing with interval 100k instead of 10k gave very consistent results
	//int diff = (current_time.tv_sec - prev_time.tv_sec) * 1000000 + (current_time.tv_usec - prev_time.tv_usec);
	//printf("signal time diff: %d microsecs\n", diff);
	prev_time = current_time;
	signal_count++;

	uthread_yield();
}

void preempt_start(bool preempt)
{
	if (preempt && enabled)
	{
		struct sigaction sa;
		sa.sa_handler = signal_handler;
		sa.sa_flags = 0;
		sigaction(SIGVTALRM, &sa, NULL);

		// initialize timer HZ times per second (1 mil microseconds)
		// same interval for each new timer
		itimer.it_value.tv_usec = 1000000 / HZ;
		itimer.it_interval.tv_usec = 1000000 / HZ;

		setitimer(ITIMER_VIRTUAL, &itimer, NULL);
	}
}

void preempt_stop(void)
{
	struct sigaction sa;
	sa.sa_handler = NULL;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGVTALRM, &sa, NULL);

	itimer.it_value.tv_sec = 0;
	itimer.it_value.tv_usec = 0;
	itimer.it_interval.tv_sec = 0;
	itimer.it_interval.tv_usec = 0;

	setitimer(ITIMER_VIRTUAL, &itimer, NULL);
}
