/*
 * RECEIVE_lib.h
 *
 *  Created on: Mar 17, 2024
 *      Author: TRAN DOAN MANH
 */

#ifndef SOURCE_APP_RECEIVE_LIB_RECEIVE_LIB_H_
#define SOURCE_APP_RECEIVE_LIB_RECEIVE_LIB_H_

	/*------------------------------- All libraries -------------------------------*/

#include "app/framework/include/af.h"

	/*------------------------------- Declare macros, variables & function prototypes -------------------------------*/

	// Variables
boolean bindingControlStairLight;			// Check whether this binding process is to control the stair light
boolean bindingSynchronizePIRtime;			// Check whether this binding process is synchronize the PIR time

	// Function prototypes
void RECEIVE_processOnOffCluster(EmberAfClusterCommand* commandInf);
void RECEIVE_processIASZoneCluster(EmberAfClusterCommand* commandInf);
void RECEIVE_processIdentifyCluster(EmberAfClusterCommand* commandInf);

	/*------------------------------- END THIS FILE -------------------------------*/

#endif /* SOURCE_APP_RECEIVE_LIB_RECEIVE_LIB_H_ */
