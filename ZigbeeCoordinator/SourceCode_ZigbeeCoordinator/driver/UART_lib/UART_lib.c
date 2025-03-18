/*
 * UART_lib.c
 *
 *  Created on: Nov 12, 2024
 *  Author: TRAN DOAN MANH
 *  Description: To transmit & receive data between ESP32 & Zigbee Coordinator
 */

	/*------------------------------- All libraries -------------------------------*/

#include "app/framework/include/af.h"
#include "stdlib.h"
#include "source/driver/UART_lib/UART_lib.h"

	/*------------------------------- Initialize variable, function pointer -------------------------------*/

	// Check the start byte of received control data to get control data
UART_typeReceivedByte_e typeReceivedByte = UART_ByteStart;

	// Function pointer
UART_handleReceivedDataFromESP32 handleReceivedDataFromESP32 = NULL;

	/*------------------------------- All functions -------------------------------*/

			/*---------- Initialize UART function ----------*/

/*
 * @func UART_initializeUart
 * @brief To register a callback to process all data that receive from ESP32
 * @param handleReceivedDataFromESP32Callback - The callback function will be called to process received data from ESP32
 * @retval None
 */
void UART_initializeUart(UART_handleReceivedDataFromESP32 handleReceivedDataFromESP32Callback)
{
		// Register the callback function to handle received data
	if(handleReceivedDataFromESP32Callback != NULL){
		handleReceivedDataFromESP32 = handleReceivedDataFromESP32Callback;
	}

		// Initialize the queue
	pQueue = (queue_p)calloc(1, sizeof(queue_t));
	QUEUE_initializeQueue(pQueue);

		// Trigger event to read data using USART2
	emberEventControlSetDelayMS(UART_readDataUSART2EventControl, CYCLE_READING_DATA);
}

		    /*---------- All functions to transmit data via UART ----------*/

/*
 * @func UART_GenerateTransmissionFrame
 * @brief To build the UART transmission frame
 * @param
 * 		  valueDeviceNetworkID - The network address of device reports to ZC
 * 		  valueReportEndpoint - The endpoint of device reports to ZC
 * 		  reportCommandID - To identify command
 * 		  commandType - The type of command
 * 		  reportData - The data to send to ESP32
 * 		  lengthReportData - The length of data to send to ESP32
 * @retval None
 */
void UART_GenerateTransmissionFrame(EmberNodeId valueDeviceNetworkID,
									RECEIVE_valueEndpoint valueReportEndpoint,
									UART_commandID_e reportCommandID,
									UART_typeCommand_e commandType,
									uint8_t *reportData,
									uint8_t lengthReportData)
{
		// Declare & reset the array "transmissionPayload" that is sent to ESP32
	memset(transmissionPayload, 0 , 16);

		// Variable to get the CXOR byte of UART transmission frame
	uint8_t byteXOR;

		// Build the array "transmissionPayload"
	*transmissionPayload = BYTE_START;
	*(transmissionPayload + 1) = lengthReportData + 7;
	*(transmissionPayload + 2) = valueDeviceNetworkID & 0xFF;
	*(transmissionPayload + 3) = (valueDeviceNetworkID >> 8) & 0xFF;
	*(transmissionPayload + 4) = valueReportEndpoint;
	*(transmissionPayload + 5) = reportCommandID;
	*(transmissionPayload + 6) = commandType;
	*(transmissionPayload + 7) = lengthReportData;
	for(uint8_t i = 0; i < lengthReportData; i++){
		*(transmissionPayload + 8 + i) = *(reportData + i);
	}
	byteXOR = UART_calculateXORbyteTransmissionFrame(transmissionPayload, (lengthReportData + 6));
	*(transmissionPayload+ 8 + lengthReportData) = byteXOR;
	emberAfCorePrintln("The report value: %d!\n", (uint16_t *)*reportData);
}

/*
 * @func UART_calculateXORbyteTransmissionFrame
 * @brief To calculate the CXOR byte of UART transmission frame
 * @param
 * 		  payloadToTransmit - UART transmission frame
 * 		  lengthPayload - The length of UART transmission frame
 * @retval The CXOR byte of UART transmission frame
 */
uint8_t UART_calculateXORbyteTransmissionFrame(uint8_t *payloadToTransmit, uint8_t lengthPayload)
{
		// Variable to calculate the CXOR byte of UART transmission frame
	uint8_t XORbyte = BYTE_CXOR;

		// Calculate the CXOR byte of UART transmission frame
	for(uint8_t i = 0; i < lengthPayload; i++){
		XORbyte ^= *(payloadToTransmit + i + 2);
	}

	return XORbyte;
}

