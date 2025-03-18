/*
 * TRANSMIT_lib.c
 *
 *  Created on: Mar 17, 2024
 *  Author: TRAN DOAN MANH
 *  Descriptor: To report for ZC and communicate with Upper stair
 */

	/*------------------------------- All libraries -------------------------------*/

#include "app/framework/include/af.h"
#include "source/app/TRANSMIT_lib/TRANSMIT_lib.h"

	/*------------------------------- Initialize variables -------------------------------*/



	/*------------------------------- All functions -------------------------------*/

/*
 * @func TRANSMIT_reportModelIDBelowSWForZC
 * @brief To report the modelID of device for ZC
 * @param valueReportEndpoint - The endpoint of device to send the report for ZC
 * @retval None
 */
void TRANSMIT_reportModelIDBelowSWForZC(MAIN_endpoints_e valueReportEndpoint)
{
		// Initialize the modelID of device
	uint8_t modelIDBelowSW[5] = {4, 'D', 'O', 'W', 'N'};

		// Check whether this device joins the Zigbee network
	if(emberAfNetworkState() != EMBER_JOINED_NETWORK){
		return;
	}

		// Report the modelID of device for ZC
	TRANSMIT_fillReportCommandIntoBuffer(ZCL_BASIC_CLUSTER_ID,
								 	   	 ZCL_MODEL_IDENTIFIER_ATTRIBUTE_ID,
										 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
										 modelIDBelowSW,
										 5,
										 ZCL_CHAR_STRING_ATTRIBUTE_TYPE);
	TRANSMIT_sendReportCommandByUnicast(valueReportEndpoint, ENDPOINT_ZC, NETWORKID_ZC);
}

/*
 * @func TRANSMIT_reportStateStairLightForZC
 * @brief To report the state of stair light for HC
 * @param
 * 		  reportEndpoint - The endpoint of device to send the report for ZC
 * 		  stateStairLight - The state of stair light
 * @retval None
 */
void TRANSMIT_reportStateStairLightForZC(MAIN_endpoints_e reportEndpoint, uint8_t stateStairLight)
{
	TRANSMIT_fillReportCommandIntoBuffer(ZCL_ON_OFF_CLUSTER_ID,
								 	 	 ZCL_ON_OFF_ATTRIBUTE_ID,
										 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
										 (uint8_t *)&stateStairLight,
										 1,
										 ZCL_BOOLEAN_ATTRIBUTE_TYPE);
	TRANSMIT_sendReportCommandByUnicast(reportEndpoint, ENDPOINT_ZC, NETWORKID_ZC);
}

/*
 * @func TRANSMIT_reportControlModeForZC
 * @brief To report the control mode for HC
 * @param
 * 		  valueReportEndpoint - The endpoint of device to send the report for ZC
 * 		  controlMode - The control mode "AUTO" or "MANUAL"
 * @retval None
 */
void TRANSMIT_reportControlModeForZC(MAIN_endpoints_e valueReportEndpoint, MAIN_controlMode_e controlMode)
{
	TRANSMIT_fillReportCommandIntoBuffer(ZCL_ON_OFF_CLUSTER_ID,
								 	 	 ZCL_ON_OFF_ATTRIBUTE_ID,
										 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
										 (uint8_t *)&controlMode,
										 1,
										 ZCL_BOOLEAN_ATTRIBUTE_TYPE);
	TRANSMIT_sendReportCommandByUnicast(valueReportEndpoint, ENDPOINT_ZC, NETWORKID_ZC);
}

/*
 * @func TRANSMIT_reportBrightnessValueForZC
 * @brief To report the brightness value for HC
 * @param
 * 		  valueReportEndpoint - The endpoint of device to send the report for ZC
 * 		  brightnessValue - The value of brightness
 * @retval None
 */
void TRANSMIT_reportBrightnessValueForZC(MAIN_endpoints_e valueReportEndpoint, uint8_t brightnessValue)
{
	TRANSMIT_fillReportCommandIntoBuffer(ZCL_ILLUM_MEASUREMENT_CLUSTER_ID,
								         ZCL_ILLUM_MEASURED_VALUE_ATTRIBUTE_ID,
										 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
										 (uint8_t *)&brightnessValue,
										 2,
										 ZCL_INT16U_ATTRIBUTE_TYPE);
	TRANSMIT_sendReportCommandByUnicast(valueReportEndpoint, ENDPOINT_ZC, NETWORKID_ZC);
}

