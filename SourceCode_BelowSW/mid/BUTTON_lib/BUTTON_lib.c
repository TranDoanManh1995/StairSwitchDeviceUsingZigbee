/*
 * BUTTON_lib.c
 *
 *  Created on: Jan 2, 2024
 *  Author: TRAN DOAN MANH
 *  Descriptor: To handle interrupt on button & call the callback function to process
 */

	/*------------------------------- All libraries -------------------------------*/

#include "app/framework/include/af.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "gpiointerrupt.h"
#include "source/mid/BUTTON_lib/BUTTON_lib.h"

	/*------------------------------- Initialize function pointer, array -------------------------------*/

	// Function pointer
BUTTON_handleNumberButtonPresses_t handleNumberButtonPresses = NULL;

	// Array
BUTTON_informationButton_s informationButton[NUMBER_BUTTON] = BUTTON_INFORMATION;

	/*------------------------------- All functions -------------------------------*/

/*
 * @func BUTTON_initializeButton
 * @brief To initialize BUTTON at inputPullupFilter and interrupt mode
 * @param handleNumberButtonPressesCallback - The callback function will be called to process the number of button presses
 * @retval None
 */
void BUTTON_initializeButton(BUTTON_handleNumberButtonPresses_t handleNumberButtonPressesCallback)
{
		// Register the callback function to handle the number of button presses
	if(handleNumberButtonPressesCallback != NULL){
		handleNumberButtonPresses = handleNumberButtonPressesCallback;
	}

		// Turn on the clock pulse on the GPIO port that is used for button
	CMU_ClockEnable(cmuClock_GPIO, true);
	GPIOINT_Init();

		// Only configure GPIO mode & interrupt for BUTTON_2 because BUTTON_1 & PIR use a common interrupt
	GPIO_PinModeSet(informationButton[1].GPIOportButton,
					informationButton[1].GPIOpinButton,
					gpioModeInputPullFilter,
					BUTTON_PULLUP_FILTER_INPUT);
	GPIOINT_CallbackRegister(informationButton[1].GPIOpinButton, BUTTON_handleInterruptButton);
	GPIO_ExtIntConfig(informationButton[1].GPIOportButton,
					  informationButton[1].GPIOpinButton,
					  informationButton[1].GPIOpinButton,
					  true,
					  true,
					  true);
}

/*
 * @func BUTTON_handleInterruptButton
 * @brief To process interrupt on button
 * @param pinInterrupt - pin of GPIO happening interrupt
 * @retval None
 */
void BUTTON_handleInterruptButton(uint8_t pinInterrupt)
{
		// Variables
	uint8_t newButtonState;
	uint8_t oldButtonState;
	uint8_t numberDebounces;

		//	Check whether BUTTON is both BUTTON1 and BUTTON2
	indexButton = BUTTON_getButtonIndex(pinInterrupt);
	if(indexButton == BUTTON_INVALID) return;

		// Wait for stabilizing button and get state of pin
	newButtonState = GPIO_PinInGet(informationButton[indexButton].GPIOportButton, informationButton[indexButton].GPIOpinButton);
	for (numberDebounces = 0;
		 numberDebounces < BUTTON_DEBOUNCE;
		 numberDebounces = (newButtonState == oldButtonState) ? (numberDebounces + 1) : 0)
	{
		oldButtonState = newButtonState;
		newButtonState = GPIO_PinInGet(informationButton[indexButton].GPIOportButton, informationButton[indexButton].GPIOpinButton);
	}
	informationButton[indexButton].stateButton = newButtonState;

		// Process the states of button
	if(informationButton[indexButton].stateButton == BUTTON_PRESS){
		informationButton[indexButton].numberButtonPresses++;
		emberAfCorePrintln("The number of button presses: %d", informationButton[indexButton].numberButtonPresses);
	} else{
		emberEventControlSetDelayMS(BUTTON_releaseButtonEventControl, BUTTON_CHECK_RELEASE_MS);
	}
}

/*
 * @func BUTTON_releaseButtonEventHandler
 * @brief To process the button releasing event
 * @param None
 * @retval None
 */
void BUTTON_releaseButtonEventHandler(void)
{
		// Inactivate BUTTON releasing event
	emberEventControlSetInactive(BUTTON_releaseButtonEventControl);

		// Call the callback function to process the number of button presses
	handleNumberButtonPresses(indexButton, informationButton[indexButton].numberButtonPresses);
	informationButton[indexButton].numberButtonPresses = 0;
}

/*
 * @func BUTTON_getButtonIndex
 * @brief To get index of button
 * @param pinInterrupt - pin of GPIO happening interrupt
 * @retval The index of button
 */
BUTTON_indexButton_e BUTTON_getButtonIndex(uint8_t pinInterrupt)
{
	for(uint8_t i = 0; i < NUMBER_BUTTON; i++){
		if(informationButton[i].GPIOpinButton == pinInterrupt)
			return i;
	}

	return BUTTON_INVALID;
}

	/*------------------------------- END THIS FILE -------------------------------*/


