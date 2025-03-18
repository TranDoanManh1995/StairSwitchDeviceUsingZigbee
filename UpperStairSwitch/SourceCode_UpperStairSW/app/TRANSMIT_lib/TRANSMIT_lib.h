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

	/*------------------------------- Declare function prototypes -------------------------------*/

	// Macros
#define NETWORKID_ZC			0x0000			// The networkID of Zigbee Coordinator
#define ENDPOINT_ZC				0x01			// The endpoint value of Zigbee Coordinator

	// Function prototypes
void TRANSMIT_reportModelIDUpperSWForZC(MAIN_endpoints_e reportEndpoint);
void TRANSMIT_fillReportCommandIntoBuffer(EmberAfClusterId clusterID,
		 	 	 	 	 	 	  	      EmberAfAttributeId attributeID,
									      uint8_t globalCommandID,
										  uint8_t *reportData,
										  uint8_t lengthReportData,
										  uint8_t typeReportData);
void TRANSMIT_sendReportCommandByUnicast(MAIN_endpoints_e endpointToReport,
							 	 	 	 uint8_t endpointToReceive,
										 uint8_t addressReceivingDevice);

	/*------------------------------- END THIS FILE -------------------------------*/

#endif /* SOURCE_APP_TRANSMIT_LIB_TRANSMIT_LIB_H_ */
