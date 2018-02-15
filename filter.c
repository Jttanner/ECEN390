//
// Created by jontt on 2/8/2018.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "filter.h"
#include "queue.h"

#define IIR_FILTER_COUNT 10

#define IIR_COEFFICIENT_COUNT 11
#define FIR_COEFFICIENT_COUNT 81

#define X_QUEUE_SIZE 81
#define Y_QUEUE_SIZE 11
#define Z_QUEUE_SIZE 10
#define OUTPUT_QUEUE 2000

#define BANDWIDTH 50

#define IIR_FILTER_BASE_NAME "Filter Number "

static queue_t xQueue;
static queue_t yQueue;
static queue_t zQueues[IIR_FILTER_COUNT];
static queue_t outputQueues[IIR_FILTER_COUNT];

//Player frequencies
const static double playerFrequencies[IIR_FILTER_COUNT] = {1471, 1724, 2000, 2273, 2632, 2941, 3333, 3571, 3846, 4167};


//Filter Coefficients

//LOWPASS ANTI-ALIASING FIR
const static double firCoefficients[FIR_COEFFICIENT_COUNT] = {
        6.0546138291252597e-04,
        5.2507143315267811e-04,
        3.8449091272701525e-04,
        1.7398667197948182e-04,
        -1.1360489934931548e-04,
        -4.7488111478632532e-04,
        -8.8813878356223768e-04,
        -1.3082618178394971e-03,
        -1.6663618496969908e-03,
        -1.8755700366336781e-03,
        -1.8432363328817916e-03,
        -1.4884258721727399e-03,
        -7.6225514924622853e-04,
        3.3245249132384837e-04,
        1.7262548802593762e-03,
        3.2768418720744217e-03,
        4.7744814146589041e-03,
        5.9606317814670249e-03,
        6.5591485566565593e-03,
        6.3172870282586493e-03,
        5.0516421324586546e-03,
        2.6926388909554420e-03,
        -6.7950808883015244e-04,
        -4.8141100026888716e-03,
        -9.2899200683230643e-03,
        -1.3538595939086505e-02,
        -1.6891587875325020e-02,
        -1.8646984919441702e-02,
        -1.8149697899123560e-02,
        -1.4875876924586697e-02,
        -8.5110608557150517e-03,
        9.8848931927316319e-04,
        1.3360421141947857e-02,
        2.8033301291042201e-02,
        4.4158668590312596e-02,
        6.0676486642862550e-02,
        7.6408062643700314e-02,
        9.0166807112971648e-02,
        1.0087463525509034e-01,
        1.0767073207825099e-01,
        1.1000000000000000e-01,
        1.0767073207825099e-01,
        1.0087463525509034e-01,
        9.0166807112971648e-02,
        7.6408062643700314e-02,
        6.0676486642862550e-02,
        4.4158668590312596e-02,
        2.8033301291042201e-02,
        1.3360421141947857e-02,
        9.8848931927316319e-04,
        -8.5110608557150517e-03,
        -1.4875876924586697e-02,
        -1.8149697899123560e-02,
        -1.8646984919441702e-02,
        -1.6891587875325020e-02,
        -1.3538595939086505e-02,
        -9.2899200683230643e-03,
        -4.8141100026888716e-03,
        -6.7950808883015244e-04,
        2.6926388909554420e-03,
        5.0516421324586546e-03,
        6.3172870282586493e-03,
        6.5591485566565593e-03,
        5.9606317814670249e-03,
        4.7744814146589041e-03,
        3.2768418720744217e-03,
        1.7262548802593762e-03,
        3.3245249132384837e-04,
        -7.6225514924622853e-04,
        -1.4884258721727399e-03,
        -1.8432363328817916e-03,
        -1.8755700366336781e-03,
        -1.6663618496969908e-03,
        -1.3082618178394971e-03,
        -8.8813878356223768e-04,
        -4.7488111478632532e-04,
        -1.1360489934931548e-04,
        1.7398667197948182e-04,
        3.8449091272701525e-04,
        5.2507143315267811e-04,
        6.0546138291252597e-04};
