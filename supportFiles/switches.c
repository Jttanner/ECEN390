/*
 * switches.c
 *
 *  Created on: May 9, 2017
 *      Author: jttanner
 */

#include "switches.h"
#include "xil_io.h"
#include "supportFiles/leds.h"
#include <stdio.h>


#define TRI_STATE_OFFSET 4
#define GPIO_OFFSET 0
#define TRI_STATE_TURN_OFF 0
#define PRINT_ERROR true
#define SWITCHES_INIT_STATUS_OK 1
#define SWITCHES_INIT_STATUS_FAIL 0
#define ALL_SWITCHES_ON 0xF
#define ALL_SWITCHES_TURN_OFF 0



void switches_writeGpioRegister(int32_t offset, int32_t value);
int32_t switches_readGpioRegister(int32_t offset);

// Initializes the SWITCHES driver software and hardware. Returns one of the STATUS values defined above.
int32_t switches_init(){
    //turn off the tri-state buffer
    //so that the  GPIO pins can function as an input
    //so that we can read the switch values
    switches_writeGpioRegister(TRI_STATE_OFFSET, TRI_STATE_TURN_OFF);
    //read the data of the switches, if it were necessary for this lab we would
    //use this to check initialize status
    switches_readGpioRegister(GPIO_OFFSET);

    //check that we get the correct values when we read the GPIO data register
    //if it is good, return a success status, otherwise return that it failed
    //for this project, it will always return true, os always return an ok status
    if(true){
        return SWITCHES_INIT_STATUS_OK;
    } else{
        return SWITCHES_INIT_STATUS_FAIL;
    }
}


int32_t switches_read(){
    //return the data of the Gpio register
    return switches_readGpioRegister(GPIO_OFFSET);
}


void switches_runTest(){
    //initialize the switces
    switches_init();
    //initialize the leds
    leds_init(PRINT_ERROR);

    while(switches_read() != ALL_SWITCHES_ON){
        //continuously update leds as switches are turned on or off until all are pressed
        leds_write(switches_read());
    }
    //reset the leds on the board after we have finished
    leds_write(ALL_SWITCHES_TURN_OFF);
}


int32_t switches_readGpioRegister(int32_t offset){
    //return the data at the register at the apporpriate register determined by the offset
    return Xil_In32(XPAR_SLIDE_SWITCHES_BASEADDR + offset);
}

void switches_writeGpioRegister(int32_t offset, int32_t value){
    //write the value to the register at the appropriate register determined by the offset
    Xil_Out32(XPAR_SLIDE_SWITCHES_BASEADDR + offset, value);
}
