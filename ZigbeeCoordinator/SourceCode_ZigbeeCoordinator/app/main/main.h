/*
 * main.h
 *
 *  Created on: Jun 4, 2024
 *      Author: TRAN DOAN MANH
 */

#ifndef SOURCE_APP_MAIN_MAIN_H_
#define SOURCE_APP_MAIN_MAIN_H_

	/*------------------------------- All libraries -------------------------------*/

#include "source/app/RECEIVE_lib/RECEIVE_lib.h"
#include "source/mid/BUTTON_lib/BUTTON_lib.h"

	/*------------------------------- Declare macro, enum & structure types, variables, event & function prototypes -------------------------------*/

	// Macro
#define NETWORKID_ZC			0x0000			// The networkID of Zigbee Coordinator
#define ENDPOINT_ZC				0x01			// The endpoint value of Zigbee Coordinator

	// Enum types
typedef enum { // Define the states of Zigbee Coordinator
	MAIN_StatePowerOn,
	MAIN_StateReport,
	MAIN_StateReboot,
	MAIN_StateIdle
}MAIN_stateZigbeeCoordinator_e;
typedef enum { // Define to control the stair light
	MAIN_LightOFF,
	MAIN_LightON
}MAIN_controlStairLight_e;
typedef enum { // Define to select the stair light controlling mode
	MAIN_ModeManual,
	MAIN_ModeAuto
}MAIN_controlMode_e;

	// Structure types
typedef struct { // Define UART data frame
	uint8_t lowByteNetworkID;
	uint8_t highByteNetworkID;
	uint8_t valueEndpoint;
	uint8_t commandID;
	uint8_t typeCommand;
	uint8_t lengthControlData;
	uint8_t controlData[2];
}MAIN_frameUART_s;

	// Variables
MAIN_stateZigbeeCoordinator_e stateZigbeeCoordinator;

	// Event to process the states of Zigbee Coordiantor
EmberEventControl MAIN_stateZigbeeCoordinatorEventControl;
void MAIN_stateZigbeeCoordinatorEventHandler(void);

	// Function prototypes
void MAIN_handleNumberButtonPresses(BUTTON_indexButton_e buttonIndex, BUTTON_numberPresses_e numberPresses);
void MAIN_handleReportsFromDevicesZigbeeNetwork(EmberNodeId networkIdDevice,
												RECEIVE_valueEndpoint valueReportEndpoint,
												RECEIVE_typeReportMessage_e typeReportMessage,
												uint8_t *contentReport);
void MAIN_handleReceivedDataFromESP32(uint8_t* receivedData);

	/*------------------------------- END THIS FILE -------------------------------*/

#endif /* SOURCE_APP_MAIN_MAIN_H_ */
