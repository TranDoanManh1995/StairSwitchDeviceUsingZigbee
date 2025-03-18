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
 * @func TRANSMIT_reportModelIDUpperSWForZC
 * @brief To report the modelID of device for ZC
 * @param reportEndpoint - The endpoint of device to send the report for ZC
 * @retval None
 */
void TRANSMIT_reportModelIDUpperSWForZC(MAIN_endpoints_e reportEndpoint)
{
		// Initialize the modelID of device
	uint8_t modelIDUpperSW[3] = {2, 'U', 'P'};

		// Check whether this device joins the Zigbee network
	if(emberAfNetworkState() != EMBER_JOINED_NETWORK){
		return;
	}

		// Report the modelID of device for ZC
	TRANSMIT_fillReportCommandIntoBuffer(ZCL_BASIC_CLUSTER_ID,
								 	   	 ZCL_MODEL_IDENTIFIER_ATTRIBUTE_ID,
										 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
										 modelIDUpperSW,
										 3,
										 ZCL_CHAR_STRING_ATTRIBUTE_TYPE);
	TRANSMIT_sendReportCommandByUnicast(reportEndpoint, ENDPOINT_ZC, NETWORKID_ZC);
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
		// Payload to transmit for ZC
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
 * 		  endpointToReport - The endpoint of device to send the report for ZC
 * 		  endpointToReceive - The endpoint of ZC to receive the report
 * 		  addressReceivingDevice - The networkID of ZC
 * @retval None
 */
void TRANSMIT_sendReportCommandByUnicast(MAIN_endpoints_e endpointToReport,
							 	 	 	 uint8_t endpointToReceive,
										 uint8_t addressReceivingDevice)
{
		// Set up source and destination endpoint
	emberAfSetCommandEndpoints(endpointToReport, endpointToReceive);

		// Send command in Unicast
	(void) emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, addressReceivingDevice);
}

	/*------------------------------- END THIS FILE -------------------------------*/

