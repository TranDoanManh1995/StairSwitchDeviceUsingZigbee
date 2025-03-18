/*
 * Manage_lib.c
 *
 *  Author: TRAN DOAN MANH
 *  Descriptor: To manage all devices in Zigbee Network
 */

	/*------------------------------- All libraries -------------------------------*/

#include "stdint.h"
#include "stdbool.h"
#include "Manage_lib.h"
#include "UART_lib/UART_lib.h"

    /*------------------------------- Declare & initialize variables -------------------------------*/

    // Variables
MANAGE_infAllDevices_s MANAGE_infAllDevices;			// Store devices information in Zigbee network

	/*------------------------------- All functions -------------------------------*/

/*
 * @func MANAGE_saveInfDeviceToManage
 * @brief To store the information of device when joining the Zigbee network
 * @param receivedData - The data receives from Zigbee Coordinator
 * @retval None
 */
void MANAGE_saveInfDeviceToManage(uint8_t *receivedData)
{
		// Variables
	UART_frameUART_s *frameReportData = (UART_frameUART_s *)receivedData;												// Structure variable to process received data
    uint16_t deviceNetworkID = frameReportData->lowByteNetworkID | (frameReportData->highByteNetworkID << 8);			// Get the networkID of device
	UART_commandID_e receivedCommandID = frameReportData->commandID;													// Get the name of command
	uint8_t indexDevice = MANAGE_infAllDevices.numberDevices;											        		// Get location to save the device information
	
        // Store the device information
	if(receivedCommandID == UART_JoinZigbeeNetwork){
		if(MANAGE_infAllDevices.numberDevices == 0){ 
			MANAGE_infAllDevices.infDevice[0].networkIdDevice = NETWORKID_ZC;
			MANAGE_infAllDevices.infDevice[0].typeDevice = MANAGE_ZCDevice;
        	MANAGE_infAllDevices.infDevice[0].infEndpoint[0].valueEndpoint = ENDPOINT_ZC;
        	MANAGE_infAllDevices.infDevice[0].infEndpoint[0].typeEndpoint = MANAGE_JoinLeaveNetwork;
			MANAGE_infAllDevices.infDevice[0].numberEndpoints++;
		} else{
			if(MANAGE_IsDeviceInfStored(deviceNetworkID)){
				return;
			} else{
				MANAGE_typeDevice_e deviceType = MANAGER_identifyDeviceType(frameReportData->reportData[0]);
				MANAGE_infAllDevices.infDevice[indexDevice].networkIdDevice = deviceNetworkID;
				MANAGE_infAllDevices.infDevice[indexDevice].typeDevice = deviceType;
        		MANAGE_infAllDevices.infDevice[indexDevice].infEndpoint[0].valueEndpoint = frameReportData->valueEndpoint;
        		MANAGE_infAllDevices.infDevice[indexDevice].infEndpoint[0].typeEndpoint = MANAGE_JoinLeaveNetwork;
				MANAGE_infAllDevices.infDevice[indexDevice].numberEndpoints++;
			}
		}
			// Calculate the number of elements of array "infDevice"
		MANAGE_infAllDevices.numberDevices++;
	}
}

/*
 * @func MANAGE_IsDeviceInfStored
 * @brief To check whether the device information is stored 
 * @param deviceNetworkID - The network address of device reports to ESP32
 * @retval True/False
 */
bool MANAGE_IsDeviceInfStored(uint16_t deviceNetworkID)
{
	for(uint8_t i = 0; i < MANAGE_infAllDevices.numberDevices; i++){
		if(MANAGE_infAllDevices.infDevice[i].networkIdDevice == deviceNetworkID)
			return true;
	}

	return false;
}

/*
 * @func MANAGER_identifyDeviceType
 * @brief To identify the type of device in Zigbee network to store
 * @param deviceType - The name of device is sent by Zigbee Coordinator
 * @retval The name of device
 */