/*
 * @func UART_FRAMEreportJoinZigbeeNetwork
 * @brief To create the UART transmission frame to report joining Zigbee network to ESP32
 * @param
 * 		  deviceNetworkID - The network address of device reports to ZC
 * 		  reportEndpoint - The endpoint of device reports to ZC
 *		  deviceType - The type of device reports to ZC
 * @retval None
 */
void UART_FRAMEreportJoinZigbeeNetwork(EmberNodeId deviceNetworkID,
									   RECEIVE_valueEndpoint reportEndpoint,
									   TOKENINF_typeDevice_e deviceType)
{
	UART_GenerateTransmissionFrame(deviceNetworkID,
								   reportEndpoint,
								   UART_JoinZigbeeNetwork,
								   UART_TypeReport,
								   (uint8_t *)&deviceType,
								   1);
}

/*
 * @func UART_FRAMEreportLeaveZigbeeNetwork
 * @brief To create the UART transmission frame to report leaving Zigbee network to ESP32
 * @param
 * 		  deviceNetworkID - The network address of device reports to ZC
 * 		  reportEndpoint - The endpoint of device reports to ZC
 * @retval None
 */
void UART_FRAMEreportLeaveZigbeeNetwork(EmberNodeId deviceNetworkID,
									    RECEIVE_valueEndpoint reportEndpoint)
{
	UART_GenerateTransmissionFrame(deviceNetworkID,
								   reportEndpoint,
								   UART_LeaveZigbeeNetwork,
								   UART_TypeReport,
								   NULL,
								   0);
}

/*
 * @func UART_FRAMEreportTemperatureValue
 * @brief To create the UART transmission frame to report temperature value to ESP32
 * @param
 * 		  deviceNetworkID - The network address of device reports to ZC
 * 		  reportEndpoint - The endpoint of device reports to ZC
 * 		  valueTemp - The temperature value reports to ZC
 * @retval None
 */
void UART_FRAMEreportTemperatureValue(EmberNodeId deviceNetworkID,
									  RECEIVE_valueEndpoint reportEndpoint,
									  uint8_t valueTemp)
{
	UART_GenerateTransmissionFrame(deviceNetworkID,
								   reportEndpoint,
								   UART_ReportValueTemperature,
								   UART_TypeReport,
								   &valueTemp,
								   1);
}

/*
 * @func UART_FRAMEreportHumidityValue
 * @brief To create the UART transmission frame to report humidity value to ESP32
 * @param
 * 		  deviceNetworkID - The network address of device reports to ZC
 * 		  reportEndpoint - The endpoint of device reports to ZC
 * 		  valueHum - The humidity value reports to ZC
 * @retval None
 */
void UART_FRAMEreportHumidityValue(EmberNodeId deviceNetworkID,
								   RECEIVE_valueEndpoint reportEndpoint,
								   uint8_t valueHum)
{
	UART_GenerateTransmissionFrame(deviceNetworkID,
								   reportEndpoint,
								   UART_ReportValueHumidity,
								   UART_TypeReport,
								   &valueHum,
								   1);
}

/*
 * @func UART_FRAMEreportBrightnessValue
 * @brief To create the UART transmission frame to report brightness value to ESP32
 * @param
 * 		  deviceNetworkID - The network address of device reports to ZC
 * 		  reportEndpoint - The endpoint of device reports to ZC
 * 		  valueBrightness - The brightness value reports to ZC
 * @retval None
 */
void UART_FRAMEreportBrightnessValue(EmberNodeId deviceNetworkID,
		 	 	 	 	 	 	 	 RECEIVE_valueEndpoint reportEndpoint,
									 uint8_t* valueBrightness)
{
	UART_GenerateTransmissionFrame(deviceNetworkID,
								   reportEndpoint,
								   UART_ReportValueBrightness,
								   UART_TypeReport,
								   valueBrightness,
								   2);
}

/*
 * @func UART_FRAMEreportControlMode
 * @brief To create the UART transmission frame to report control mode to ESP32
 * @param
 * 		  deviceNetworkID - The network address of device reports to ZC
 * 		  reportEndpoint - The endpoint of device reports to ZC
 * 		  controlMode - The control mode value reports to ZC
 * @retval None
 */
