/*
 * MANAGE_lib.c
 *
 *  Created on: Nov 8, 2024
 *  Author: TRAN DOAN MANH
 *  Description: To store/remove the information of all devices in Zigbee network into/from NVM3 to manage & control all that devices
 */

	/*------------------------------- All libraries -------------------------------*/

#include "app/framework/include/af.h"
#include "source/app/main/main.h"
#include "source/mid/MANAGE_lib/MANAGE_lib.h"

	/*------------------------------- All functions -------------------------------*/

/*
 * @func MANAGE_saveInfDeviceIntoNVM3
 * @brief To store the information of device when joining the Zigbee network
 * @param
 * 		  deviceNetworkID - The network address of device reports to ZC
 * 		  modelID - The name of report device
 * @retval Result of function execution
 */
bool MANAGE_saveInfDeviceIntoNVM3(EmberNodeId deviceNetworkID, char *modelID)
{
		// Get the value of token "DEVICE_INFORMATION"
	halCommonGetToken(&MANAGE_infAllDevices, TOKEN_DEVICE_INFORMATION);

		// Store the device information into token "DEVIVE INFORMATION"
	if(MANAGE_infAllDevices.numberDevices == 0){
		MANAGE_infAllDevices.infDevice[0].networkIdDevice = NETWORKID_ZC;
		MANAGE_infAllDevices.infDevice[0].typeDevice = TOKENINF_ZC;
	} else{
		if(MANAGE_IsDeviceInfStoredIntoNVM3(deviceNetworkID)){
			return false;
		} else{
			TOKENINF_typeDevice_e deviceType = MANAGE_identifyDeviceType(modelID);
			MANAGE_infAllDevices.infDevice[MANAGE_infAllDevices.numberDevices].networkIdDevice = deviceNetworkID;
			MANAGE_infAllDevices.infDevice[MANAGE_infAllDevices.numberDevices].typeDevice = deviceType;
		}
	}

		// Calculate the number of elements of array "infDevice" and store the upper data into NVM3
	MANAGE_infAllDevices.numberDevices++;
	halCommonSetToken(TOKEN_DEVICE_INFORMATION, &MANAGE_infAllDevices);

	return true;
}

/*
 * @func MANAGE_IsDeviceInfStoredIntoNVM3
 * @brief To check whether the device information is stored into NVM3
 * @param deviceNetworkID - The network address of device reports to ZC
 * @retval True/False
 */
bool MANAGE_IsDeviceInfStoredIntoNVM3(EmberNodeId deviceNetworkID)
{
	for(uint8_t i = 0; i < MANAGE_infAllDevices.numberDevices; i++){
		if(MANAGE_infAllDevices.infDevice[i].networkIdDevice == deviceNetworkID)
			return true;
	}

	return false;
}

/*
 * @func MANAGE_identifyDeviceType
 * @brief To identify the type of device in Zigbee network to store into NVM3
 * @param modelID - The name of report device
 * @retval The type of device
 */
TOKENINF_typeDevice_e MANAGE_identifyDeviceType(char* modelID)
{
	if(strcmp(modelID, "UP") == 0){
		return TOKENINF_UpperStair;
	} else if(strcmp(modelID, "DOWN") == 0){
		return TOKENINF_BelowStair;
	} else
		return TOKENINF_Undefined;
}

/*
 * @func MANAGE_removeInfDeviceInNVM3
 * @brief To remove a device in NVM3 when it leaves the Zigbee network
 * @param networkID_device - The network address of device reports to ZC
 * @retval None
 */
void MANAGE_removeInfDeviceInNVM3(EmberNodeId networkID_device)
{
		// Get the value of token "DEVICE_INFORMATION"
	halCommonGetToken(&MANAGE_infAllDevices, TOKEN_DEVICE_INFORMATION);

		// Variable to get the device index
	uint8_t indexDevice;
	indexDevice = MANAGE_getIndexDeviceInNVM3(networkID_device);

		// Remove and store new data into NVM3
	if(indexDevice != INVALID_DEVICE_INDEX){
		for(uint8_t i = indexDevice; i < MANAGE_infAllDevices.numberDevices; i++){
			MANAGE_infAllDevices.infDevice[i] = MANAGE_infAllDevices.infDevice[i+1];
		}
		MANAGE_infAllDevices.numberDevices--;
	}
	halCommonSetToken(TOKEN_DEVICE_INFORMATION, &MANAGE_infAllDevices);
}

/*
 * @func MANAGE_getIndexDeviceInNVM3
 * @brief To get ordinal number of array element which saves the device information in NVM3
 * @param deviceNetworkID - The network address of device reports to ZC
 * @retval The ordinal number of array element
 */
uint8_t MANAGE_getIndexDeviceInNVM3(EmberNodeId deviceNetworkID)
{
	for(uint8_t i = 0; i < MANAGE_infAllDevices.numberDevices; i++){
		if(MANAGE_infAllDevices.infDevice[i].networkIdDevice == deviceNetworkID)
			return i;
	}

	return INVALID_DEVICE_INDEX;
}

/*
 * @func MANAGE_getTypeDeviceInNVM3
 * @brief To get type of device in NVM3
 * @param deviceNetworkID - The network address of device reports to ZC
 * @retval The enum "TOKENINF_deviceType_e"
 */
TOKENINF_typeDevice_e MANAGE_getTypeDeviceInNVM3(EmberNodeId deviceNetworkID)
{
		// Get the value of token "DEVICE_INFORMATION"
	halCommonGetToken(&MANAGE_infAllDevices, TOKEN_DEVICE_INFORMATION);

		// Compare with NetworkID to check
	for(uint8_t i = 0; i < MANAGE_infAllDevices.numberDevices; i++){
		if(MANAGE_infAllDevices.infDevice[i].networkIdDevice == deviceNetworkID)
			return MANAGE_infAllDevices.infDevice[i].typeDevice;
	}

	return TOKENINF_Undefined;
}

/*
 * @func MANAGE_getNetworkIdDeviceInNVM3
 * @brief To get networkID of device in NVM3
 * @param deviceType - The device type to get device networkID in NVM3
 * @retval The networkID of device
 */
EmberNodeId MANAGE_getNetworkIdDeviceInNVM3(TOKENINF_typeDevice_e deviceType)
{
		// Get the value of token "DEVICE_INFORMATION"
	halCommonGetToken(&MANAGE_infAllDevices, TOKEN_DEVICE_INFORMATION);

		// Compare with NetworkID to check
	for(uint8_t i = 0; i < MANAGE_infAllDevices.numberDevices; i++){
		if(MANAGE_infAllDevices.infDevice[i].typeDevice == deviceType)
			return MANAGE_infAllDevices.infDevice[i].networkIdDevice;
	}

	return INVALID_DEVICE_NETWORKID;
}

	/*------------------------------- END THIS FILE -------------------------------*/