//BANDPASS IIR,   A
const static double iirACoefficientConstants[FILTER_FREQUENCY_COUNT][IIR_COEFFICIENT_COUNT] = {
        {-5.9638000000000000e+00, 1.9125000000000000e+01, -4.0341000000000001e+01, 6.1536999999999999e+01, -7.0019999999999996e+01, 6.0298999999999999e+01, -3.8734000000000002e+01, 1.7994000000000000e+01, -5.4978999999999996e+00, 9.0332999999999997e-01},
        {-4.6378000000000004e+00, 1.3502000000000001e+01, -2.6155999999999999e+01, 3.8590000000000003e+01, -4.3039000000000001e+01, 3.7813000000000002e+01, -2.5114000000000001e+01, 1.2702999999999999e+01, -4.2755000000000001e+00, 9.0332999999999997e-01},
        {-3.0590999999999999e+00, 8.6417000000000002e+00, -1.4279000000000000e+01, 2.1302000000000000e+01, -2.2193999999999999e+01, 2.0873000000000001e+01, -1.3710000000000001e+01, 8.1303999999999998e+00, -2.8201999999999998e+00, 9.0332999999999997e-01},
        {-1.4072000000000000e+00, 5.6904000000000003e+00, -5.7374999999999998e+00, 1.1958000000000000e+01, -8.5434999999999999e+00, 1.1717000000000001e+01, -5.5087999999999999e+00, 5.3536999999999999e+00, -1.2972999999999999e+00, 9.0332999999999997e-01},
        {8.2011000000000001e-01, 5.1673999999999998e+00, 3.2580000000000000e+00, 1.0393000000000001e+01, 4.8102000000000000e+00, 1.0183999999999999e+01, 3.1282000000000001e+00, 4.8616000000000001e+00, 7.5605000000000000e-01, 9.0332999999999997e-01},
        {2.7081000000000000e+00, 7.8319000000000001e+00, 1.2202000000000000e+01, 1.8652000000000001e+01, 1.8757999999999999e+01, 1.8276000000000000e+01, 1.1715000000000000e+01, 7.3684000000000003e+00, 2.4965000000000002e+00, 9.0332999999999997e-01},
        {4.9480000000000004e+00, 1.4692000000000000e+01, 2.9082000000000001e+01, 4.3180000000000000e+01, 4.8441000000000003e+01, 4.2311000000000000e+01, 2.7922999999999998e+01, 1.3821999999999999e+01, 4.5614999999999997e+00, 9.0332999999999997e-01},
        {6.1702000000000004e+00, 2.0126999999999999e+01, 4.2973999999999997e+01, 6.5957999999999998e+01, 7.5230000000000004e+01, 6.4629999999999995e+01, 4.1262000000000000e+01, 1.8936000000000000e+01, 5.6882000000000001e+00, 9.0332999999999997e-01},
        {7.4093000000000000e+00, 2.6858000000000001e+01, 6.1579000000000001e+01, 9.8257999999999996e+01, 1.1359000000000000e+02, 9.6280000000000001e+01, 5.9125000000000000e+01, 2.5268999999999998e+01, 6.8304999999999998e+00, 9.0332999999999997e-01},
        {8.5742999999999991e+00, 3.4307000000000002e+01, 8.4034999999999997e+01, 1.3928999999999999e+02, 1.6305000000000001e+02, 1.3647999999999999e+02, 8.0686000000000007e+01, 3.2276000000000003e+01, 7.9044999999999996e+00, 9.0332999999999997e-01}
};
//BANDPASS IIR,   B
const static double iirBCoefficientConstants[FILTER_FREQUENCY_COUNT][IIR_COEFFICIENT_COUNT] = {
        {9.0927999999999999e-10, -0.0000000000000000e+00, -4.5463999999999999e-09, -0.0000000000000000e+00, 9.0927999999999999e-09, -0.0000000000000000e+00, -9.0927999999999999e-09, -0.0000000000000000e+00, 4.5463999999999999e-09, -0.0000000000000000e+00, -9.0927999999999999e-10},
        {9.0928999999999997e-10, 0.0000000000000000e+00, -4.5463999999999999e-09, 0.0000000000000000e+00, 9.0929000000000003e-09, 0.0000000000000000e+00, -9.0929000000000003e-09, 0.0000000000000000e+00, 4.5463999999999999e-09, 0.0000000000000000e+00, -9.0928999999999997e-10},
        {9.0928999999999997e-10, 0.0000000000000000e+00, -4.5463999999999999e-09, 0.0000000000000000e+00, 9.0929000000000003e-09, 0.0000000000000000e+00, -9.0929000000000003e-09, 0.0000000000000000e+00, 4.5463999999999999e-09, 0.0000000000000000e+00, -9.0928999999999997e-10},
        {9.0928999999999997e-10, 0.0000000000000000e+00, -4.5463999999999999e-09, 0.0000000000000000e+00, 9.0929000000000003e-09, 0.0000000000000000e+00, -9.0929000000000003e-09, 0.0000000000000000e+00, 4.5463999999999999e-09, 0.0000000000000000e+00, -9.0928999999999997e-10},
        {9.0928999999999997e-10, 0.0000000000000000e+00, -4.5463999999999999e-09, 0.0000000000000000e+00, 9.0929000000000003e-09, 0.0000000000000000e+00, -9.0929000000000003e-09, 0.0000000000000000e+00, 4.5463999999999999e-09, 0.0000000000000000e+00, -9.0928999999999997e-10},
        {9.0928999999999997e-10, -0.0000000000000000e+00, -4.5463999999999999e-09, -0.0000000000000000e+00, 9.0929000000000003e-09, -0.0000000000000000e+00, -9.0929000000000003e-09, -0.0000000000000000e+00, 4.5463999999999999e-09, -0.0000000000000000e+00, -9.0928999999999997e-10},
        {9.0927999999999999e-10, -0.0000000000000000e+00, -4.5463999999999999e-09, -0.0000000000000000e+00, 9.0927999999999999e-09, -0.0000000000000000e+00, -9.0927999999999999e-09, -0.0000000000000000e+00, 4.5463999999999999e-09, -0.0000000000000000e+00, -9.0927999999999999e-10},
        {9.0929999999999995e-10, 0.0000000000000000e+00, -4.5465000000000004e-09, 0.0000000000000000e+00, 9.0930000000000008e-09, 0.0000000000000000e+00, -9.0930000000000008e-09, 0.0000000000000000e+00, 4.5465000000000004e-09, 0.0000000000000000e+00, -9.0929999999999995e-10},
        {9.0927000000000000e-10, 0.0000000000000000e+00, -4.5463000000000003e-09, 0.0000000000000000e+00, 9.0926999999999994e-09, 0.0000000000000000e+00, -9.0926999999999994e-09, 0.0000000000000000e+00, 4.5463000000000003e-09, 0.0000000000000000e+00, -9.0927000000000000e-10},
        {9.0907000000000005e-10, 0.0000000000000000e+00, -4.5453999999999996e-09, 0.0000000000000000e+00, 9.0907000000000005e-09, 0.0000000000000000e+00, -9.0907000000000005e-09, 0.0000000000000000e+00, 4.5453999999999996e-09, 0.0000000000000000e+00, -9.0907000000000005e-10}
};

