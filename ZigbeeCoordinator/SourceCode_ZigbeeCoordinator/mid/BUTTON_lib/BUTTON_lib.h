/*
 * BUTTON_lib.h
 *
 *  Created on: Jan 2, 2024
 *      Author: TRAN DOAN MANH
 */

#ifndef SOURCE_MID_BUTTON_LIB_BUTTON_LIB_H_
#define SOURCE_MID_BUTTON_LIB_BUTTON_LIB_H_

	/*------------------------------- All libraries -------------------------------*/

#include "stdbool.h"

	/*------------------------------- Declare macros, enum & structure types, variables & function prototypes -------------------------------*/

	// Macros
#define NUMBER_BUTTON						2			// The number of BUTTONs on Zigbee circuit
#define BUTTON_PULLUP_FILTER_INPUT			1			// Select the input mode of BUTTON pin "Pull-Up Filter"
#define BUTTON_DEBOUNCE						5			// Eliminate button bounce
#define BUTTON_CHECK_RELEASE_MS				500
#define BUTTON_PRESS						0			// The state of BUTTON pin when pressing that button
	// Pin and port of BUTTON 1, 2 on Zigbee circuit
#define BUTTON_1_PORT					gpioPortD
#define BUTTON_1_PIN					(4U)
#define BUTTON_2_PORT					gpioPortD
#define BUTTON_2_PIN					(3U)
	// The array contains information of BUTTON 1, 2
#define BUTTON_INFORMATION				{{BUTTON_1_PORT, BUTTON_1_PIN}, {BUTTON_2_PORT, BUTTON_2_PIN}}

	// Enum types
typedef enum { // Define the index of buttons on circuit
	BUTTON_1,
	BUTTON_2,
	BUTTON_INVALID
}BUTTON_indexButton_e;
typedef enum { // Define the number of button presses
	BUTTON_Press_1	= 1,
	BUTTON_Press_2,
	BUTTON_Press_3,
	BUTTON_Press_4,
	BUTTON_Press_5,
	BUTTON_Press_6,
	BUTTON_Press_7
}BUTTON_numberPresses_e;

	// Structure types
typedef struct { // Define the information of button
	GPIO_Port_TypeDef GPIOportButton;
	unsigned int	  GPIOpinButton;
	bool			  stateButton;
	uint8_t			  numberButtonPresses;
}BUTTON_informationButton_s;

	// Variables
BUTTON_indexButton_e indexButton;			// Identify BUTTON_1 or BUTTON_2

	// Event of releasing button
EmberEventControl BUTTON_releaseButtonEventControl;
void BUTTON_releaseButtonEventHandler(void);

	// Define a function pointer & create a function to call the processing callback
typedef void (*BUTTON_handleNumberButtonPresses_t)(BUTTON_indexButton_e , BUTTON_numberPresses_e );
void BUTTON_initializeButton(BUTTON_handleNumberButtonPresses_t );

	// Function prototypes
void BUTTON_handleInterruptButton(uint8_t pinInterrupt);
BUTTON_indexButton_e BUTTON_getButtonIndex(uint8_t pinInterrupt);

	/*------------------------------- END THIS FILE -------------------------------*/

#endif /* SOURCE_MID_BUTTON_LIB_BUTTON_LIB_H_ */
