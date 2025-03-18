/*
 * main.h
 *
 *  Created on: May 20, 2024
 *      Author: TRAN DOAN MANH
 */

#ifndef SOURCE_APP_MAIN_MAIN_H_
#define SOURCE_APP_MAIN_MAIN_H_

	/*------------------------------- All libraries -------------------------------*/

#include "app/framework/include/af.h"
#include "zigbee-framework/zigbee-device-common.h"
#include "source/app/NETWORK_lib/NETWORK_lib.h"
#include "source/driver/PIR_lib/PIR_lib.h"
#include "source/mid/BUTTON_lib/BUTTON_lib.h"
#include "source/mid/LED_lib/LED_lib.h"

	/*------------------------------- Declare macros, enum types, variables, events, function prototypes -------------------------------*/

	// Macros
#define CYCLE_READ_BRIGHTNESS_VALUE					30000			// The time cycle to read the brightness value
#define CYCLE_READ_TEMPHUM_VALUE					60000			// The time cycle to read the temperature & humidity value
#define PIR_CONTROLMODE_ENDPOINT_UPPERSW			2				// The pir & control mode endpoint of upper stair SW.
#define STAIRLIGHT_ENDPOINT_UPPERSW					1				// The stair light endpoint of upper stair SW

	// Enum types
typedef enum { // Define the states of below stair switch
	MAIN_StatePowerOn,
	MAIN_StateReport,
	MAIN_StateReboot,
	MAIN_StateIdle
}MAIN_stateBelowStairSW_e;
typedef enum { // Define all endpoints of below stair switch
	MAIN_EndpointStairLight = 1,
	MAIN_EndpointLightSensor,
	MAIN_EndpointHumSensor,
	MAIN_EndpointTempSensor,
	MAIN_EndpointPIRAndControlMode,
	MAIN_EndpointReportDeviceInf
}MAIN_endpoints_e;
typedef enum { // Define the control modes
	MAIN_ModeManual,
	MAIN_ModeAuto,
	MAIN_ModeUndefined
}MAIN_controlMode_e;
typedef enum { // Define the states of stair light, Select MAIN_LightON = 0 to be suitable with the state of GPIO pin
	MAIN_LightON,
	MAIN_LightOFF
}MAIN_stateStairLight_e;
typedef enum { // Define the device types in binding
	MAIN_TargetType,
	MAIN_InitiatorType
}MAIN_deviceTypeInBinding_e;

	// Variables
EmberNodeId MAIN_networkIdBelowSW;									// The networkID of below stair switch
MAIN_controlMode_e MAIN_controlMode;								// The control mode "AUTO" or "MANUAL"
MAIN_stateStairLight_e stateStairLight;								// The state of stair light
EmberAfStatus stateActivateBinding;									// The state of activating "Target" or "Initiator"
MAIN_stateBelowStairSW_e stateBelowSW;								// The state of below stair switch
uint8_t messageLeaveingNetwork[ZDO_MESSAGE_OVERHEAD + 1];			// The content of network leaving message

	// Events
EmberEventControl MAIN_stateBelowStairSWEventControl;			// Event to process the states of below stair switch
void MAIN_stateBelowStairSWEventHandler(void);
EmberEventControl MAIN_readBrightnessValueEventControl;			// Event to read the brightness value
void MAIN_readBirghtnessValueEventHandler(void);
EmberEventControl MAIN_readTempHumValueEventControl;			// Event to read the temperature & humidity value
void MAIN_readTempHumValueEventHandler(void);
EmberEventControl MAIN_rebootBelowStairSWEventControl;			// Event to reboot the below stair switch
void MAIN_rebootBelowStairSWEventHandler(void);

	// Function prototypes
void MAIN_handleZigbeeNetworkStateBelowSW(NETWORK_stateZigbeeNetwork_e networkStateBelowSW);
void MAIN_handleNumberButtonPresses(BUTTON_indexButton_e buttonIndex, BUTTON_numberPresses_e numberPresses);
void MAIN_handleMotionSignal(PIR_stateMotion_e stateMotion);
void MAIN_turnOnOffStairLight(MAIN_stateStairLight_e stairLightState,
						  	  MAIN_endpoints_e valueReportEndpoint,
							  EmberNodeId deviceNetworkID,
							  uint8_t reportCommandID);
void MAIN_synchronizeStateStairLight_ControlMode_PIRtimeWithUpperSW(EmberNodeId networkIdDevice,
										   	   	         	 	 	MAIN_endpoints_e reportEndpoint,
																	uint8_t receptionEndpoint,
																	EmberAfClusterId clusterID,
																	uint8_t commandID);
void MAIN_selectControlMode(MAIN_endpoints_e reportEndpoint,
							MAIN_controlMode_e selectedControlMode,
							LED_ledColor_e modeLED,
							PIR_controlInterrupt_e interruptControl);

	/*------------------------------- END THIS FILE -------------------------------*/

#endif /* SOURCE_APP_MAIN_MAIN_H_ */
