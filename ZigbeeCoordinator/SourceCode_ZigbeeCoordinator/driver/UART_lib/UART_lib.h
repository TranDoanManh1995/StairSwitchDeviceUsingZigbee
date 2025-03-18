/*
 * UART_lib.h
 *
 *  Created on: Nov 12, 2024
 *      Author: TRAN DOAN MANH
 */

#ifndef SOURCE_DRIVER_UART_LIB_UART_LIB_H_
#define SOURCE_DRIVER_UART_LIB_UART_LIB_H_

	/*------------------------------- All libraries -------------------------------*/

#include "app/framework/include/af.h"
#include "source/app/RECEIVE_lib/RECEIVE_lib.h"
#include "source/mid/QUEUE_lib/QUEUE_lib.h"

	/*------------------------------- Declare macros, enum types, variables & function prototypes -------------------------------*/

	// Macros
#define BYTE_START					0xB1			// Define the first byte of the UART transmission frame
#define CYCLE_READING_DATA			100				// Cycle time to read data is transmitted by ESP32
#define BYTE_CXOR					0xFF			// The first byte to calculate CXOR
#define DIMENSION_ARRAY				16				// The dimension of array to store data is sent by ESP32

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

	// Variables
uint8_t transmissionPayload[DIMENSION_ARRAY];				// An array contains data to transmit to ESP32
uint8_t receptionPayload[DIMENSION_ARRAY];					// An array contains data is sent by ESP32
uint16_t lengthReceptionPayload;
queue_p pQueue;												// A pointer variable of queue structure
uint8_t dataSendByESP32[DIMENSION_ARRAY];					// An array to store data from queue buffer
uint8_t indexArray;
uint8_t calculateXORbyte;									// Calculate the CXOR byte in UART reception frame

	// Define a function pointer & create a function to call the processing callback
typedef void (*UART_handleReceivedDataFromESP32)(uint8_t* );
void UART_initializeUart(UART_handleReceivedDataFromESP32 );

	// Event of reading data from USART2
EmberEventControl UART_readDataUSART2EventControl;
void UART_readDataUSART2EventHandler(void);

	// Prototype of transmission functions
void UART_GenerateTransmissionFrame(EmberNodeId valueDeviceNetworkID,
									RECEIVE_valueEndpoint valueReportEndpoint,
									UART_commandID_e reportCommandID,
									UART_typeCommand_e commandType,
									uint8_t *reportData,
									uint8_t lengthReportData);
uint8_t UART_calculateXORbyteTransmissionFrame(uint8_t *payloadToTransmit, uint8_t lengthPayload);
void UART_FRAMEreportJoinZigbeeNetwork(EmberNodeId deviceNetworkID,
									   RECEIVE_valueEndpoint reportEndpoint,
									   TOKENINF_typeDevice_e deviceType);
void UART_FRAMEreportLeaveZigbeeNetwork(EmberNodeId deviceNetworkID,
									    RECEIVE_valueEndpoint reportEndpoint);
void UART_FRAMEreportTemperatureValue(EmberNodeId deviceNetworkID,
									  RECEIVE_valueEndpoint reportEndpoint,
									  uint8_t valueTemp);
void UART_FRAMEreportHumidityValue(EmberNodeId deviceNetworkID,
								   RECEIVE_valueEndpoint reportEndpoint,
								   uint8_t valueHum);
void UART_FRAMEreportBrightnessValue(EmberNodeId deviceNetworkID,
		 	 	 	 	 	 	 	 RECEIVE_valueEndpoint reportEndpoint,
									 uint8_t* valueBrightness);
void UART_FRAMEreportControlMode(EmberNodeId deviceNetworkID,
								 RECEIVE_valueEndpoint reportEndpoint,
								 uint8_t controlMode);
void UART_FRAMEreportOnOffLight(EmberNodeId deviceNetworkID,
								RECEIVE_valueEndpoint reportEndpoint,
								uint8_t stateStairLight);

	// Prototype of reception functions
void UART_handleInstancesReceivedDataFromESP32(void);
UART_stateReceivedData_e UART_handleDataInQueue(void);

	/*------------------------------- END THIS FILE -------------------------------*/

#endif /* SOURCE_DRIVER_UART_LIB_UART_LIB_H_ */
