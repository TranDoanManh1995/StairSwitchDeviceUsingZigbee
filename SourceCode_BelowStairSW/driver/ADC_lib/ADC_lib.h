/*
 * ADC_lib.h
 *
 *  Created on: Dec 6, 2023
 *      Author: TRAN DOAN MANH
 */

#ifndef SOURCE_DRIVER_ADC_LIB_ADC_LIB_H_
#define SOURCE_DRIVER_ADC_LIB_ADC_LIB_H_

	/*------------------------------- All libraries -------------------------------*/

#include "app/framework/include/af.h"

	/*------------------------------- Declare macros, function prototypes -------------------------------*/

	// Macros
#define CLK_SRC_ADC_FREQ        20000000			// CLK_SRC_ADC
#define CLK_ADC_FREQ            10000000			// CLK_ADC - 10 MHz max in normal mode

	// Function prototypes
void ADC_initializeADC0(void);
uint32_t ADC_readLightSensorValue(void);

	/*------------------------------- END THIS FILE -------------------------------*/

#endif /* SOURCE_DRIVER_ADC_LIB_ADC_LIB_H_ */
