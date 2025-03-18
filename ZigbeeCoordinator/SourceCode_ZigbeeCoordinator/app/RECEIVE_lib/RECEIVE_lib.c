/*
 * RECEIVE_lib.c
 *
 *  Created on: June 03 , 2024
 *  Author: TRAN DOAN MANH
 *  Description: To receive & handle reports from devices in Zigbee network
 */

	/*------------------------------- All libraries -------------------------------*/

#include "app/framework/include/af.h"
#include "source/app/RECEIVE_lib/RECEIVE_lib.h"

	/*------------------------------- Initialize function pointer -------------------------------*/

	// Function pointer
RECEIVE_handleReportsFromDevicesZigbeeNetwork handleReportsFromDevicesZigbeeNetwork = NULL;

	/*------------------------------- All functions -------------------------------*/

/*
 * @func RECEIVE_initializeReceiveData
 * @brief To register a callback to handle all data that receive from all devices in Zigbee network
 * @param handleReportsFromDevicesZigbeeNetworkCallback - The callback function will be called to handle received data from ZR device
 * @retval None
 */
void RECEIVE_initializeReceiveData(RECEIVE_handleReportsFromDevicesZigbeeNetwork handleReportsFromDevicesZigbeeNetworkCallback)
{
	if(handleReportsFromDevicesZigbeeNetworkCallback != NULL){
		handleReportsFromDevicesZigbeeNetwork = handleReportsFromDevicesZigbeeNetworkCallback;
	}
}

/*
 * @func emberAfPreZDOMessageReceivedCallback
 * @brief To receive notice leaving the network
 * @param
 * 		 deviceNetworkID - The networkID of report device
 * 		 infReportCommand - The pointer variable of the structure type "EmberApsFrame"
 * 							to get information of report message
 * 		 contentReport - The content of report
 * 		 lengthContentReport - The length of content report
 * @retval True/False
 */
bool emberAfPreZDOMessageReceivedCallback(EmberNodeId deviceNetworkID,
                                          EmberApsFrame* infReportCommand,
                                          uint8_t* contentReport,
                                          uint16_t lengthContentReport)
{
	if(infReportCommand->clusterId == LEAVE_RESPONSE){
		emberAfCorePrintln("handleReportsFromDevicesZigbeeNetwork");
		handleReportsFromDevicesZigbeeNetwork(deviceNetworkID,
											  infReportCommand->sourceEndpoint,
											  RECEIVE_DeviceLeaveNetwork,
											  NULL);
	}

	return false;
}

/*
 * @func emberAfPreCommandReceivedCallback
 * @brief To handle ZCL command
 * @param infReportCommand - The pointer variable of the structure type "EmberAfClusterCommand"
 * 							 to get information of incoming message
 * @retval True/False
 */
bool emberAfPreCommandReceivedCallback(EmberAfClusterCommand* infReportCommand)
{
	if(infReportCommand->clusterSpecific == 0){
		switch(infReportCommand->apsFrame->clusterId){
			case ZCL_BASIC_CLUSTER_ID:
				RECEIVE_handleBasicCluster(infReportCommand);
				break;

			case ZCL_ON_OFF_CLUSTER_ID:
				if(infReportCommand->commandId == ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID){ // Remove default response messenger
					RECEIVE_handleOnOffCluster(infReportCommand);
				}
				break;

			case ZCL_TEMP_MEASUREMENT_CLUSTER_ID:
				RECEIVE_handleTemperatureMeasurementCluster(infReportCommand);
				break;

			case ZCL_RELATIVE_HUMIDITY_MEASUREMENT_CLUSTER_ID:
				RECEIVE_handleRelativeHumidityMeasurementCluster(infReportCommand);
				break;

			case ZCL_ILLUM_MEASUREMENT_CLUSTER_ID:
				RECEIVE_handleIlluminanceMeasurementCluster(infReportCommand);
				break;

			default:
				break;
		}
	}

	return false;
}

/*
 * @func RECEIVE_handleBasicCluster
 * @brief To get the modelID value and report that value to Web
 * @param infReportCommand - The pointer variable of the structure type "EmberAfClusterCommand"
 * 							 to get information of incoming message
 * @retval None
 */
void RECEIVE_handleBasicCluster(EmberAfClusterCommand* infReportCommand)
{
		// Variables
	uint8_t* modelID;					    // Get the modelID of switch
	static char modelIdDevice[10];			// Store the modelID of device

		// Get the modelID of switch and report for Web
	modelID = emberAfGetString(infReportCommand->buffer,
							   (infReportCommand->payloadStartIndex) + 4,
							   infReportCommand->bufLen);
	memset(modelIdDevice, '\0', 10);
	memcpy(modelIdDevice, &modelID[1], modelID[0]);
	handleReportsFromDevicesZigbeeNetwork(infReportCommand->source,
										  infReportCommand->apsFrame->sourceEndpoint,
										  RECEIVE_DeviceJoinNetwork,
										  (uint8_t *)modelIdDevice);
}

