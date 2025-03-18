/*
 * LED_lib.c
 *
 *  Created on: Jan 4, 2024
 *  Author: TRAN DOAN MANH
 *  Description: To control LEDs on circuit
 */

	/*------------------------------- All libraries -------------------------------*/

#include "app/framework/include/af.h"
#include "source/mid/LED_lib/LED_lib.h"

	/*------------------------------- Initialize array -------------------------------*/

	// Array
LED_informationLed_s informationLed[NUMBER_LED][NUMBER_COLOR] = {LED1_RGB, LED2_RGB};

	/*------------------------------- All functions -------------------------------*/

/*
 * @func LED_initializeLed
 * @brief To initialize LEDs at output_pushpull mode
 * @param None
 * @retval None
 */
void LED_initializeLed(void)
{
		// Enable clock pulse for GPIO
	CMU_ClockEnable(cmuClock_GPIO, true);

		// Initialize all LEDs at output mode "Push-Pull"
	for(uint8_t i = 0; i < NUMBER_LED; i++){
		for(uint8_t j = 0; j < NUMBER_COLOR; j++){
			GPIO_PinModeSet(informationLed[i][j].GPIOportLed,
							informationLed[i][j].GPIOpinLed,
							gpioModePushPull,
							LED_PUSHPULL_OUTPUT);
		}
	}

		// Turn off all LEDs after initializing
	LED_turnOffLed(LED_1);
	LED_turnOffLed(LED_2);
}

/*
 * @func LED_turnOffLed
 * @brief To turn off LED
 * @param indexLed - The index of LED on circuit
 * @retval None
 */
void LED_turnOffLed(LED_indexLED_e indexLed)
{
	for(uint8_t i = 0; i < NUMBER_COLOR; i++){
		GPIO_PinOutSet(informationLed[indexLed][i].GPIOportLed, informationLed[indexLed][i].GPIOpinLed);
	}
}

/*
 * @func LED_turnOnLed
 * @brief To turn on LED
 * @param
 * 		  indexLed - The index of LED
 * 		  colorLed - The color of LED
 * @retval None
 */
void LED_turnOnLed(LED_indexLED_e indexLed, LED_ledColor_e colorLed)
{
	for(uint8_t i = 0; i < NUMBER_COLOR; i++){
		if(((colorLed >> i) & 0x01) == 1){
			GPIO_PinOutClear(informationLed[indexLed][i].GPIOportLed, informationLed[indexLed][i].GPIOpinLed);
		} else{
			GPIO_PinOutSet(informationLed[indexLed][i].GPIOportLed, informationLed[indexLed][i].GPIOpinLed);
		}
	}
}

/*
 * @func LED_blinkLed
 * @brief To configure LED at blink mode
 * @param
 * 		  indexLed - The index of LED
 * 		  colorLed - The color of LED
 *		  numberLedBlinks - The number of LED blinks
 *		  timeHighPulse - The time at high pulse of a cycle (milisecond)
 *		  timeLowPulse - The time at low pulse of a cycle (milisecond)
 * @retval None
 */
void LED_blinkLed(LED_indexLED_e indexLed,
				  LED_ledColor_e colorLed,
				  uint8_t numberLedBlinks,
				  uint32_t timeHighPulse,
				  uint32_t timeLowPulse)
{
		// Configure at blink mode
	controlModeLed[indexLed].color = colorLed;
	controlModeLed[indexLed].LED_selectControlMode_u.blinkMode.timeHighPulseCycle = timeHighPulse;
	controlModeLed[indexLed].LED_selectControlMode_u.blinkMode.timeLowPulseCycle = timeLowPulse;
	controlModeLed[indexLed].LED_selectControlMode_u.blinkMode.numberBlinks = numberLedBlinks * 2;

		// Trigger event to control LED
	if(indexLed == LED_1){
		emberEventControlSetActive(LED1_blinkLedEventControl);
	} else if(indexLed == LED_2){
		emberEventControlSetActive(LED2_blinkLedEventControl);
	}
}

