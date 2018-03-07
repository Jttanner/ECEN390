#ifndef TRANSMITTER_H_
#define TRANSMITTER_H_
 
#define TRANSMITTER_OUTPUT_PIN 13		// JF1 (pg. 25 of ZYBO reference manual).
#define TRANSMITTER_WAVEFORM_WIDTH 20000	// Based on a system tick-rate of 100 kHz.
#define TRANSMITTER_HIGH_VALUE 1
#define TRANSMITTER_LOW_VALUE 0
#define TEST_TIMER_COUNT_LIMIT 100
#define TRANSMIT_TWO_HUNDRED_MS_TIME_LIMIT_TICK_COUNT 20000
#define PLAYER_COUNT 10
#include <stdint.h>
#include <stdio.h>
#include "filter.h"
#include "supportFiles/switches.h"
#include "supportFiles/utils.h"

#include "supportFiles/mio.h"

void transmitter_enableTestMode();
void debug_printStateMachine();
void transmitter_disableTestMode();

// The transmitter state machine generates a square wave output at the chosen frequency
// as set by transmitter_setFrequencyNumber(). The step counts for the frequencies
// are provided in filter.h

static enum transmitterState{
	INIT_STATE,
	OFF_STATE,
	GENERATE_SIGNAL_STATE,
	STOP_GENERATE_SIGNAL_STATE
} currState;

static bool transmitter_enabled;
static uint16_t signalGenerationTickCount;
static uint16_t stopSignalGenerationTickCount;
static uint16_t playerFrequencyNumber;
static uint16_t nextPlayerFrequencyNumber;
static uint16_t transmitTimer;
static bool continuous;
static bool debug;

void transmitter_setContinuousMode(bool continuousModeFlag);
 
// Standard init function.
void transmitter_init(){
	mio_init(false);  // false disables any debug printing if there is a system failure during init.
	mio_setPinAsOutput(TRANSMITTER_OUTPUT_PIN);  // Configure the signal direction of the pin to be an output.
	transmitter_enabled = false;
	continuous = false;
	currState = INIT_STATE;
	signalGenerationTickCount = 0;
	stopSignalGenerationTickCount = 0;
	playerFrequencyNumber = 0;
	nextPlayerFrequencyNumber = 0;
	transmitTimer = 0;
	debug = false;
}


void transmitter_generateSignalToBoard() {
	mio_writePin(TRANSMITTER_OUTPUT_PIN, TRANSMITTER_HIGH_VALUE); // Write a '1' to JF-1.
}
 
 
void transmitter_stopGeneratingSignalToBoard(){
	mio_writePin(TRANSMITTER_OUTPUT_PIN, TRANSMITTER_LOW_VALUE); // Write a '0' to JF-1.
}
 
// Starts the transmitter.
void transmitter_run(){
	transmitter_enabled = true;
}
 
// Returns true if the transmitter is still running.
bool transmitter_running(){
	return transmitter_enabled;
}

/*
	You will set the waveform frequency by reading the ZYBO slide-switches in main() and then calling transmitter_setFrequencyNumber().
	Do not read the slide-switches in the transmitter state-machine.
*/
 
// Sets the frequency number. If this function is called while the
// transmitter is running, the frequency will not be updated until the
// transmitter stops and transmitter_run() is called again.
void transmitter_setFrequencyNumber(uint16_t frequencyNumber){
    //if (!continuous && transmitter_enabled){
    if (0){

    } else{
        nextPlayerFrequencyNumber = frequencyNumber;
        if (frequencyNumber > 9){
            nextPlayerFrequencyNumber = 9;
        }
        if (frequencyNumber < 0){
            nextPlayerFrequencyNumber = 0;
        }
    }

}
 
