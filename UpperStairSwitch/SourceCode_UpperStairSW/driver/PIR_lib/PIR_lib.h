/*
 * PIR_lib.h
 *
 *  Created on: June 5, 2024
 *      Author: TRAN DOAN MANH
 */

#ifndef SOURCE_MID_PIR_LIB_PIR_LIB_H_
#define SOURCE_MID_PIR_LIB_PIR_LIB_H_

	/*------------------------------- All libraries -------------------------------*/

#include "app/framework/include/af.h"

	/*------------------------------- Declare macros, variables, enum types, event, function pointer & function prototypes -------------------------------*/

	// Macros
#define PIR_PORT						gpioPortC			// Pin and port of PIR sensor on Zigbee circuit
#define PIR_PIN							(4U)
#define PIR_PULLUP_FILTER_INPUT			1					// Select the input mode of PIR pin "Pull-Up Filter"

	// Enum types
typedef enum { // Define the states of finding motion
	PIR_StateDebounce,
	PIR_StateWait2s,
	PIR_StateWait30s
}PIR_stateFindingMotion_e;
typedef enum { // Define the states of motion
	PIR_Unmotion,
	PIR_Motion
}PIR_stateMotion_e;
typedef enum { // Define to control interrupt for PIR sensor
	PIR_DisableInterrupt,
	PIR_EnableInterrupt
}PIR_controlInterrupt_e;

	// Variables
PIR_stateFindingMotion_e PIR_stateFindingMotion;

	// Event of detecting motion
EmberEventControl PIR_detectMotionEventControl;
void PIR_detectMotionEventHandler(void);

	// Define a function pointer & create a function to call the processing callback
typedef void (*PIR_handleMotionSignal_t)(PIR_stateMotion_e );
void PIR_initializePIRsensor(PIR_handleMotionSignal_t );

	// Function prototypes
void PIR_controlInterruptPIRsensor(PIR_controlInterrupt_e controlInterrupt);
void PIR_handleInterruptPIRsensor(uint8_t pin);
boolean PIR_checkMotionSignalPIROUTpin(void);

	/*------------------------------- END THIS FILE -------------------------------*/

#endif /* SOURCE_MID_PIR_LIB_PIR_LIB_H_ */