/*
 * @func LED1_blinkLedEventHandler
 * @brief To blink LED_1
 * @param None
 * @retval None
 */
void LED1_blinkLedEventHandler(void)
{
		// Inactivate LED_1 blink event
	emberEventControlSetInactive(LED1_blinkLedEventControl);

		// Control LED_1 blink
	if(controlModeLed[LED_1].LED_selectControlMode_u.blinkMode.numberBlinks != 0){
		if((controlModeLed[LED_1].LED_selectControlMode_u.blinkMode.numberBlinks % 2) == 0){
			for(uint8_t i = 0; i < NUMBER_COLOR; i++){
				if(((controlModeLed[LED_1].color >> i) & 0x01) == 1){
					GPIO_PinOutClear(informationLed[LED_1][i].GPIOportLed, informationLed[LED_1][i].GPIOpinLed);
				} else{
					GPIO_PinOutSet(informationLed[LED_1][i].GPIOportLed, informationLed[LED_1][i].GPIOpinLed);
				}
			}
			emberEventControlSetDelayMS(LED1_blinkLedEventControl, controlModeLed[LED_1].LED_selectControlMode_u.blinkMode.timeHighPulseCycle);
		} else{
			for(uint8_t i = 0; i < NUMBER_COLOR; i++){
				GPIO_PinOutSet(informationLed[LED_1][i].GPIOportLed, informationLed[LED_1][i].GPIOpinLed);
			}
			emberEventControlSetDelayMS(LED1_blinkLedEventControl, controlModeLed[LED_1].LED_selectControlMode_u.blinkMode.timeLowPulseCycle);
		}
		controlModeLed[LED_1].LED_selectControlMode_u.blinkMode.numberBlinks--;
	} else{
		for(uint8_t i = 0; i < NUMBER_COLOR; i++){
			GPIO_PinOutSet(informationLed[LED_1][i].GPIOportLed, informationLed[LED_1][i].GPIOpinLed);
		}
	}
}

/*
 * @func LED2_blinkLedEventHandler
 * @brief To blink LED_2
 * @param None
 * @retval None
 */
void LED2_blinkLedEventHandler(void)
{
		// Inactivate LED_2 blink event
	emberEventControlSetInactive(LED2_blinkLedEventControl);

		// Control LED_2 blink
	if(controlModeLed[LED_2].LED_selectControlMode_u.blinkMode.numberBlinks != 0){
		if((controlModeLed[LED_2].LED_selectControlMode_u.blinkMode.numberBlinks % 2) == 0){
			for(uint8_t i = 0; i < NUMBER_COLOR; i++){
				if(((controlModeLed[LED_2].color >> i) & 0x01) == 1){
					GPIO_PinOutClear(informationLed[LED_2][i].GPIOportLed, informationLed[LED_2][i].GPIOpinLed);
				} else{
					GPIO_PinOutSet(informationLed[LED_2][i].GPIOportLed, informationLed[LED_2][i].GPIOpinLed);
				}
			}
			emberEventControlSetDelayMS(LED2_blinkLedEventControl, controlModeLed[LED_2].LED_selectControlMode_u.blinkMode.timeHighPulseCycle);
		} else{
			for(uint8_t i = 0; i < NUMBER_COLOR; i++){
				GPIO_PinOutSet(informationLed[LED_2][i].GPIOportLed, informationLed[LED_2][i].GPIOpinLed);
			}
			emberEventControlSetDelayMS(LED2_blinkLedEventControl, controlModeLed[LED_2].LED_selectControlMode_u.blinkMode.timeLowPulseCycle);
		}
		controlModeLed[LED_2].LED_selectControlMode_u.blinkMode.numberBlinks--;
	} else{
		for(uint8_t i = 0; i < NUMBER_COLOR; i++){
			GPIO_PinOutSet(informationLed[LED_2][i].GPIOportLed, informationLed[LED_2][i].GPIOpinLed);
		}
	}
}

	/*------------------------------- END THIS FILE -------------------------------*/