void initXQueue();
void initYQueue();
void initZQueues();
void initOutputQueues();
void filter_fillQueue(queue_t* q, double fillValue);


// Must call this prior to using any filter functions.
void filter_init() {
    initXQueue();
    initYQueue();
    initZQueues();
    initOutputQueues();
}

void initXQueue(){
    queue_init(&(xQueue), X_QUEUE_SIZE, "xQueue");
    for (uint16_t i = 0; i < Y_QUEUE_SIZE; ++i){
        queue_overwritePush(&(xQueue), 0.0);
    }
}

void initYQueue(){
    queue_init(&(yQueue), Y_QUEUE_SIZE, "yQueue");
    for (uint16_t i = 0; i < Y_QUEUE_SIZE; ++i){
        queue_overwritePush(&(yQueue), 0.0);
    }
}

void initZQueues(){
    for (uint16_t i = 0; i < IIR_FILTER_COUNT; ++i){
        char queueNumber[QUEUE_MAX_NAME_SIZE]; //buffer for queue number
        char queueName[QUEUE_MAX_NAME_SIZE]; //buffer for queue name
        sprintf(queueNumber, "%d", i);
        strcpy(queueName, "zQueue"); // initialize name
        strcat(queueName, queueNumber); //concatenate queue name and number to build full name
        queue_init(&(zQueues[i]), Z_QUEUE_SIZE, queueName); //init the queue
        filter_fillQueue(&zQueues[i], 0.0); // fill the queue with values initialized to 0
    }
}

void initOutputQueues(){
    for (uint16_t i = 0; i < IIR_FILTER_COUNT; ++i){
        char queueNumber[QUEUE_MAX_NAME_SIZE]; //buffer for queue number
        char queueName[QUEUE_MAX_NAME_SIZE]; //buffer for queue name
        sprintf(queueNumber, "%d", i);
        strcpy(queueName, "outputQueue"); // initialize name
        strcat(queueName, queueNumber); //concatenate queue name and number to build full name
        queue_init(&(outputQueues[i]), OUTPUT_QUEUE, queueName); //init the queue
        filter_fillQueue(&outputQueues[i], 0.0); // fill the queue with values initialized to 0
    }
}

// Use this to copy an input into the input queue of the FIR-filter (xQueue).
void filter_addNewInput(double x) {
    queue_overwritePush(&xQueue, x);
}

