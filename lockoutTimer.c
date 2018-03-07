#ifndef LOCKOUTTIMER_H_
#define LOCKOUTTIMER_H_
 
// The lockoutTimer is active for 1/2 second once it is started.
// It is used to lock-out the detector once a hit has been detected.
// This ensure that only one hit is detected per 1/2-second interval.
 
#define LOCKOUT_TIMER_EXPIRE_VALUE 50000	// Defined in terms of 100 kHz ticks.
#define INIT 0
#define CLK0 0
#define TIMERNUM 2

#include <stdint.h>
#include <stdio.h>
#include "supportFiles/intervalTimer.h"

static uint16_t count = INIT;
static bool running = false;
static bool lockout_flag = false;

static enum lockout_st_t {
	init_st,
	lockout_st,
} currentState;


// Standard init function.
void lockoutTimer_init() {
	currentState = init_st;
	lockout_flag = false;
	running = false;
}
 
// Calling this starts the timer.
void lockoutTimer_start() {
	lockout_flag = true;
	running = true;
}
 
// Returns true if the timer is running.
bool lockoutTimer_running() {
	return(running);
}
 
// Standard tick function.
void lockoutTimer_tick() {
	switch(currentState) {
	case init_st:
		if (lockout_flag) {
			currentState = lockout_st;
			running = true;
			count = INIT;
		}
		break;
	case lockout_st:
		if (count > LOCKOUT_TIMER_EXPIRE_VALUE) {
			running = false;
			lockout_flag = false;
			currentState = init_st;
		}
		break;
	default:
    	printf("lockout state_transtion: hit default case\n\r");
    	break;

	}

	switch(currentState) {
	case init_st:
		break;
	case lockout_st:
		count++;
		break;
	default:
    	printf("Trigger state_transtion: hit default case\n\r");
    	break;
	}
}
 
// Test function.
void lockoutTimer_runTest() {
    intervalTimer_reset(CLK0);
    intervalTimer_init(CLK0);
    intervalTimer_start(CLK0);
    lockoutTimer_start();
    while(lockoutTimer_running()) {}
    intervalTimer_stop(CLK0);
    printf("time in lockout: %lf\n", intervalTimer_getTotalDurationInSeconds(CLK0));
}
 
#endif /* LOCKOUTTIMER_H_ */
