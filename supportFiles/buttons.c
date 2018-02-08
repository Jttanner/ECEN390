/*
 * buttons.c
 *
 *  Created on: May 9, 2017
 *      Author: jttanner
 */

#include "buttons.h"
#include "xil_io.h"
#include "supportFiles/display.h"
#include <stdio.h>


int32_t buttons_readGpioRegister(int32_t offset);
void buttons_writeGpioRegister(int32_t offset, int32_t value);

#define TRI_STATE_OFFSET 4
#define GPIO_OFFSET 0
#define TRI_STATE_TURN_OFF 0
#define PRINT_ERROR true
#define BUTTONS_INIT_STATUS_OK 1
#define BUTTONS_INIT_STATUS_FAIL 0
#define ALL_BUTTONS_ON 0xF
#define BUTTON3_LEFT_SIDE_X_VALUE 0
#define BUTTON2_LEFT_SIDE_X_VALUE 80
#define BUTTON1_LEFT_SIDE_X_VALUE 160
#define BUTTON0_LEFT_SIDE_X_VALUE 240
#define DISPLAY_BUTTON_RECTANGLE_WIDTH 80
#define DISPLAY_BUTTON_RECTANGLE_HEIGHT 120
#define ALL_TEXT_Y_VALUE 60
#define BUTTON3_TEXT_X_VALUE 20
#define BUTTON2_TEXT_X_VALUE 100
#define BUTTON1_TEXT_X_VALUE 180
#define BUTTON0_TEXT_X_VALUE 260
#define MAX_WIDTH 320
#define MAX_HEIGHT 240
#define BUTTON3_TEXT "BTN3"
#define BUTTON2_TEXT "BTN2"
#define BUTTON1_TEXT "BTN1"
#define BUTTON0_TEXT "BTN0"
#define BUTTON_TEXT_SIZE 2
#define DEFAULT_STATE 0

int32_t buttons_init(){
    //turn off the tri-state buffer
    //so that the  GPIO pins can function as an input
    //so that we can read the switch values
    buttons_writeGpioRegister(TRI_STATE_OFFSET, TRI_STATE_TURN_OFF);
    //read the date from the
    buttons_readGpioRegister(GPIO_OFFSET);

    //check that we get the correct values when we read the GPIO data register
    //if it is good, return a success status, otherwise return that it failed
    if(true){
        return BUTTONS_INIT_STATUS_OK;
    } else{
        return BUTTONS_INIT_STATUS_FAIL;
    }
}


int32_t buttons_read(){
    return buttons_readGpioRegister(GPIO_OFFSET);
}

void buttons_runTest(){
    //initialize buttons
    buttons_init();
    //initialize the display
    display_init();
    //erase anything that might be already on the board
    display_fillRect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_BLACK);
    //set text size
    display_setTextSize(BUTTON_TEXT_SIZE);
    //make a variable to store the current state of what buttons are pressed
    uint32_t currentState = DEFAULT_STATE;
    uint32_t previousState = DEFAULT_STATE;
    while (buttons_read() != ALL_BUTTONS_ON){
        //continuously update the lcds until all all pressed
        currentState = buttons_read();
        //check if the state of pressed buttons has changed
        if(previousState != currentState){
            //erase so that we can rewrite if the state changed
            display_fillRect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_BLACK);

            if (currentState & BUTTONS_BTN3_MASK){
                //write button3 for all of its potential combinations
                display_fillRect(BUTTON3_LEFT_SIDE_X_VALUE, 0, DISPLAY_BUTTON_RECTANGLE_WIDTH, DISPLAY_BUTTON_RECTANGLE_HEIGHT, DISPLAY_BLUE);
                //set the location for the text of button 3 and write it
                display_setCursor(BUTTON3_TEXT_X_VALUE, ALL_TEXT_Y_VALUE);
                display_println(BUTTON3_TEXT);
            }
            if (currentState & BUTTONS_BTN2_MASK){
                //write button 2 for all of its potential combinations
                display_fillRect(BUTTON2_LEFT_SIDE_X_VALUE, 0, DISPLAY_BUTTON_RECTANGLE_WIDTH, DISPLAY_BUTTON_RECTANGLE_HEIGHT, DISPLAY_RED);
                //set tbe location for the text of button 2 and write it
                display_setCursor(BUTTON2_TEXT_X_VALUE, ALL_TEXT_Y_VALUE);
                display_println(BUTTON2_TEXT);
            }
            if (currentState & BUTTONS_BTN1_MASK){
                //write button 1 for all of its potential combinations
                display_fillRect(BUTTON1_LEFT_SIDE_X_VALUE, 0, DISPLAY_BUTTON_RECTANGLE_WIDTH, DISPLAY_BUTTON_RECTANGLE_HEIGHT, DISPLAY_GREEN);
                //set the location for the text of button 1 and write it
                display_setCursor(BUTTON1_TEXT_X_VALUE, ALL_TEXT_Y_VALUE);
                display_println(BUTTON1_TEXT);
            }
            if (currentState & BUTTONS_BTN0_MASK){
                //write button 0 for all of its potential combinations
                display_fillRect(BUTTON0_LEFT_SIDE_X_VALUE, 0, DISPLAY_BUTTON_RECTANGLE_WIDTH, DISPLAY_BUTTON_RECTANGLE_HEIGHT, DISPLAY_YELLOW);
                //set the location of the text of button 0 and write it
                display_setCursor(BUTTON0_TEXT_X_VALUE, ALL_TEXT_Y_VALUE);
                display_println(BUTTON0_TEXT);
            }
        }
        //set the previous state after we have done our checks to refer to later
        previousState = currentState;
    }
    //once all buttons have been pressed, clear the screen
    display_fillRect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_BLACK);
}


int32_t buttons_readGpioRegister(int32_t offset){
    //get the values at the address for the register which is decided by the offset
    return Xil_In32(XPAR_PUSH_BUTTONS_BASEADDR + offset);
}

void buttons_writeGpioRegister(int32_t offset, int32_t value){
    //write new values for the register at the address decided by the offset
    Xil_Out32(XPAR_PUSH_BUTTONS_BASEADDR + offset, value);
}
