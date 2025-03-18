/*
 * Manage_lib.h
 *
 *  Author: TRAN DOAN MANH
 */

#ifndef _MANAGE_LIB_H_
#define _MANAGE_LIB_H_

	/*------------------------------- All libraries -------------------------------*/

#include "stdint.h"
#include "stdbool.h"

	/*------------------------------- Declare macro, enum & structure types & function prototypes -------------------------------*/

	// Macro
#define INVALID_DEVICE_INDEX			0xFF			// The invalid index of device
#define INVALID_ENDPOINT_VALUE			0xFF			// The invalid index of device
#define NETWORKID_ZC					0x0000			// The networkID of Zigbee Coordinator
#define ENDPOINT_ZC						0x01			// The endpoint value of Zigbee Coordinator

	// Enum types
typedef enum { // Define type of devices in Zigbee network
	MANAGE_ZCDevice,
	MANAGE_UpperStairDevice,
	MANAGE_BelowStairDevice,
	MANAGE_AllDevices,
	MANAGE_Undefined
}MANAGE_typeDevice_e;
typedef enum { // Define type of endpoints 
	MANAGE_JoinLeaveNetwork,
	MANAGE_TempSensor,
	MANAGE_HumSensor,
	MANAGE_LightSensor,
    MANAGE_ControlMode,
    MANAGE_OnOffLight,
}MANAGE_typeEndpoint_e;

	// Structure types
typedef struct { // Define endpoints of device & type of that endpoints
    uint8_t valueEndpoint;
    MANAGE_typeEndpoint_e typeEndpoint;
}MANAGE_infEndpoint_s;
typedef struct{ // Define to store the information of a device in Zigbee network
	uint16_t networkIdDevice;
	MANAGE_typeDevice_e typeDevice;
    MANAGE_infEndpoint_s infEndpoint[10];
    uint8_t numberEndpoints;
}MANAGE_infDevice_s;
typedef struct{ // Define to store the information of all devices in Zigbee network
	MANAGE_infDevice_s infDevice[10];
	uint8_t numberDevices;
}MANAGE_infAllDevices_s;

	// Function prototypes
void MANAGE_saveInfDeviceToManage(uint8_t *receivedData);
void MANAGE_removeInfDevice(uint8_t indexDevice);
bool MANAGE_IsDeviceInfStored(uint16_t deviceNetworkID);
MANAGE_typeDevice_e MANAGER_identifyDeviceType(uint8_t deviceType);
uint8_t MANAGE_getDeviceIndex(uint16_t deviceNetworkID);
MANAGE_typeDevice_e MANAGER_getDeviceType(uint16_t deviceNetworkID);
void MANAGE_saveEndpointDevice(uint8_t indexDevice, 
							   uint8_t indexEndpoint,
							   uint8_t endpointValue, 
							   MANAGE_typeEndpoint_e endpointType);
uint8_t MANAGE_getEndpointValueDevice(uint16_t deviceNetworkID, 
							   		  MANAGE_typeEndpoint_e endpointType);

#endif 

	/*------------------------------- END THIS FILE -------------------------------*/