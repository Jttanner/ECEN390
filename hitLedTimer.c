#ifndef HITLEDTIMER_H_
#define HITLEDTIMER_H_
 
// The hitLedTimer illuminates the LEDs (LD0 and the LED attached to pin JF-3) for 1/2 second when activated.
 
#define HIT_LED_TIMER_EXPIRE_VALUE 50000	// Defined in terms of 100 kHz ticks.
#define HIT_LED_TIMER_OUTPUT_PIN 11 // JF-3
#define LED_TEST_TIMER_MAX_VALUE 1

#define LED_ON 1
#define LED_OFF 0

#include <stdint.h>
#include <stdio.h>

#include "supportFiles/mio.h"
#include "supportFiles/utils.h"

void hitLedTimer_turnLedOn();
void hitLedTimer_turnLedOff();

static uint16_t ledTimerCounter;
static uint16_t testTimerCounter;
static bool timerStart;
static bool running;

static enum ledState{
    INIT_STATE,
    OFF_STATE,
    ON_STATE
} currState;



// Standard init function. Implement it even if it is not necessary. You may need it later.
void hitLedTimer_init(){
    currState = INIT_STATE;
    mio_init(false);  // false disables any debug printing if there is a system failure during init.
    mio_setPinAsOutput(HIT_LED_TIMER_OUTPUT_PIN);  // Configure the signal direction of the pin to be an output.
    ledTimerCounter = 0;
    testTimerCounter = 0;
    timerStart = false;
    running = false;
}
 
// Calling this starts the timer.
void hitLedTimer_start(){
    timerStart = true;
}
 
// Returns true if the timer is currently running.
bool hitLedTimer_running(){
    if (currState == ON_STATE){
        return true;
    } else{
        return false;
    }
}

void debugPrintState(){
    switch(currState){
        case INIT_STATE:
            printf("init state\n");
            break;
        case OFF_STATE:
            printf("off state\n");
            break;
        case ON_STATE:
            printf("on state\n");
            break;
        default:
            break;
    }
}
 
// Standard tick function.
void hitLedTimer_tick(){
    //debugPrintState();
    switch(currState){
        case INIT_STATE:
            currState = OFF_STATE;
            break;
        case OFF_STATE:
            if (timerStart){
                currState = ON_STATE;
                timerStart = false;
            }
            break;
        case ON_STATE:
            if (ledTimerCounter > HIT_LED_TIMER_EXPIRE_VALUE){
                ledTimerCounter = 0;
                currState = OFF_STATE;
            } else{
                currState = currState;
            }
            break;
        default:
            break;
    }

    switch(currState){
        case INIT_STATE:
            break;
        case OFF_STATE:
            running = false;
            hitLedTimer_turnLedOff();
            break;
        case ON_STATE:
            running = true;
            hitLedTimer_turnLedOn();
            ++ledTimerCounter;
            break;
        default:
            break;
    }

}


 
// Turns the gun's hit-LED on.
void hitLedTimer_turnLedOn(){
    mio_writePin(HIT_LED_TIMER_OUTPUT_PIN, LED_ON); // Write a '1' to JF-1.
}
 
// Turns the gun's hit-LED off.
void hitLedTimer_turnLedOff(){
    mio_writePin(HIT_LED_TIMER_OUTPUT_PIN, LED_OFF);
}
 
// Test function
void hitLedTimer_runTest(){
    hitLedTimer_init();
    while(testTimerCounter < LED_TEST_TIMER_MAX_VALUE){
        hitLedTimer_start();
        //utils_msDelay(2000);
        ++testTimerCounter;
    }
}
 
#endif /* HITLEDTIMER_H_ */
