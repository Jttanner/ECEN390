/*
 * filterTest.h
 *
 *  Created on: Mar 11, 2015
 *      Author: hutch
 */

#ifndef FILTERTEST_H_
#define FILTERTEST_H_

#include <stdbool.h>
#include <stdint.h>

// Invoke init before calling filterTest_runTest().
void filterTest_init();

// Performs a comprehensive test of the FIR, IIR filters and plots frequency response on the TFT.
bool filterTest_runTest();

double filter_getCurrentPowerValue(uint16_t filterNumber);

#endif /* FILTERTEST_H_ */
