/*
 * TRANSMIT_lib.h
 *
 *  Created on: Mar 17, 2024
 *      Author: TRAN DOAN MANH
 */

#ifndef SOURCE_APP_TRANSMIT_LIB_TRANSMIT_LIB_H_
#define SOURCE_APP_TRANSMIT_LIB_TRANSMIT_LIB_H_

	/*------------------------------- All libraries -------------------------------*/

#include "app/framework/include/af.h"
#include "source/app/main/main.h"

	/*------------------------------- Declare macros, function prototypes -------------------------------*/

	// Macros
#define NETWORKID_ZC			0x0000			// The networkID of Zigbee Coordinator
#define ENDPOINT_ZC				0x01			// The endpoint value of Zigbee Coordinator

	// Function prototypes
void TRANSMIT_reportModelIDBelowSWForZC(MAIN_endpoints_e valueReportEndpoint);
void TRANSMIT_reportStateStairLightForZC(MAIN_endpoints_e reportEndpoint, uint8_t stateStairLight);
void TRANSMIT_reportControlModeForZC(MAIN_endpoints_e valueReportEndpoint, MAIN_controlMode_e controlMode);
void TRANSMIT_reportBrightnessValueForZC(MAIN_endpoints_e valueReportEndpoint, uint8_t valueBrightness);
void TRANSMIT_reportTemperatureValueForZC(MAIN_endpoints_e valueReportEndpoint, uint8_t valueTemp);
void TRANSMIT_reportHumidityValueForZC(MAIN_endpoints_e valueReportEndpoint, uint8_t valueHum);
void TRANSMIT_fillReportCommandIntoBuffer(EmberAfClusterId clusterID,
		 	 	 	 	 	 	  	      EmberAfAttributeId attributeID,
									      uint8_t globalCommandID,
										  uint8_t *reportData,
										  uint8_t lengthReportData,
										  uint8_t typeReportData);
void TRANSMIT_sendReportCommandByUnicast(MAIN_endpoints_e reportEndpoint,
							 	 	 	 uint8_t receptionEndpoint,
										 uint8_t addressReceivingDevice);

	/*------------------------------- END THIS FILE -------------------------------*/

#endif /* SOURCE_APP_TRANSMIT_LIB_TRANSMIT_LIB_H_ */
