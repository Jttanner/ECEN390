#ifndef TRIGGER_H_
#define TRIGGER_H_

#include "supportFiles/buttons.h"
#include "transmitter.h"
#include "lockoutTimer.h"
#include <stdbool.h>
#include <stdio.h>
#include "supportFiles/switches.h"
 
// The trigger state machine debounces both the press and release of gun trigger.
// Ultimately, it will activate the transmitter when a debounced press is detected.
 
// Init trigger data-structures.
// Determines whether the trigger switch of the gun is connected (see discussion in lab web pages).
// Initializes the mio subsystem.
#define INIT 0
#define TICK_FREQ 100000
#define DELAY .05
#define HALF_SECOND_TEST DELAY/(1/TICK_FREQ)
#define DELAY 5000


volatile static bool trigger_flag = false;       //used to enable the SM
static uint16_t count = INIT; //used to create a delay

static enum trigger_st_t {
    init_st,
    wait_st,
    debounce_trigger_st,
    shoot_st,
    debounce_release_st,
} currentState;

void trigger_init() {
    count = INIT;
    currentState = init_st;
}

static bool triggerPressed() {
    return(buttons_read() & BUTTONS_BTN0_MASK);
}

// Enable the trigger state machine. The trigger state-machine is inactive until this function is called.
// This allows you to ignore the trigger when helpful (mostly useful for testing).
// I don't have an associated trigger_disable() function because I don't need to disable the trigger.
void trigger_enable() {
    trigger_flag = true;
}
 
// Standard tick function.
void trigger_tick() {
    switch(currentState) {
    case init_st:
        if (trigger_flag) {
            //printf("enter wait\n");
            currentState = wait_st;
        }
        else {
            currentState = init_st;
        }
        break;
    case wait_st:
        if (triggerPressed() && !lockoutTimer_running()) {
            //printf("enter debounce\n");
            currentState = debounce_trigger_st;
        }
        else {
            currentState = wait_st;
        }
        break;
    case debounce_trigger_st:
        //printf("count: %d", count);
        if (!triggerPressed()) {
            //printf("return_wait\n");
            currentState = wait_st;
            count = INIT;
            break;
        }
        if (count > DELAY) {
            //transmitter_run();         //this is so the gun will only fire once on the transition into shoot_st
            printf("D\n");
            uint32_t switch0 = switches_read() & SWITCHES_SW0_MASK;
            uint32_t switch1 = switches_read() & SWITCHES_SW1_MASK;
            uint32_t switch2 = switches_read() & SWITCHES_SW2_MASK;
            uint32_t switch3 = switches_read() & SWITCHES_SW3_MASK;
            uint32_t selectedPlayer = switch0 + switch1 + switch2 + switch3;
            transmitter_setFrequencyNumber(selectedPlayer);
            transmitter_run();
            currentState = shoot_st;
            count = INIT;
        }
        break;
    case shoot_st:
        if (!triggerPressed()) {
            //printf("to debounce\n");
            currentState = debounce_release_st;
        }
        else {
            currentState = shoot_st;
        }
        break;
    case debounce_release_st:
        if (triggerPressed()) {
            //printf("back to shoot\n");
            currentState = shoot_st;
            break;
        }
        if (count > DELAY) {
            printf("U\n\r");
            currentState = wait_st;
            count = INIT;
        }
        break;
    default:
        printf("Trigger state_transtion: hit default case\n\r");
        break;
    }
    switch(currentState) {
    case init_st:
        break;
    case wait_st:
        break;
    case debounce_trigger_st:
        count++;
        break;
    case shoot_st:
        break;
    case debounce_release_st:
        count++;
        break;
    default:
        printf("Trigger state_action: hit default case\n\r");
        break;
    }
}
 
// Trigger test function.
void trigger_runTest() {
    printf("trigger_enabled\n");
    trigger_init();
    trigger_enable();
}
 
#endif /* TRIGGER_H_ */
