/*
 * main.h
 *
 *  Created on: May 20, 2024
 *      Author: TRAN DOAN MANH
 */

#ifndef SOURCE_APP_MAIN_MAIN_H_
#define SOURCE_APP_MAIN_MAIN_H_

	/*------------------------------- All libraries -------------------------------*/

#include "zigbee-framework/zigbee-device-common.h"
#include "source/app/NETWORK_lib/NETWORK_lib.h"
#include "source/driver/PIR_lib/PIR_lib.h"
#include "source/mid/BUTTON_lib/BUTTON_lib.h"
#include "source/mid/LED_lib/LED_lib.h"

	/*------------------------------- Declare macros, enum types, variables, events, function prototypes -------------------------------*/

	// Macros
#define PIR_CONTROLMODE_ENDPOINT_BELOWSW			5				// The pir & control mode endpoint of below stair SW
#define STAIRLIGHT_ENDPOINT_BELOWSW					1				// The stair light endpoint of below stair SW

	// Enum types
typedef enum { // Define the states of upper stair switch
	MAIN_StatePowerOn,
	MAIN_StateReport,
	MAIN_StateReboot,
	MAIN_StateIdle
}MAIN_stateBelowStairSW_e;
typedef enum { // Define all endpoints of upper stair switch
	MAIN_EndpointStairLight = 1,
	MAIN_EndpointPIRAndControlMode,
	MAIN_EndpointReportDeviceInf
}MAIN_endpoints_e;
typedef enum { // Define the control modes
	MAIN_ModeManual,
	MAIN_ModeAuto,
	MAIN_ModeUndefined
}MAIN_controlMode_e;
typedef enum { // Define the states of stair light
	MAIN_LightOFF,
	MAIN_LightON
}MAIN_stateStairLight_e;
typedef enum { // Define the device types in binding
	MAIN_TargetType,
	MAIN_InitiatorType
}MAIN_deviceTypeInBinding_e;

	// Variables
EmberNodeId MAIN_networkIdUpperSW;									// The networkID of upper stair switch
MAIN_controlMode_e MAIN_controlMode;								// The control mode "AUTO" or "MANUAL"
//MAIN_stateStairLight_e MAIN_stateStairLight;						// The state of stair light
EmberAfStatus stateActivateBinding;									// The state of activating "Target" or "Initiator"
MAIN_stateBelowStairSW_e stateUpperSW;								// The state of upper stair switch
uint8_t messageLeaveingNetwork[ZDO_MESSAGE_OVERHEAD + 1];			// The content of network leaving message

	// Events
EmberEventControl MAIN_stateUpperStairSWEventControl;			// Event to process the states of upper stair switch
void MAIN_stateUpperStairSWEventHandler(void);
EmberEventControl MAIN_rebootUpperStairSWEventControl;			// Event to reboot the upper stair switch
void MAIN_rebootUpperStairSWEventHandler(void);

	// Function prototypes
void MAIN_handleZigbeeNetworkStateUpperSW(NETWORK_stateZigbeeNetwork_e networkStateUpperSW);
void MAIN_handleNumberButtonPresses(BUTTON_indexButton_e buttonIndex, BUTTON_numberPresses_e numberPresses);
void MAIN_handleMotionSignal(PIR_stateMotion_e stateMotion);
void MAIN_synchronizeStateStairLight_ControlMode_PIRtimeWithBelowSW(EmberNodeId networkID,
										   	   	         	 	 	MAIN_endpoints_e reportEndpoint,
																	uint8_t receptionEndpoint,
																	EmberAfClusterId clusterID,
																	uint8_t commandID);
void MAIN_selectControlMode(MAIN_controlMode_e selectedControlMode,
							LED_ledColor_e modeLED,
							PIR_controlInterrupt_e interruptControl);

	/*------------------------------- END THIS FILE -------------------------------*/

#endif /* SOURCE_APP_MAIN_MAIN_H_ */
