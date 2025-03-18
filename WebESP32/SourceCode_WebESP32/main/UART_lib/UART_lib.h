/*
 * UART_lib.h
 * 
 *  Author: Tran Doan Manh
*/

#ifndef _UART_LIB_H
#define _UART_LIB_H

    /*------------------------------- All libraries -------------------------------*/
    
#include "stdint.h"
#include "Manage_lib/Manage_lib.h"

    /*------------------------------- Declare macros, enum & structure types, function prototypes -------------------------------*/

    // Macros
#define UART2_TX_PIN            17                  // Pin TX of UART2
#define UART2_RX_PIN            16                  // Pin RX of UART2
#define SIZE_RX_BUFFER          1024                // Dimension of data buffer in queue 
#define BYTE_START         		0xB1			    // Define the first byte of the UART transmission frame
#define BYTE_CXOR				0xFF                // Define the first byte to calculate value of byte XOR in received message 
#define DIMENSION_ARRAY			16                  // Dimension of received data array

    // Enum types
typedef enum { // Define all UART commands to communicate with ESP32
	UART_CreateZigbeeNetwork = 1,
	UART_OpenZigbeeNetwork,
	UART_CloseZigbeeNetwork,
	UART_JoinZigbeeNetwork,
	UART_LeaveZigbeeNetwork,
	UART_ReportValueTemperature,
	UART_ReportValueHumidity,
	UART_ReportValueBrightness,
	UART_ControlMode,
	UART_TurnOnOffLight
}UART_commandID_e;
typedef enum { // Define the types of communication messages between ESP32 & ZC
	UART_TypeSet = 1,			// The control message type sends to ZC by ESP32
	UART_TypeReport				// The report message type sends to ESP32 by ZC
}UART_typeCommand_e;
typedef enum { // Define the types of bytes in UART transmission frame
    UART_ByteStart,
    UART_BytesData,
    UART_ByteCXOR
}UART_typeReceivedByte_e;
typedef enum { // Define the states of received data
	UART_NoReceivedData,
	UART_ValidReceivedData,
	UART_InvalidReceivedData
}UART_stateReceivedData_e;
typedef enum { // Define to select the stair light controlling mode
	UART_ModeManual,
	UART_ModeAuto
}UART_controlMode_e;
typedef enum { // Define to control the stair light
	UART_TurnOffLight,
	UART_TurnOnLight
}UART_controlLight_e;

	// Structure types
typedef struct { // Define UART data frame
	uint8_t lowByteNetworkID;
	uint8_t highByteNetworkID;
	uint8_t valueEndpoint;
	uint8_t commandID;
	uint8_t typeCommand;
	uint8_t lengthReportData;
	uint8_t reportData[2];
}UART_frameUART_s;

	// Define a function pointer & create a function to call the processing callback
typedef void (*UART_handleReceivedDataFromZC)(uint8_t* );
void UART_initializeUart2(UART_handleReceivedDataFromZC );

	// Prototype of reception functions
void UART_handleRXevents(void *pvParameters);
void UART_handleInstancesReceivedDataFromZigbeeCoordinator(void);
UART_stateReceivedData_e UART_handleDataInQueue(void);

	// Prototype of transmission functions
void UART_GenerateTransmissionFrame(uint16_t deviceNetworkID,
									uint8_t controlEndpoint,
									UART_commandID_e controlCommandID,
									UART_typeCommand_e commandType,
									uint8_t *controlData,
									uint8_t lengthControlData);
uint8_t UART_calculateXORbyteTransmissionFrame(uint8_t *payloadToTransmit, uint8_t lengthPayload);
void UART_createZigbeeNetwork(uint16_t valueDeviceNetworkID, uint8_t valueControlEndpoint);
void UART_openZigbeeNetwork(uint16_t valueDeviceNetworkID, uint8_t valueControlEndpoint);
void UART_closeZigbeeNetwork(uint16_t valueDeviceNetworkID, uint8_t valueControlEndpoint);
void UART_removeStairSwitchInZigbeeNetwork(uint16_t valueDeviceNetworkID, 
                                           uint8_t valueControlEndpoint,
                                           MANAGE_typeDevice_e deviceType);
void UART_selectControlMode(UART_controlMode_e controlMode);
void UART_controlStairLight(MANAGE_typeDevice_e deviceType, UART_controlLight_e controlLight);

#endif

    /*------------------------------- END THIS FILE -------------------------------*/