MANAGE_typeDevice_e MANAGER_identifyDeviceType(uint8_t deviceType)
{
	if(deviceType == MANAGE_UpperStairDevice){
		return MANAGE_UpperStairDevice;
	} else if(deviceType == MANAGE_BelowStairDevice){
		return MANAGE_BelowStairDevice;
	} else
		return MANAGE_Undefined;
}

/*
 * @func MANAGE_getDeviceIndex
 * @brief To get index of device in information storing array
 * @param deviceNetworkID - The network address of device reports to ESP32
 * @retval The index of device in information storing array
 */
uint8_t MANAGE_getDeviceIndex(uint16_t deviceNetworkID)
{
	for(uint8_t i = 0; i < MANAGE_infAllDevices.numberDevices; i++){
		if(MANAGE_infAllDevices.infDevice[i].networkIdDevice == deviceNetworkID)
			return i;
	}

	return INVALID_DEVICE_INDEX;
}

/*
 * @func MANAGER_getDeviceType
 * @brief To get type of device in array
 * @param deviceNetworkID - The network address of device reports to ESP32
 * @retval The type of device
 */
MANAGE_typeDevice_e MANAGER_getDeviceType(uint16_t deviceNetworkID)
{
	for(uint8_t i = 0; i < MANAGE_infAllDevices.numberDevices; i++){
		if(MANAGE_infAllDevices.infDevice[i].networkIdDevice == deviceNetworkID)
			return MANAGE_infAllDevices.infDevice[i].typeDevice;
	}

	return MANAGE_Undefined;
}

/*
 * @func MANAGE_saveEndpointDevice
 * @brief To store endpoint and type of it
 * @param 
 * 		 indexDevice - The index of device in array "deviceInformation"
 * 		 indexEndpoint - The location to save new endpoint of device in array "infEndpoint"
 * 		 endpointValue - The value of endpoint
 * 		 endpointType - The content of endpoint
 * @retval None
 */
void MANAGE_saveEndpointDevice(uint8_t indexDevice, 
							   uint8_t indexEndpoint,
							   uint8_t endpointValue, 
							   MANAGE_typeEndpoint_e endpointType)
{
    MANAGE_infAllDevices.infDevice[indexDevice].infEndpoint[indexEndpoint].valueEndpoint = endpointValue;
    MANAGE_infAllDevices.infDevice[indexDevice].infEndpoint[indexEndpoint].typeEndpoint = endpointType;
	MANAGE_infAllDevices.infDevice[indexDevice].numberEndpoints++;
}

/*
 * @func MANAGE_getEndpointValueDevice
 * @brief To get endpoint value of devive to control
 * @param 
 * 		 deviceNetworkID - The network address of device to control
 * 		 endpointType - The content of endpoint
 * @retval None
 */
uint8_t MANAGE_getEndpointValueDevice(uint16_t deviceNetworkID, 
							   		  MANAGE_typeEndpoint_e endpointType)
{
	for(uint8_t i = 0; i < MANAGE_infAllDevices.numberDevices; i++){
		if(MANAGE_infAllDevices.infDevice[i].networkIdDevice == deviceNetworkID){
			for(uint8_t j = 0; j < MANAGE_infAllDevices.infDevice[i].numberEndpoints; j++){
				if(MANAGE_infAllDevices.infDevice[i].infEndpoint[j].typeEndpoint == endpointType){
					return MANAGE_infAllDevices.infDevice[i].infEndpoint[j].valueEndpoint;
				}
			}
		}	
	}

	return INVALID_ENDPOINT_VALUE;
}

/*
 * @func MANAGE_removeInfDevice
 * @brief To remove the device information when it leaves the Zigbee network
 * @param indexDevice - The index of device in array
 * @retval None
 */
void MANAGE_removeInfDevice(uint8_t indexDevice)
{
	if(indexDevice != INVALID_DEVICE_INDEX){
		for(uint8_t i = indexDevice; i < MANAGE_infAllDevices.numberDevices; i++){
			MANAGE_infAllDevices.infDevice[i] = MANAGE_infAllDevices.infDevice[i+1];
		}
		MANAGE_infAllDevices.numberDevices--;
	}
}

	/*------------------------------- END THIS FILE -------------------------------*/