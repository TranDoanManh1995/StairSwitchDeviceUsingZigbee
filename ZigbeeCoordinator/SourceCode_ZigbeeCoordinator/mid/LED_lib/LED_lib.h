/*
 * LED_lib.h
 *
 *  Created on: Jan 4, 2024
 *      Author: TRAN DOAN MANH
 */

#ifndef SOURCE_MID_LED_LIB_LED_LIB_H_
#define SOURCE_MID_LED_LIB_LED_LIB_H_

	/*------------------------------- All libraries -------------------------------*/


	/*------------------------------- Declare macros, enum & structure types, variables, function prototypes -------------------------------*/

	// Macros
#define NUMBER_LED					2			// The number of LEDs on Zigbee circuit
#define NUMBER_COLOR				3			// The number of colors in a LED
#define LED_PUSHPULL_OUTPUT			0			// Select the output mode of LED pin "Push-Pull"
	// Pin and port of LED_1 on Zigbee circuit
#define LED1_PORT				gpioPortA
#define LED1_BLUE_PIN			(0U)
#define LED1_GREEN_PIN			(3U)
#define LED1_RED_PIN			(4U)
	// Pin and port of LED_2 on Zigbee circuit
#define LED2_PORT				gpioPortD
#define LED2_BLUE_PIN			(2U)
#define LED2_GREEN_PIN			(1U)
#define LED2_RED_PIN			(0U)
	// The array contains information of LED_1, LED_2
#define LED1_RGB				{{LED1_PORT,LED1_BLUE_PIN}, {LED1_PORT,LED1_GREEN_PIN}, {LED1_PORT,LED1_RED_PIN}}
#define LED2_RGB				{{LED2_PORT,LED2_BLUE_PIN}, {LED2_PORT,LED2_GREEN_PIN}, {LED2_PORT,LED2_RED_PIN}}

	// Enum types
typedef enum { // Define all colors of LED
	LED_Off		= 0x000,
	LED_Blue	= BIT(0),				// (1U << 0)
	LED_Green	= BIT(1),				// (1U << 1)
	LED_Red		= BIT(2),				// (1U << 2)
	LED_Pink 	= BIT(0) | BIT(2),		// ledBlue + ledRed
	LED_Yellow	= BIT(2) | BIT(1)		// ledGreen + ledRed
}LED_ledColor_e;
typedef enum { // Define the index of LEDs on circuit
	LED_1,
	LED_2,
	LED_INVALID
}LED_indexLED_e;

	// Structure types
typedef struct { // Define the information of LED
	GPIO_Port_TypeDef GPIOportLed;
	unsigned int	  GPIOpinLed;
}LED_informationLed_s;
typedef struct { // Define the blink mode
	uint8_t  numberBlinks;
	uint16_t timeHighPulseCycle;
	uint16_t timeLowPulseCycle;
}LED_blinkMode_s;
typedef struct { // Define the dimmer mode
	boolean dimHigh;
	uint8_t dimLevel;
}LED_dimmerMode_s;
typedef struct { // Define the control mode of LED
	LED_ledColor_e color;
	union {
		LED_blinkMode_s  blinkMode;
		LED_dimmerMode_s dimMode;
	}LED_selectControlMode_u;
}LED_controlMode_s;

	// Variables
LED_controlMode_s controlModeLed[NUMBER_LED];

	// Event of blinking LED
EmberEventControl LED1_blinkLedEventControl, LED2_blinkLedEventControl;
void LED1_blinkLedEventHandler(void);
void LED2_blinkLedEventHandler(void);

	// Function prototypes
void LED_initializeLed(void);
void LED_turnOffLed(LED_indexLED_e indexLed);
void LED_turnOnLed(LED_indexLED_e indexLed, LED_ledColor_e colorLed);
void LED_blinkLed(LED_indexLED_e indexLed,
				  LED_ledColor_e colorLed,
				  uint8_t numberLedBlinks,
				  uint32_t timeHighPulse,
				  uint32_t timeLowPulse);

	/*------------------------------- END THIS FILE -------------------------------*/

#endif /* SOURCE_MID_LED_LIB_LED_LIB_H_ */
