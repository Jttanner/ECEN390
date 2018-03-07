/*
 * main.c
 *
 *  Created on: Jan 11, 2018
 *      Author: tshumwa2
 */

#include <stdio.h>
#include "queue.h"
#include "filterTest.h"
#include "filter.h"
#include "transmitter.h"
#include "trigger.h"
#include "lockoutTimer.h"
#include "supportFiles/utils.h"
#include "hitLedTimer.h"



#include "supportFiles/interrupts.h"
#include "supportFiles/buttons.h"

int main()
{
    interrupts_initAll(true);               // main interrupt init function.
    interrupts_enableTimerGlobalInts();     // enable global interrupts.
    interrupts_startArmPrivateTimer();      // start the main timer.
    interrupts_enableArmInts();             // now the ARM processor can see interrupts.
    printf("trigger\n");
    trigger_runTest();
    while (1){
        printf("continuous\n");
        transmitter_init();
        transmitter_setContinuousMode(true);
        transmitter_runTest();
        printf("non-continuous\n");
        transmitter_init();
        transmitter_setContinuousMode(false);
        transmitter_runTest();
        printf("hitLedTimer\n");
        if (buttons_read() & BUTTONS_BTN2_MASK){
            hitLedTimer_runTest();
        }
        if (buttons_read() & BUTTONS_BTN1_MASK){
            printf("lockoutTimer\n");
            lockoutTimer_runTest();
        }
    }
}