// Fills a queue with the given fillValue. For example,
// if the queue is of size 10, and the fillValue = 1.0,
// after executing this function, the queue will contain 10 values
// all of them 1.0.
void filter_fillQueue(queue_t* q, double fillValue) {
    for (uint16_t i = 0; i < queue_size(q); ++i){
        queue_overwritePush(q, fillValue);
    }
}

// Invokes the FIR-filter. Input is contents of xQueue.
// Output is returned and is also pushed on to yQueue.
double filter_firFilter() {
    //queue_readElementAt();
    //queue_overwritePush(yQueue, fillValue);
}

// Use this to invoke a single iir filter. Input comes from yQueue.
// Output is returned and is also pushed onto zQueue[filterNumber].
double filter_iirFilter(uint16_t filterNumber) {
    double output = 0;
    for(uint16_t i = 0; i < queue_size(&yQueue); ++i){
        double playerFrequency = playerFrequencies[filterNumber]; //current player frequency
        double element = queue_readElementAt(&yQueue, i); //gets element at this location of the queue
        //b*x/a
        output += iirACoefficientConstants[filterNumber][i]*element/iirBCoefficientConstants[filterNumber][i];
//        double lowerFrequencyBandLimit = playerFrequencies[i] - BANDWIDTH/2;
//        double upperFrequencyBandLimit = playerFrequencies[i] + BANDWIDTH/2;
        //filter
        //push onto z queue
    }

    return output;
}

// Use this to compute the power for values contained in an outputQueue.
// If force == true, then recompute power by using all values in the outputQueue.
// This option is necessary so that you can correctly compute power values the first time.
// After that, you can incrementally compute power values by:
// 1. Keeping track of the power computed in a previous run, call this prev-power.
// 2. Keeping track of the oldest outputQueue value used in a previous run, call this oldest-value.
// 3. Get the newest value from the power queue, call this newest-value.
// 4. Compute new power as: prev-power - (oldest-value * oldest-value) + (newest-value * newest-value).
// Note that this function will probably need an array to keep track of these values for each
// of the 10 output queues.
double filter_computePower(uint16_t filterNumber, bool forceComputeFromScratch, bool debugPrint) {

}

// Returns the last-computed output power value for the IIR filter [filterNumber].
double filter_getCurrentPowerValue(uint16_t filterNumber) {

}

// Get a copy of the current power values.
// This function copies the already computed values into a previously-declared array
// so that they can be accessed from outside the filter software by the detector.
// Remember that when you pass an array into a C function, changes to the array within
// that function are reflected in the returned array.
void filter_getCurrentPowerValues(double powerValues[]) {

}

// Using the previously-computed power values that are current stored in currentPowerValue[] array,
// Copy these values into the normalizedArray[] argument and then normalize them by dividing
// all of the values in normalizedArray by the maximum power value contained in currentPowerValue[].
void filter_getNormalizedPowerValues(double normalizedArray[], uint16_t* indexOfMaxValue) {

}

/*********************************************************************************************************
********************************** Verification-assisting functions. *************************************
********* Test functions access the internal data structures of the filter.c via these functions. ********
*********************** These functions are not used by the main filter functions. ***********************
**********************************************************************************************************/

// Returns the array of FIR coefficients.
const double* filter_getFirCoefficientArray() {

}

// Returns the number of FIR coefficients.
uint32_t filter_getFirCoefficientCount() {

}

// Returns the array of coefficients for a particular filter number.
const double* filter_getIirACoefficientArray(uint16_t filterNumber) {

}

// Returns the number of A coefficients.
uint32_t filter_getIirACoefficientCount() {

}

// Returns the array of coefficients for a particular filter number.
const double* filter_getIirBCoefficientArray(uint16_t filterNumber) {

}

// Returns the number of B coefficients.
uint32_t filter_getIirBCoefficientCount() {

}

// Returns the size of the yQueue.
uint32_t filter_getYQueueSize() {

}

// Returns the decimation value.
uint16_t filter_getDecimationValue() {

}

// Returns the address of xQueue.
queue_t* filter_getXQueue() {

}

// Returns the address of yQueue.
queue_t* filter_getYQueue() {

}

// Returns the address of zQueue for a specific filter number.
queue_t* filter_getZQueue(uint16_t filterNumber) {

}

// Returns the address of the IIR output-queue for a specific filter-number.
queue_t* filter_getIirOutputQueue(uint16_t filterNumber) {

}

//void filter_runTest();

//this is lame ahahahah