/*
 * @func RECEIVE_handleOnOffCluster
 * @brief To get the On/Off value of stair light is reported from ZR device and report that value to Web
 * @param infReportCommand - The pointer variable of the structure type "EmberAfClusterCommand"
 * 							 to get information of incoming message
 * @retval None
 */
void RECEIVE_handleOnOffCluster(EmberAfClusterCommand* infReportCommand)
{
		// Variables
	static uint8_t stateLight;			// Get the state of stair light
	static uint8_t controlMode;			// Get the control mode

		// Get the state of stair light, control mode and report for Web
	if(infReportCommand->apsFrame->sourceEndpoint == 1){
		stateLight = emberAfGetInt8u(infReportCommand->buffer,
									 (infReportCommand->payloadStartIndex) + 4,
									 infReportCommand->bufLen);
		handleReportsFromDevicesZigbeeNetwork(infReportCommand->source,
											  infReportCommand->apsFrame->sourceEndpoint,
											  RECEIVE_ReportOnOffLight,
											  &stateLight);
	} else if(infReportCommand->apsFrame->sourceEndpoint == 5){
		controlMode = emberAfGetInt8u(infReportCommand->buffer,
									  (infReportCommand->payloadStartIndex) + 4,
									  infReportCommand->bufLen);
		handleReportsFromDevicesZigbeeNetwork(infReportCommand->source,
											  infReportCommand->apsFrame->sourceEndpoint,
											  RECEIVE_ReportControlMode,
											  &controlMode);
	}
}

/*
 * @func RECEIVE_handleTemperatureMeasurementCluster
 * @brief To get the temperature value is reported from ZR device and report that value to Web
 * @param infReportCommand - The pointer variable of the structure type "EmberAfClusterCommand"
 * 							 to get information of incoming message
 * @retval None
 */
void RECEIVE_handleTemperatureMeasurementCluster(EmberAfClusterCommand* infReportCommand)
{
		// Variable to get et the temperature value
	static uint8_t valueTemp;

		// Get the temperature value and report for Web
	valueTemp = emberAfGetInt8u(infReportCommand->buffer,
								(infReportCommand->payloadStartIndex) + 4,
								infReportCommand->bufLen);
	handleReportsFromDevicesZigbeeNetwork(infReportCommand->source,
										  infReportCommand->apsFrame->sourceEndpoint,
										  RECEIVE_ReportValueTemp,
										  &valueTemp);
}

/*
 * @func RECEIVE_handleRelativeHumidityMeasurementCluster
 * @brief To get the relative humidity value is reported from ZR device and report that value to Web
 * @param infReportCommand - The pointer variable of the structure type "EmberAfClusterCommand"
 * 							 to get information of incoming message
 * @retval None
 */
void RECEIVE_handleRelativeHumidityMeasurementCluster(EmberAfClusterCommand* infReportCommand)
{
		// Variable to get the relative humidity value
	static uint8_t valueHum;

		// Get the relative humidity value and report for Web
	valueHum = emberAfGetInt8u(infReportCommand->buffer,
							   (infReportCommand->payloadStartIndex) + 4,
							   infReportCommand->bufLen);
	handleReportsFromDevicesZigbeeNetwork(infReportCommand->source,
										  infReportCommand->apsFrame->sourceEndpoint,
										  RECEIVE_ReportValueHum,
										  &valueHum);
}

/*
 * @func RECEIVE_handleIlluminanceMeasurementCluster
 * @brief To get the light intensity value is reported from ZR device and report that value to Web
 * @param infReportCommand - The pointer variable of the struct type "EmberAfClusterCommand"
 * 							 to get information of incoming message
 * @retval None
 */
void RECEIVE_handleIlluminanceMeasurementCluster(EmberAfClusterCommand* infReportCommand)
{
		// Variable to get the brightness value
	static uint16_t valueBrightness;

		// Get the light intensity value and report for Web
	valueBrightness = emberAfGetInt16u(infReportCommand->buffer,
									   (infReportCommand->payloadStartIndex) + 4,
									   infReportCommand->bufLen);
	handleReportsFromDevicesZigbeeNetwork(infReportCommand->source,
										  infReportCommand->apsFrame->sourceEndpoint,
										  RECEIVE_ReportValueBrightness,
										  (uint8_t *)&valueBrightness);
}

	/*------------------------------- END THIS FILE -------------------------------*/