void UART_FRAMEreportControlMode(EmberNodeId deviceNetworkID,
								 RECEIVE_valueEndpoint reportEndpoint,
								 uint8_t controlMode)
{
	UART_GenerateTransmissionFrame(deviceNetworkID,
								   reportEndpoint,
								   UART_ControlMode,
								   UART_TypeReport,
								   &controlMode,
								   1);
}

/*
 * @func UART_FRAMEreportOnOffLight
 * @brief To create the UART transmission frame to report state of light to ESP32
 * @param
 * 		  deviceNetworkID - The network address of device reports to ZC
 * 		  reportEndpoint - The endpoint of device reports to ZC
 * 		  stateStairLight - The state of light reports to ZC
 * @retval None
 */
void UART_FRAMEreportOnOffLight(EmberNodeId deviceNetworkID,
								RECEIVE_valueEndpoint reportEndpoint,
								uint8_t stateStairLight)
{
	UART_GenerateTransmissionFrame(deviceNetworkID,
								   reportEndpoint,
								   UART_TurnOnOffLight,
								   UART_TypeReport,
								   &stateStairLight,
								   1);
}

			/*---------- All functions to receive data via UART ----------*/

/*
 * @func UART_readDataUSART2EventHandler
 * @brief To read data, push it into the queue
 * @param None
 * @retval None
 */
void UART_readDataUSART2EventHandler(void)
{
		// Inactivate this event
	emberEventControlSetInactive(UART_readDataUSART2EventControl);

		// Get & process received data
	lengthReceptionPayload = emberSerialReadAvailable(comPortUsart2);
	if(lengthReceptionPayload != 0){
		emberSerialReadData(comPortUsart2, receptionPayload, lengthReceptionPayload, NULL);
		for(uint8_t i = 0; i < lengthReceptionPayload; i++){
			QUEUE_pushDataIntoDataBuffer(pQueue, receptionPayload + i);
		}
		UART_handleInstancesReceivedDataFromESP32();
	}

		// To perform this event every 100 ms
	emberEventControlSetDelayMS(UART_readDataUSART2EventControl, CYCLE_READING_DATA);
}

/*
 * @func UART_handleInstancesReceivedDataFromESP32
 * @brief To handle the instances of received data from ESP32
 * @param None
 * @retval None
 */
void UART_handleInstancesReceivedDataFromESP32(void)
{
		// Variable to get the state of received data
	UART_stateReceivedData_e receivedDataState;

		// Get the state of received data
	receivedDataState = UART_handleDataInQueue();

		// Process appropriately with the state of received data
	switch(receivedDataState){
		case UART_NoReceivedData:
			emberAfCorePrintln("UART_NoReceivedData !");
			break;

		case UART_ValidReceivedData:
			handleReceivedDataFromESP32(&dataSendByESP32[1]);
			break;

		case UART_InvalidReceivedData:
			emberAfCorePrintln("UART_InvalidReceivedData !");
			break;

		default:
			break;
	}
}

/*
 * @func UART_handleDataInQueue
 * @brief To get data from the queue buffer & process it
 * @param None
 * @retval The state of received data
 */
UART_stateReceivedData_e UART_handleDataInQueue(void)
{
		// Variable
	uint8_t dataQueue;															// To get data from queue buffer
	UART_stateReceivedData_e stateReceivedData = UART_NoReceivedData;			// The state of received data

		// Get data from queue buffer & process it
	while(QUEUE_IsDataBufferEmpty(pQueue) != true){
		QUEUE_popDataFromDataBuffer(pQueue, &dataQueue);
		switch(typeReceivedByte){
			case UART_ByteStart:
				if(dataQueue == BYTE_START){
					indexArray = 0;
					calculateXORbyte = BYTE_CXOR;
					typeReceivedByte = UART_BytesData;
				} else{
					stateReceivedData = UART_InvalidReceivedData;
				}
				break;

			case UART_BytesData:
				dataSendByESP32[indexArray] = dataQueue;
				if(indexArray > 0){
					calculateXORbyte ^= dataQueue;
				}
				if(++indexArray == *dataSendByESP32){
					typeReceivedByte = UART_ByteCXOR;
				}
				break;

			case UART_ByteCXOR:
				if(dataQueue == calculateXORbyte){
					stateReceivedData = UART_ValidReceivedData;
				} else{
					stateReceivedData = UART_InvalidReceivedData;
				}
				typeReceivedByte = UART_ByteStart;
				break;

			default:
				typeReceivedByte = UART_ByteStart;
				break;
		}
	}

	return stateReceivedData;
}

	/*------------------------------- END THIS FILE -------------------------------*/