/*
 * @func TRANSMIT_reportTemperatureValueForZC
 * @brief To report the temperature value for HC
 * @param
 * 		  valueReportEndpoint - The endpoint of device to send the report for ZC
 * 		  tempValue - The value of temperature
 * @retval None
 */
void TRANSMIT_reportTemperatureValueForZC(MAIN_endpoints_e valueReportEndpoint, uint8_t tempValue)
{
	TRANSMIT_fillReportCommandIntoBuffer(ZCL_TEMP_MEASUREMENT_CLUSTER_ID,
								 	 	 ZCL_TEMP_MEASURED_VALUE_ATTRIBUTE_ID,
										 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
										 (uint8_t *)&tempValue,
										 1,
										 ZCL_INT8U_ATTRIBUTE_TYPE);
	TRANSMIT_sendReportCommandByUnicast(valueReportEndpoint, ENDPOINT_ZC, NETWORKID_ZC);
}

/*
 * @func SEND_HumValueReport
 * @brief To send the humidity value for ZC
 * @param
 * 		  valueReportEndpoint - The endpoint of device to send the report for ZC
 * 		  humValue - The value of humidity
 * @retval None
 */
void TRANSMIT_reportHumidityValueForZC(MAIN_endpoints_e valueReportEndpoint, uint8_t humValue)
{
	TRANSMIT_fillReportCommandIntoBuffer(ZCL_RELATIVE_HUMIDITY_MEASUREMENT_CLUSTER_ID,
								 	 	 ZCL_RELATIVE_HUMIDITY_MEASURED_VALUE_ATTRIBUTE_ID,
										 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
										 (uint8_t *)&humValue,
										 1,
										 ZCL_INT8U_ATTRIBUTE_TYPE);
	TRANSMIT_sendReportCommandByUnicast(valueReportEndpoint, ENDPOINT_ZC, NETWORKID_ZC);
}

/*
 * @func TRANSMIT_fillReportCommandIntoBuffer
 * @brief To fill the report command into buffer
 * @param
 * 		  clusterID - The identifier of cluster to send command
 * 		  attributeID - The identifier of attribute to send command
 * 		  globalCommandID - The identifier of global command to send command
 * 		  reportData - Data to report for ZC
 * 		  lengthReportData - The length of that data
 * 		  typeReportData - The type of that data
 * @retval None
 */
void TRANSMIT_fillReportCommandIntoBuffer(EmberAfClusterId clusterID,
		 	 	 	 	 	 	  	      EmberAfAttributeId attributeID,
									      uint8_t globalCommandID,
										  uint8_t *reportData,
										  uint8_t lengthReportData,
										  uint8_t typeReportData)
{
		// Payload to report for ZC
	uint8_t payloadToReport[20];

		// Set up payload to transmit for ZC
	payloadToReport[0] = (uint8_t)(attributeID & 0x00FF);
	payloadToReport[1] = (uint8_t)((attributeID & 0xFF00) >> 8);
	payloadToReport[2] = EMBER_SUCCESS;
	payloadToReport[3] = (uint8_t)typeReportData;
	memcpy(&payloadToReport[4], reportData, lengthReportData);

		// Fill the upper payload into buffer
	(void) emberAfFillExternalBuffer((ZCL_GLOBAL_COMMAND | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT | ZCL_DISABLE_DEFAULT_RESPONSE_MASK),
									  clusterID,
									  globalCommandID,
									  "b",
									  payloadToReport,
									  lengthReportData + 4);
}

/*
 * @func TRANSMIT_sendReportCommandByUnicast
 * @brief To send the report command for ZC by Unicast method
 * @param
 * 		  reportEndpoint - The endpoint of device to send the report for ZC
 * 		  receptionEndpoint - The endpoint of ZC to receive the report
 * 		  addressReceivingDevice - The networkID of ZC
 * @retval None
 */
void TRANSMIT_sendReportCommandByUnicast(MAIN_endpoints_e reportEndpoint,
							 	 	 	 uint8_t receptionEndpoint,
										 uint8_t addressReceivingDevice)
{
		// Set up source and destination endpoint
	emberAfSetCommandEndpoints(reportEndpoint, receptionEndpoint);

		// Send command in Unicast
	(void) emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, addressReceivingDevice);
}

	/*------------------------------- END THIS FILE -------------------------------*/

