#ifndef DETECTOR_H_
#define DETECTOR_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "queue.h"
#include "filter.h"
#include "isr.h"
#include "supportFiles/interrupts.h"
#include "lockoutTimer.h"
#include "hitLedTimer.h"
#define MAX 4095
#define HALF 2
#define DECIMATION_FACTOR 10

#define FUDGE_FACTOR 250
#define PLAYER_COUNT 10
#define FIRST_VALUE 0
#define MEDIAN_INDEX 4
#define MAX_INDEX 9

#define BREAK_VALUE_FOR_SORT 10


typedef uint16_t detector_hitCount_t;

detector_hitCount_t detector_hitArray[DECIMATION_FACTOR];
static bool detectorHitFlag = false;

double map(uint16_t value, uint16_t scalefactorMax) {
    uint16_t scalefactorMid = (scalefactorMax / HALF);
    return((value - scalefactorMid)/(double)scalefactorMid);
}

int which_player() {
    uint8_t bestIndex = 0;
    double highest_power = filter_getCurrentPowerValue(0);
    for (uint8_t i = 0; i < DECIMATION_FACTOR; ++i) {
        double curr_power = filter_getCurrentPowerValue(i);
        if (curr_power > highest_power){
            highest_power =  curr_power;
            bestIndex = i;
        }
    }
    return bestIndex;
}

// Always have to init things.
void detector_init(){
    filter_init();
    lockoutTimer_init();
    hitLedTimer_init();
    for (uint8_t i = 0; i < DECIMATION_FACTOR; ++i) {
        filter_computePower(i, true, false);
    }
}

//insertion sort helper function
void shiftAndInsertValues(double *sortingArray, double insertValue, uint16_t insertIndex){
    for (uint16_t i = PLAYER_COUNT; i >= insertIndex; --i){
        if (i == insertIndex){
            sortingArray[insertIndex] = insertValue;
            return;
        } else{
            sortingArray[i] = sortingArray[i - 1];
        }
    }
}

// Returns true if a hit was detected.
bool detector_hitDetected(){
    double powerValues[PLAYER_COUNT];
    for (uint16_t i = 0; i < PLAYER_COUNT; ++i){
        if(i == FIRST_VALUE){
            powerValues[i] = filter_getCurrentPowerValue(i);
        } else{
            double nextPowerValue = filter_getCurrentPowerValue(i);
            bool inserted = false;
            for (int16_t j = 0; j < i; ++j){
                if (nextPowerValue <= powerValues[j]){
                    shiftAndInsertValues(powerValues, nextPowerValue, j);
                    j = BREAK_VALUE_FOR_SORT;
                    inserted = true;
                }
                if (!inserted && j == i - 1){
                    powerValues[i] = nextPowerValue;
                }
            }

        }
    }
    double threshold = powerValues[MEDIAN_INDEX] * FUDGE_FACTOR;
    //printf("MEDIAN VALUE: %lf, THRESHOLD: %lf\n", powerValues[MEDIAN_INDEX], threshold);
    return powerValues[MAX_INDEX] > threshold ? true : false;
}


// Runs the entire detector: decimating fir-filter, iir-filters, power-computation, hit-detection.
// if interruptsEnabled = false, interrupts are not running. If interruptsEnabled = false
// you can pop values from the ADC queue without disabling interrupts.
// If interruptsEnabled = true, do the following:
// 1. disable interrupts.
// 2. pop the value from the ADC queue.
// 3. re-enable interrupts.
// Use this to determine whether you should disable and re-enable interrrupts when accessing the adc queue.
// if ignoreSelf == true, ignore hits that are detected on your frequency.
// Your frequency is simply the frequency indicated by the slide switches.
void detector(bool interruptsEnabled, bool ignoreSelf){
    uint32_t elementCount;
    uint16_t rawAdcValue;
    double scaledAdcValue;
    elementCount = isr_adcBufferElementCount();
    for (uint32_t i = 0; i < elementCount; i++) {
        if (interruptsEnabled) {    //if interrupts are enabled
            interrupts_disableArmInts();
        }
        rawAdcValue = isr_removeDataFromAdcBuffer();
        if (!interruptsEnabled) {                                //will this work?
            interrupts_enableArmInts();
        }
        scaledAdcValue = map(rawAdcValue, MAX);    //use this to map the function (x - 2047)/2047
        filter_addNewInput(scaledAdcValue);      //this adds from Adc to xqueue (the first one in our filter process)
        if (!((i-1)%DECIMATION_FACTOR)) {
            filter_firFilter();
            for (uint8_t i = 0; i < DECIMATION_FACTOR; ++i) {
                filter_iirFilter(i);
                filter_computePower(i, false, false);
            }
            if (!lockoutTimer_running()) {
                if(detector_hitDetected()); {      //run hit detect and returns true if hit
                    lockoutTimer_start();
                    hitLedTimer_start();
                    detector_hitArray[which_player()]++;          //returns the player that got hit
                    detectorHitFlag = true;
                }
            }
        }
    }
}


// Clear the detected hit once you have accounted for it.
void detector_clearHit(){
    detectorHitFlag = false;
}

// Get the current hit counts.
// Copy the current hit counts into the user-provided hitArray
// using a for-loop.
void detector_getHitCounts(detector_hitCount_t hitArray[]){
    for (uint8_t i = 0; i < DECIMATION_FACTOR; ++i) {
        hitArray[i] = detector_hitArray[i];
    }
}

// Test function
void detector_runTest(){
    detector_init();
    //printf("begin test\n");
    double hit_testPowerArray[PLAYER_COUNT] = {11, 20, 14, 32, 9400, 40, 20, 10, 5, 12};  //example test array
    double miss_testPowerArray[PLAYER_COUNT] = {11, 20, 14, 32, 12, 40, 20, 10, 5, 12};  //example test array
    for (uint16_t i = 0; i < PLAYER_COUNT; ++i){  //go through each player value
        test_addPowerValue(i, hit_testPowerArray[i]);
    }
    printf("HIT DETECTED?: %d  (expected hit)\n", detector_hitDetected());
    for (uint16_t i = 0; i < PLAYER_COUNT; ++i){  //go through each player value
        test_addPowerValue(i, miss_testPowerArray[i]);
    }
    printf("HIT DETECTED?: %d  (expected miss)\n", detector_hitDetected());
}

#endif /* DETECTOR_H_ */
