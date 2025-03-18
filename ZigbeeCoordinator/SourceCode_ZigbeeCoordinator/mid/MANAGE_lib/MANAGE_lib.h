/*
 * MANAGE_lib.h
 *
 *  Created on: Nov 8, 2024
 *      Author: TRAN DOAN MANH
 */

#ifndef SOURCE_MID_MANAGE_LIB_MANAGE_LIB_H_
#define SOURCE_MID_MANAGE_LIB_MANAGE_LIB_H_

	/*------------------------------- All libraries -------------------------------*/

#include "app/framework/include/af.h"
#include "token-deviceInformation.h"
#include "stdbool.h"

	/*------------------------------- Declare macros, variables & function prototypes -------------------------------*/

	// Macros
#define INVALID_DEVICE_INDEX				0xFF			// The invalid value of device index in NVM3
#define INVALID_DEVICE_NETWORKID			0xFFFF			// The invalid value of networkID in NVM3

	// Variables
TOKENINF_infAllDevices_s MANAGE_infAllDevices;			// Store all devices in Zigbee network into NVM3

	// Function prototypes
bool MANAGE_saveInfDeviceIntoNVM3(EmberNodeId deviceNetworkID, char *modelID);
bool MANAGE_IsDeviceInfStoredIntoNVM3(EmberNodeId deviceNetworkID);
TOKENINF_typeDevice_e MANAGE_identifyDeviceType(char* modelID);
void MANAGE_removeInfDeviceInNVM3(EmberNodeId networkID_device);
uint8_t MANAGE_getIndexDeviceInNVM3(EmberNodeId deviceNetworkID);
TOKENINF_typeDevice_e MANAGE_getTypeDeviceInNVM3(EmberNodeId deviceNetworkID);
EmberNodeId MANAGE_getNetworkIdDeviceInNVM3(TOKENINF_typeDevice_e deviceType);

	/*------------------------------- END THIS FILE -------------------------------*/

#endif /* SOURCE_MID_MANAGE_LIB_MANAGE_LIB_H_ */