// Standard tick function.
void transmitter_tick(){
    if (debug){
        debug_printStateMachine();
    }

	//state transitions
	switch(currState){
		case INIT_STATE:
			currState = OFF_STATE;
			break;
		case OFF_STATE:
			if (transmitter_enabled){
				currState = GENERATE_SIGNAL_STATE;
			} else{
				currState = currState;
			}
			break;	
		case GENERATE_SIGNAL_STATE:
		    if (transmitTimer > TRANSMIT_TWO_HUNDRED_MS_TIME_LIMIT_TICK_COUNT){
		        if (!continuous){
		            currState = OFF_STATE;
		        } else{
		            currState = currState;
		        }

		        transmitter_enabled = false;
		        transmitTimer = 0;
		        break;
		    }
			if (signalGenerationTickCount > filter_frequencyTickTable[playerFrequencyNumber]/2 - 1){
				currState = STOP_GENERATE_SIGNAL_STATE;
				transmitter_stopGeneratingSignalToBoard();
				signalGenerationTickCount = 0;
			} else{
				currState = currState;
			}
			break;
		case STOP_GENERATE_SIGNAL_STATE:
		    if (stopSignalGenerationTickCount > filter_frequencyTickTable[playerFrequencyNumber]/2 ){
		    //if(stopSignalGenerationTickCount > 10){
		        currState = GENERATE_SIGNAL_STATE;
		        stopSignalGenerationTickCount = 0;
		    } else{
		        currState = currState;
		    }
		    break;
	}
	
	//state actions
	switch(currState){
		case INIT_STATE:
			break;
		case OFF_STATE:
			playerFrequencyNumber = nextPlayerFrequencyNumber;
			break;	
		case GENERATE_SIGNAL_STATE:
		    if (continuous){
		        playerFrequencyNumber = nextPlayerFrequencyNumber;
		    }
		    transmitter_generateSignalToBoard();
			++signalGenerationTickCount;
			++transmitTimer;
			break;
		case STOP_GENERATE_SIGNAL_STATE:
		    transmitter_stopGeneratingSignalToBoard();
		    ++stopSignalGenerationTickCount;
		    ++transmitTimer;
		    break;

	}
}
 
// Tests the transmitter.
void transmitter_runTest(){;
    transmitter_disableTestMode();
    uint32_t testTimer = 0;
    while (testTimer < TEST_TIMER_COUNT_LIMIT){
        uint32_t switch0 = switches_read() & SWITCHES_SW0_MASK;
        uint32_t switch1 = switches_read() & SWITCHES_SW1_MASK;
        uint32_t switch2 = switches_read() & SWITCHES_SW2_MASK;
        uint32_t switch3 = switches_read() & SWITCHES_SW3_MASK;
        uint32_t selectedPlayer = switch0 + switch1 + switch2 + switch3;
        //printf("selectedPlayer: %d\n", selectedPlayer);
        transmitter_setFrequencyNumber(selectedPlayer);
        transmitter_run();
        if (continuous){
            utils_msDelay(100);
        } else{
            utils_msDelay(1000);
        }
        //utils_msDelay(100);
        printf("%d\n",++testTimer);
    }
}

void debug_printStateMachine(){
    switch(currState){
        case INIT_STATE:
            printf("init\n");
            break;
        case OFF_STATE:
            printf("off\n");
            break;
        case GENERATE_SIGNAL_STATE:
            printf("generate\n");
            break;
        default:
            printf("FAILED AT A DEFAULT\n");
            break;
    }
}

 
// Runs the transmitter continuously.
// if continuousModeFlag == true, transmitter runs continuously, otherwise, transmits one waveform and stops.
// To set continuous mode, you must invoke this function prior to calling transmitter_run().
// If the transmitter is in currently in continuous mode, it will stop running if this function is
// invoked with continuousModeFlag == false. It can stop immediately or 
// wait until the last 200 ms waveform is complete.
// NOTE: while running continuously, the transmitter will change frequencies at the end of each 200 ms waveform.
void transmitter_setContinuousMode(bool continuousModeFlag){
	continuous = continuousModeFlag;
}
 
// This is provided for testing as explained in the transmitter section of the web-page. When enabled,
// debug prints are enabled to help to demonstrate the behavior of the transmitter.
void transmitter_enableTestMode(){
	debug = true;
}
 
// This is provided for testing as explained in the transmitter section of the web-page. When disabled,
// debug prints that were previously enabled no longer appear.
void transmitter_disableTestMode(){
	debug = false;
}
 
#endif /* TRANSMITTER_H_ */
