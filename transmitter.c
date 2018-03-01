#ifndef TRANSMITTER_H_
#define TRANSMITTER_H_
 
#define TRANSMITTER_OUTPUT_PIN 13		// JF1 (pg. 25 of ZYBO reference manual).
#define TRANSMITTER_WAVEFORM_WIDTH 20000	// Based on a system tick-rate of 100 kHz.
#define TRANSMITTER_HIGH_VALUE 1
#define TRANSMITTER_LOW_VALUE 0
#include <stdint.h>
#include "filter.h"

#include "supportFiles/mio.h"

 
// The transmitter state machine generates a square wave output at the chosen frequency
// as set by transmitter_setFrequencyNumber(). The step counts for the frequencies
// are provided in filter.h

static enum transmitterState{
	INIT_STATE,
	OFF_STATE,
	GENERATE_SIGNAL_STATE
} currState;

static bool transmitter_enabled;
static uint16_t signalGenerationTickCount;
static uint16_t playerFrequencyNumber;
static uint16_t nextPlayerFrequencyNumber;
 
// Standard init function.
void transmitter_init(){
	mio_init(false);  // false disables any debug printing if there is a system failure during init.
	mio_setPinAsOutput(TRANSMITTER_OUTPUT_PIN);  // Configure the signal direction of the pin to be an output.
	transmitter_enabled = false;
	currState = INIT_STATE;
	signalGenerationTickCount = 0;
	playerFrequencyNumber = 0;
	nextPlayerFrequencyNumber = 0;
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
	nextPlayerFrequencyNumber = frequencyNumber;
}
 
// Standard tick function.
void transmitter_tick(){
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
			if (signalGenerationTickCount > filter_frequencyTickTable[playerFrequencyNumber]){
				currState = OFF_STATE;
				transmitter_enabled = false;
				transmitter_stopGeneratingSignalToBoard();
			} else{
				signalGenerationTickCount = 0;
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
			transmitter_generateSignalToBoard();
			++signalGenerationTickCount;
			break;
	}
}
 
// Tests the transmitter.
void transmitter_runTest(){
	
}
 
// Runs the transmitter continuously.
// if continuousModeFlag == true, transmitter runs continuously, otherwise, transmits one waveform and stops.
// To set continuous mode, you must invoke this function prior to calling transmitter_run().
// If the transmitter is in currently in continuous mode, it will stop running if this function is
// invoked with continuousModeFlag == false. It can stop immediately or 
// wait until the last 200 ms waveform is complete.
// NOTE: while running continuously, the transmitter will change frequencies at the end of each 200 ms waveform.
void transmitter_setContinuousMode(bool continuousModeFlag){
	
}
 
// This is provided for testing as explained in the transmitter section of the web-page. When enabled,
// debug prints are enabled to help to demonstrate the behavior of the transmitter.
void transmitter_enableTestMode(){
	
}
 
// This is provided for testing as explained in the transmitter section of the web-page. When disabled,
// debug prints that were previously enabled no longer appear.
void transmitter_disableTestMode(){
	
}
 
#endif /* TRANSMITTER_H_ */