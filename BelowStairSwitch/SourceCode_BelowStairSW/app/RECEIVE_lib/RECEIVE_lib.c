/*
 * RECEIVE_lib.c
 *
 *  Created on: June 03 , 2024
 *  Author: TRAN DOAN MANH
 *  Descriptor: To process the messages that receive from ZC & Upper stair
 */

	/*------------------------------- All libraries -------------------------------*/

#include "app/framework/include/af.h"
#include "source/app/main/main.h"
#include "source/app/RECEIVE_lib/RECEIVE_lib.h"
#include "source/driver/PIR_lib/PIR_lib.h"
#include "source/mid/LED_lib/LED_lib.h"

	/*------------------------------- Initialize variables -------------------------------*/



	/*------------------------------- All functions -------------------------------*/

/*
 * @func emberAfPluginFindAndBindInitiatorCompleteCallback
 * @brief To check whether the binding process is successful
 * @param statusBindingProcess - The status of binding process
 * @retval None
 */
void emberAfPluginFindAndBindInitiatorCompleteCallback(EmberStatus statusBindingProcess)
{
	if(statusBindingProcess == EMBER_SUCCESS){
		if(bindingControlStairLight == true){
			LED_blinkLed(LED_2, LED_Green, 3, 300, 300);
			bindingControlStairLight = false;
		} else if(bindingSynchronizePIRtime == true){
			LED_blinkLed(LED_2, LED_Blue, 3, 300, 300);
			bindingSynchronizePIRtime = false;
		}
	}
}

/*
 * @func emberAfPreCommandReceivedCallback
 * @brief To process ZCL command
 * @param commandInf - Pointer variable of the struct type "EmberAfClusterCommand"
 * 					   to get information of incoming message
 * @retval True/False
 */
bool emberAfPreCommandReceivedCallback(EmberAfClusterCommand* commandInf)
{
	if(commandInf->clusterSpecific){
		switch(commandInf->apsFrame->clusterId){
			case ZCL_ON_OFF_CLUSTER_ID:
				RECEIVE_processOnOffCluster(commandInf);
				break;

			case ZCL_IAS_ZONE_CLUSTER_ID:
				RECEIVE_processIASZoneCluster(commandInf);
				break;

			case ZCL_IDENTIFY_CLUSTER_ID:
				RECEIVE_processIdentifyCluster(commandInf);
				break;

			default:
				break;
		}
	}

	return false;
}

/*
 * @func RECEIVE_processOnOffCluster
 * @brief To control the stair light and select the control mode
 * @param commandInf - Pointer variable of the struct type "EmberAfClusterCommand"
 * 					   to get information of incoming message
 * @retval None
 */
void RECEIVE_processOnOffCluster(EmberAfClusterCommand* commandInf)
{
	switch(commandInf->commandId){
		case ZCL_ON_COMMAND_ID:
			if(commandInf->apsFrame->sourceEndpoint == 1){ // That endpoint could belong to UpperSW or Zigbee Coordinator
				if(commandInf->apsFrame->destinationEndpoint == MAIN_EndpointPIRAndControlMode){
					MAIN_selectControlMode(MAIN_EndpointPIRAndControlMode, MAIN_ModeAuto, LED_Blue, PIR_EnableInterrupt);
					MAIN_synchronizeStateStairLight_ControlMode_PIRtimeWithUpperSW(MAIN_networkIdBelowSW,
																				   MAIN_EndpointPIRAndControlMode,
																				   PIR_CONTROLMODE_ENDPOINT_UPPERSW,
																				   ZCL_ON_OFF_CLUSTER_ID,
																				   ZCL_ON_COMMAND_ID);
				} else if(commandInf->apsFrame->destinationEndpoint == MAIN_EndpointStairLight){
					MAIN_turnOnOffStairLight(MAIN_LightON, MAIN_EndpointStairLight, MAIN_networkIdBelowSW, ZCL_ON_COMMAND_ID);
				}
			} else if(commandInf->apsFrame->sourceEndpoint == PIR_CONTROLMODE_ENDPOINT_UPPERSW){
				MAIN_selectControlMode(MAIN_EndpointPIRAndControlMode, MAIN_ModeAuto, LED_Blue, PIR_EnableInterrupt);
			}
			break;

		case ZCL_OFF_COMMAND_ID:
			if(commandInf->apsFrame->sourceEndpoint == 1){ // That endpoint could belong to UpperSW of Zigbee Coordinator
				if(commandInf->apsFrame->destinationEndpoint == MAIN_EndpointPIRAndControlMode){
					MAIN_selectControlMode(MAIN_EndpointPIRAndControlMode, MAIN_ModeManual, LED_Red, PIR_DisableInterrupt);
					MAIN_synchronizeStateStairLight_ControlMode_PIRtimeWithUpperSW(MAIN_networkIdBelowSW,
																				   MAIN_EndpointPIRAndControlMode,
																				   PIR_CONTROLMODE_ENDPOINT_UPPERSW,
																				   ZCL_ON_OFF_CLUSTER_ID,
																				   ZCL_OFF_COMMAND_ID);
				} else if(commandInf->apsFrame->destinationEndpoint == MAIN_EndpointStairLight){
					MAIN_turnOnOffStairLight(MAIN_LightOFF, MAIN_EndpointStairLight, MAIN_networkIdBelowSW, ZCL_OFF_COMMAND_ID);
				}
			} else if(commandInf->apsFrame->sourceEndpoint == PIR_CONTROLMODE_ENDPOINT_UPPERSW){
				MAIN_selectControlMode(MAIN_EndpointPIRAndControlMode, MAIN_ModeManual, LED_Red, PIR_DisableInterrupt);
			}
			break;

		default:
			break;
	}
}

/*
 * @func RECEIVE_processIASZoneCluster
 * @brief To get the motion signal from UpperSW and synchronize
 * @param commandInf - Pointer variable of the struct type "EmberAfClusterCommand"
 * 					   to get information of incoming message
 * @retval None
 */
void RECEIVE_processIASZoneCluster(EmberAfClusterCommand* commandInf)
{
	switch(commandInf->commandId){
		case ZCL_ZONE_ENROLL_RESPONSE_COMMAND_ID:
			PIR_stateFindingMotion = PIR_StateWait2s;
			emberEventControlSetActive(PIR_detectMotionEventControl);
			break;

		default:
			break;
	}
}

/*
 * @func RECEIVE_processIdentifyCluster
 * @brief To define the binding process of controlling the stair light or synchronizing the PIR time on two stair SWs
 * @param commandInf - Pointer variable of the struct type "EmberAfClusterCommand"
 * 					   to get information of incoming message
 * @retval None
 */
void RECEIVE_processIdentifyCluster(EmberAfClusterCommand* commandInf)
{
	if(commandInf->source != MAIN_networkIdBelowSW){
		if(commandInf->apsFrame->sourceEndpoint == STAIRLIGHT_ENDPOINT_UPPERSW
				&& commandInf->apsFrame->destinationEndpoint == MAIN_EndpointStairLight)
		{
			bindingControlStairLight = true;
		} else if(commandInf->apsFrame->sourceEndpoint == PIR_CONTROLMODE_ENDPOINT_UPPERSW
				&& commandInf->apsFrame->destinationEndpoint == MAIN_EndpointPIRAndControlMode)
		{
			bindingSynchronizePIRtime = true;
		}
	}
}

	/*------------------------------- END THIS FILE -------------------------------*/
