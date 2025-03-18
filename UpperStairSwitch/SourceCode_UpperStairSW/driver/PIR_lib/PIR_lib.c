/*
 * PIR_lib.c
 *
 *  Created on: June 5, 2024
 *  Author: TRAN DOAN MANH
 *  Descriptor: To process the motion signal of PIR sensor
 */

	/*------------------------------- All libraries -------------------------------*/

#include "app/framework/include/af.h"
#include "gpiointerrupt.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "source/app/main/main.h"
#include "source/driver/PIR_lib/PIR_lib.h"

	/*------------------------------- Initialize pointer function -------------------------------*/

	// Function pointer
PIR_handleMotionSignal_t handleMotionSignal = NULL;

	/*------------------------------- All functions -------------------------------*/

/*
 * @func PIR_initializePIRsensor
 * @brief To initialize PIR sensor
 * @param handleMotionSignalCallback - The callback function will be called to process the motion signal
 * @retval None
 */
void PIR_initializePIRsensor(PIR_handleMotionSignal_t handleMotionSignalCallback)
{
		// Register the callback function to process motion
	if(handleMotionSignalCallback != NULL){
		handleMotionSignal = handleMotionSignalCallback;
	}

		// Turn on the clock pulse on the GPIO port that is used for PIR sensor
	GPIOINT_Init();
	CMU_ClockEnable(cmuClock_GPIO, true);

		// Configure GPIO mode & register the interrupt processing function
	GPIO_PinModeSet(PIR_PORT, PIR_PIN, gpioModeInputPullFilter, PIR_PULLUP_FILTER_INPUT);
	GPIOINT_CallbackRegister(PIR_PIN, PIR_handleInterruptPIRsensor);
}

/*
 * @func PIR_controlInterruptPIRsensor
 * @brief To control interrupt for PIR sensor
 * @param controlInterrupt - Enable/Disable interrupt for PIR sensor
 * @retval None
 */
void PIR_controlInterruptPIRsensor(PIR_controlInterrupt_e controlInterrupt)
{
	if(controlInterrupt == PIR_EnableInterrupt){
		GPIO_ExtIntConfig(PIR_PORT, PIR_PIN, PIR_PIN, true, false, true);
	} else if(controlInterrupt == PIR_DisableInterrupt){
		GPIO_ExtIntConfig(PIR_PORT, PIR_PIN, PIR_PIN, false, false, false);
	}
}

/*
 * @func PIR_handleInterruptPIRsensor
 * @brief To process after happening interrupt
 * @param GPIOpin - GPIO pin connects to PIR_OUT pin
 * @retval None
 */
void PIR_handleInterruptPIRsensor(uint8_t GPIOpin)
{
		// Check GPIO pin that connects to PIR_OUT pin
	if(GPIOpin != PIR_PIN) return;

		// Check motion to process
	if(PIR_checkMotionSignalPIROUTpin()){
		PIR_stateFindingMotion = PIR_StateDebounce;
		PIR_controlInterruptPIRsensor(PIR_DisableInterrupt);
		emberEventControlSetInactive(PIR_detectMotionEventControl);
		emberEventControlSetDelayMS(PIR_detectMotionEventControl, 200);			// Delay 200ms to avoid bounce
	}
}

/*
 * @func PIR_checkMotionSignalPIROUTpin
 * @brief To check the motion signal of PIROUT pin
 * @param None
 * @retval Motion/Unmotion
*/
boolean PIR_checkMotionSignalPIROUTpin(void)
{
		// Get signal of PIROUT pin
	static boolean signalPIROUTpin;

		// Get the signal of PIROUT pin
	signalPIROUTpin = GPIO_PinInGet(PIR_PORT, PIR_PIN);

	return signalPIROUTpin;
}

/*
 * @func PIR_detectMotionEventHandler
 * @brief To process motion event
 * @param None
 * @retval None
 */
void PIR_detectMotionEventHandler(void)
{
		// Inactivate detecting motion event
	emberEventControlSetInactive(PIR_detectMotionEventControl);

		// Process all status of finding motion
	switch(PIR_stateFindingMotion){
		case PIR_StateDebounce:
			if(PIR_checkMotionSignalPIROUTpin()){
				emberAfCorePrintln("PIR_DETECT_MOTION !");
				PIR_stateFindingMotion = PIR_StateWait2s;
				if(handleMotionSignal != NULL){
					handleMotionSignal(PIR_Motion);
				}
				emberEventControlSetDelayMS(PIR_detectMotionEventControl, 2000);
			} else PIR_controlInterruptPIRsensor(PIR_EnableInterrupt);
			break;

		case PIR_StateWait2s:
			emberAfCorePrintln("PIR_STATE_WAIT_2S !");
			PIR_stateFindingMotion = PIR_StateWait30s;
			if(MAIN_controlMode == MAIN_ModeAuto){
				PIR_controlInterruptPIRsensor(PIR_EnableInterrupt);
			}
			emberEventControlSetDelayMS(PIR_detectMotionEventControl, 30000);
			break;

		case PIR_StateWait30s:
			if(handleMotionSignal != NULL){
				emberAfCorePrintln("PIR_DETECT_UNMOTION !");
				handleMotionSignal(PIR_Unmotion);
			}
			break;

		default:
			break;
	}
}

	/*------------------------------- END THIS FILE -------------------------------*/
