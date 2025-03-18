/*
 * NETWORK_lib.h
 *
 *  Created on: Jan 4, 2024
 *      Author: TRAN DOAN MANH
 */

#ifndef SOURCE_APP_NETWORK_LIB_NETWORK_LIB_H_
#define SOURCE_APP_NETWORK_LIB_NETWORK_LIB_H_

	/*------------------------------- All libraries -------------------------------*/

#include "app/framework/include/af.h"

	/*------------------------------- Declare enum types, variables, event, function pointer & function prototypes -------------------------------*/

	// Enum types
typedef enum { // Define the network states of device
	NETWORK_JoinSuccess,
	NETWORK_JoinFail,
	NETWORK_Jointed,
	NETWORK_OutNetwork,
	NETWORK_LostParent,
}NETWORK_stateZigbeeNetwork_e;

	// Event of finding & joining Zigbee network
EmberEventControl NETWORK_findJoinZigbeeNetworkEventControl;
void NETWORK_findJoinZigbeeNetworkEventHandler(void);

	// Define a function pointer & create a function to call the processing callback
typedef void (*NETWORK_handleZigbeeNetworkState_t)(NETWORK_stateZigbeeNetwork_e );
void NETWORK_initializeZigbeeNetwork(NETWORK_handleZigbeeNetworkState_t );

	// Function prototypes
void NETWORK_findJoinZigbeeNetwork(void);
void NETWORK_stopFindingZigbeeNetwork(void);

	/*------------------------------- END THIS FILE -------------------------------*/

#endif /* SOURCE_APP_NETWORK_LIB_NETWORK_LIB_H_*/
