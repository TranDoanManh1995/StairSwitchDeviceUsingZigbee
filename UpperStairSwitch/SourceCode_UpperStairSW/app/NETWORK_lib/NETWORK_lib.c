/*
 * NETWORK_lib.c
 *
 *  Created on: Jan 4, 2024
 *  Author: TRAN DOAN MANH
 *  Descriptor: To find & join the Zigbee network
 */

	/*------------------------------- All libraries -------------------------------*/

#include "app/framework/include/af.h"
#include "source/app/NETWORK_lib/NETWORK_lib.h"
#include "source/mid/LED_lib/led_lib.h"

	/*------------------------------- Initialize function pointer & variable -------------------------------*/

	// Function pointer
NETWORK_handleZigbeeNetworkState_t handleZigbeeNetworkState = NULL;

	// Variable
uint8_t numberFindingNetwork = 0;

	/*------------------------------- All functions -------------------------------*/

/*
 * @func NETWORK_initializeZigbeeNetwork
 * @brief To register process function when the state of network changes
 * @param handleZigbeeNetworkStateCallback - The callback function will be called to process the Zigbee network state of device
 * @retval None
 */
void NETWORK_initializeZigbeeNetwork(NETWORK_handleZigbeeNetworkState_t handleZigbeeNetworkStateCallback)
{
	if(handleZigbeeNetworkStateCallback != NULL){
		handleZigbeeNetworkState = handleZigbeeNetworkStateCallback;
	}
}

/*
 * @func NETWORK_findJoinZigbeeNetwork
 * @brief To initialize the event "finding Zigbee network" after 2000 ms
 * @param None
 * @retval None
 */
void NETWORK_findJoinZigbeeNetwork(void)
{
	if(emberAfNetworkState() == EMBER_NO_NETWORK){
		emberEventControlSetDelayMS(NETWORK_findJoinZigbeeNetworkEventControl, 2000);
	}
}

/*
 * @func NETWORK_findJoinZigbeeNetworkEventHandler
 * @brief To find and join the Zigbee network
 * @param None
 * @retval None
 */
void NETWORK_findJoinZigbeeNetworkEventHandler(void)
{
		// Inactivate network finding event
	emberEventControlSetInactive(NETWORK_findJoinZigbeeNetworkEventControl);

		// Find & join the Zigbee network
	if(emberAfNetworkState() == EMBER_NO_NETWORK){
		emberAfPluginNetworkSteeringStart();
		LED_blinkLed(LED_2, LED_Red, 1, 500, 500);
		numberFindingNetwork++;
		emberEventControlSetDelayMS(NETWORK_findJoinZigbeeNetworkEventControl, 5000);
	}
}

/*
 * @func NETWORK_stopFindingZigbeeNetwork
 * @brief To stop finding the Zigbee network
 * @param None
 * @retval None
 */
void NETWORK_stopFindingZigbeeNetwork(void)
{
	emberAfPluginNetworkSteeringStop();
}

/*
 * @func emberAfStackStatusCallback
 * @brief This function is called by the application framework from the stack status
 *		  handler. This callbacks provides applications an opportunity to be notified
 * 		  of changes to the stack status and take appropriate action
 * @param status - The Zigbee network state of device
 * @retval True/False
 */
bool emberAfStackStatusCallback(EmberStatus status)
{
		// Check the state of network to process
	if(status == EMBER_NETWORK_UP){
		if(numberFindingNetwork > 0){
			NETWORK_stopFindingZigbeeNetwork();
			if(handleZigbeeNetworkState != NULL){
				handleZigbeeNetworkState(NETWORK_JoinSuccess);
			}
			numberFindingNetwork = 0;
		} else{
			if(handleZigbeeNetworkState != NULL){
				handleZigbeeNetworkState(NETWORK_Jointed);
			}
		}
	} else{
		if(emberAfNetworkState() == EMBER_NO_NETWORK){
			if(handleZigbeeNetworkState != NULL){
				handleZigbeeNetworkState(NETWORK_OutNetwork);
			}
		} else if(emberAfNetworkState() == EMBER_JOINED_NETWORK_NO_PARENT){
			handleZigbeeNetworkState(NETWORK_LostParent);
		}
	}

		// This value is ignored by the framework.
	return false;
}

	/*------------------------------- END THIS FILE -------------------------------*/
