/*
 * RECEIVE_lib.h
 *
 *  Created on: June 03 , 2024
 *      Author: TRAN DOAN MANH
 */

#ifndef SOURCE_APP_RECEIVE_LIB_RECEIVE_LIB_H_
#define SOURCE_APP_RECEIVE_LIB_RECEIVE_LIB_H_

	/*------------------------------- All libraries -------------------------------*/

#include "app/framework/include/af.h"

	/*------------------------------- Declare enum & new data types, function prototypes -------------------------------*/

	// Enum types
typedef enum { // Define the types of report message
	RECEIVE_DeviceZigbeeCoordinator,
	RECEIVE_DeviceJoinNetwork,
	RECEIVE_DeviceLeaveNetwork,
	RECEIVE_ReportValueTemp,
	RECEIVE_ReportValueHum,
	RECEIVE_ReportValueBrightness,
	RECEIVE_ReportOnOffLight,
	RECEIVE_ReportControlMode
}RECEIVE_typeReportMessage_e;

	// New data types
typedef uint8_t RECEIVE_valueEndpoint;

	// Define a function pointer & create a function to call the processing callback
typedef void (*RECEIVE_handleReportsFromDevicesZigbeeNetwork)(EmberNodeId ,
														  	  RECEIVE_valueEndpoint ,
															  RECEIVE_typeReportMessage_e ,
															  uint8_t* );
void RECEIVE_initializeReceiveData(RECEIVE_handleReportsFromDevicesZigbeeNetwork );

	// Function prototypes
void RECEIVE_handleBasicCluster(EmberAfClusterCommand* infReportCommand);
void RECEIVE_handleOnOffCluster(EmberAfClusterCommand* infReportCommand);
void RECEIVE_handleTemperatureMeasurementCluster(EmberAfClusterCommand* infReportCommand);
void RECEIVE_handleRelativeHumidityMeasurementCluster(EmberAfClusterCommand* infReportCommand);
void RECEIVE_handleIlluminanceMeasurementCluster(EmberAfClusterCommand* infReportCommand);

	/*------------------------------- END THIS FILE -------------------------------*/

#endif /* SOURCE_APP_RECEIVE_LIB_RECEIVE_LIB_H_ */
