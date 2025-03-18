/*
 * TRANSMIT_lib.h
 *
 *  Created on: Nov 16, 2024
 *      Author: TRAN DOAN MANH
 */

#ifndef SOURCE_APP_TRANSMIT_LIB_TRANSMIT_LIB_H_
#define SOURCE_APP_TRANSMIT_LIB_TRANSMIT_LIB_H_

	/*------------------------------- All libraries -------------------------------*/

#include "app/framework/include/af.h"

	/*------------------------------- Declare function prototypes -------------------------------*/

	// Function prototypes
void TRANSMIT_removeDeviceInZigbeeNetwork(EmberNodeId deviceNetworkID);
void TRANSMIT_turnOnOffStairLight(EmberNodeId deviceNetworkID, uint8_t controlEndpoint, uint8_t controlLight);
void TRANSMIT_selecStairLightControlMode(EmberNodeId deviceNetworkID,
										 uint8_t controlEndpoint,
										 uint8_t controlMode);

	/*------------------------------- END THIS FILE -------------------------------*/

#endif /* SOURCE_APP_TRANSMIT_LIB_TRANSMIT_LIB_H_ */
