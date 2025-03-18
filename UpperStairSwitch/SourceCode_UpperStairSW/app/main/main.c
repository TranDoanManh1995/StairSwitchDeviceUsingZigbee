/*
 * main.c
 *
 *  Created on: May 20, 2024
 *  Author: TRAN DOAN MANH
 *  Descriptor: To main program
 */

	/*------------------------------- All libraries -------------------------------*/

#include "app/framework/include/af.h"
#include "app/framework/plugin/find-and-bind-target/find-and-bind-target.h"
#include "app/framework/plugin/find-and-bind-initiator/find-and-bind-initiator.h"
#include "source/app/main/main.h"
#include "source/app/RECEIVE_lib/RECEIVE_lib.h"
#include "source/app/TRANSMIT_lib/TRANSMIT_lib.h"

	/*------------------------------- Initialize variables -------------------------------*/

	// Variables
boolean jointedZigbeeNetwork = false;													// To define whether this device joined the Zigbee network
boolean upperSWreboot = false;																// To only reboot once when leaving the Zigbee network
MAIN_deviceTypeInBinding_e activateInitiatorTargetForSWs = MAIN_TargetType;				// To select activating "Target" or "Initiator" to bind two buttons on two stair SWs that controls the stair light
MAIN_deviceTypeInBinding_e activateInitiatorTargetForPIRs = MAIN_TargetType;			// To select activating "Target" or "Initiator" to bind two PIRs sensor on two stair SWs that synchronizes time of PIRs
MAIN_stateStairLight_e MAIN_stateStairLight = MAIN_LightOFF;							// The state of stair light

	/*------------------------------- All functions -------------------------------*/

/*
 * @func emberAfMainInitCallback
 * @brief This function is called from the application's main function. It gives the
 * 		  application a chance to do any initialization required at system startup.
 * @param None
 * @retval None
 */
void emberAfMainInitCallback(void)
{
		// Initialize all peripherals & callback
	LED_initializeLed();
	PIR_initializePIRsensor(MAIN_handleMotionSignal);
	BUTTON_initializeButton(MAIN_handleNumberButtonPresses);
	NETWORK_initializeZigbeeNetwork(MAIN_handleZigbeeNetworkStateUpperSW);

		// Disable interrupt of PIR sensor
	PIR_controlInterruptPIRsensor(PIR_DisableInterrupt);

		// Trigger event to process the states of below stair switch
	stateUpperSW = MAIN_StatePowerOn;
	emberEventControlSetActive(MAIN_stateUpperStairSWEventControl);
}

/*
 * @func MAIN_stateUpperStairSWEventHandler
 * @brief To process the states upper stair switch appropriately
 * @param None
 * @retval None
 */
void MAIN_stateUpperStairSWEventHandler(void)
{
		// Inactivate this event
	emberEventControlSetInactive(MAIN_stateUpperStairSWEventControl);

		// Get the network state of Upper Stair Switch
	EmberNetworkStatus networkStateUpperSW;

		// Process the states upper stair switch
	switch(stateUpperSW){
		case MAIN_StatePowerOn:
			networkStateUpperSW = emberAfNetworkState();
			if(networkStateUpperSW == EMBER_NO_NETWORK){
				LED_blinkLed(LED_2, LED_Red, 3, 300, 300);
				NETWORK_findJoinZigbeeNetwork();
			}
			stateUpperSW = MAIN_StateIdle;
			break;

		case MAIN_StateReport:
			TRANSMIT_reportModelIDUpperSWForZC(MAIN_EndpointReportDeviceInf);
			MAIN_networkIdUpperSW = emberAfGetNodeId();
			MAIN_selectControlMode(MAIN_ModeManual, LED_Red, PIR_DisableInterrupt);
			networkStateUpperSW = MAIN_StateIdle;
			break;

		case MAIN_StateReboot:
			networkStateUpperSW = emberAfNetworkState();
			if(networkStateUpperSW == EMBER_JOINED_NETWORK){
					// Report the Zigbee network & clear the binding table
				messageLeaveingNetwork[1] = 0x00;
				emberSendZigDevRequest(0x0000,
									   LEAVE_RESPONSE,
									   EMBER_AF_DEFAULT_APS_OPTIONS,
									   messageLeaveingNetwork,
									   sizeof(messageLeaveingNetwork));
				emberClearBindingTable();
					// Leave the Zigbee network
				emberLeaveNetwork();
			}
				// Reboot the upper stair switch
			if(upperSWreboot == false){
				emberEventControlSetDelayMS(MAIN_rebootUpperStairSWEventControl, 5000);
				upperSWreboot = true;
			}
			stateUpperSW = MAIN_StateIdle;
			break;

		case MAIN_StateIdle:
			stateUpperSW = MAIN_StateIdle;
			break;

		default:
			break;
	}
}

/*
 * @func MAIN_handleZigbeeNetworkStateUpperSW
 * @brief To process the Zigbee network states of Upper stair switch
 * @param networkStateUpperSW - The Zigbee network state of Upper stair switch
 * @retval None
 */
void MAIN_handleZigbeeNetworkStateUpperSW(NETWORK_stateZigbeeNetwork_e networkStateUpperSW)
{
	switch(networkStateUpperSW){
		case NETWORK_Jointed:
			LED_blinkLed(LED_2, LED_Pink, 5, 300, 300);
			jointedZigbeeNetwork = true;
			stateUpperSW = MAIN_StateReport;
			emberEventControlSetDelayMS(MAIN_stateUpperStairSWEventControl, 1000);
			break;

		case NETWORK_JoinFail:
			jointedZigbeeNetwork = false;
			stateUpperSW = MAIN_StateIdle;
			emberEventControlSetDelayMS(MAIN_stateUpperStairSWEventControl, 1000);
			break;

		case NETWORK_JoinSuccess:
			LED_blinkLed(LED_2, LED_Blue, 3, 300, 300);
			jointedZigbeeNetwork = true;
			stateUpperSW = MAIN_StateReport;
			emberEventControlSetDelayMS(MAIN_stateUpperStairSWEventControl, 1000);
			break;

		case NETWORK_LostParent:
			LED_blinkLed(LED_2, LED_Yellow, 3, 300, 300);
			jointedZigbeeNetwork = true;
			stateUpperSW = MAIN_StateIdle;
			emberEventControlSetDelayMS(MAIN_stateUpperStairSWEventControl, 1000);
			break;

		case NETWORK_OutNetwork:
			if(jointedZigbeeNetwork){
				LED_blinkLed(LED_2, LED_Yellow, 5, 300, 300);
				stateUpperSW = MAIN_StateReboot;
				emberEventControlSetDelayMS(MAIN_stateUpperStairSWEventControl, 1000);
			}
			break;

		default:
			break;
	}
}

/*
 * @func MAIN_handleNumberButtonPresses
 * @brief To process the number of button presses
 * @param
 * 		  buttonIndex - BUTTON_1 or BUTTON_2
 * 		  numberPresses - The number of button presses
 * @retval None
 */
void MAIN_handleNumberButtonPresses(BUTTON_indexButton_e buttonIndex, BUTTON_numberPresses_e numberPresses)
{
	switch(numberPresses){
		case BUTTON_Press_1:
			if(buttonIndex == BUTTON_2){
				if(MAIN_stateStairLight == MAIN_LightON){
					MAIN_synchronizeStateStairLight_ControlMode_PIRtimeWithBelowSW(MAIN_networkIdUpperSW,
																				   MAIN_EndpointStairLight,
																				   STAIRLIGHT_ENDPOINT_BELOWSW,
																				   ZCL_ON_OFF_CLUSTER_ID,
																				   ZCL_OFF_COMMAND_ID);
				} else if(MAIN_stateStairLight == MAIN_LightOFF){
					MAIN_synchronizeStateStairLight_ControlMode_PIRtimeWithBelowSW(MAIN_networkIdUpperSW,
																				   MAIN_EndpointStairLight,
																				   STAIRLIGHT_ENDPOINT_BELOWSW,
																				   ZCL_ON_OFF_CLUSTER_ID,
																				   ZCL_ON_COMMAND_ID);
				}
			}
			break;

		case BUTTON_Press_2:
			if(buttonIndex == BUTTON_2){
				if(MAIN_controlMode == MAIN_ModeManual){
					MAIN_selectControlMode(MAIN_ModeAuto, LED_Blue, PIR_EnableInterrupt);
					MAIN_synchronizeStateStairLight_ControlMode_PIRtimeWithBelowSW(MAIN_networkIdUpperSW,
																				   MAIN_EndpointPIRAndControlMode,
																				   PIR_CONTROLMODE_ENDPOINT_BELOWSW,
																				   ZCL_ON_OFF_CLUSTER_ID,
																				   ZCL_ON_COMMAND_ID);
				} else if(MAIN_controlMode == MAIN_ModeAuto){
					MAIN_selectControlMode(MAIN_ModeManual, LED_Red, PIR_DisableInterrupt);
					MAIN_synchronizeStateStairLight_ControlMode_PIRtimeWithBelowSW(MAIN_networkIdUpperSW,
																				   MAIN_EndpointPIRAndControlMode,
																				   PIR_CONTROLMODE_ENDPOINT_BELOWSW,
																				   ZCL_ON_OFF_CLUSTER_ID,
																				   ZCL_OFF_COMMAND_ID);
				}
			}
			break;

		case BUTTON_Press_3:
			if(buttonIndex == BUTTON_2){
				if(activateInitiatorTargetForSWs == MAIN_TargetType){
					activateInitiatorTargetForSWs = MAIN_InitiatorType;
					stateActivateBinding = emberAfPluginFindAndBindInitiatorStart(MAIN_EndpointStairLight);
					if(stateActivateBinding == EMBER_ZCL_STATUS_SUCCESS){
						LED_blinkLed(LED_2, LED_Green, 2, 300, 300);
					}
				} else if(activateInitiatorTargetForSWs == MAIN_InitiatorType){
					activateInitiatorTargetForSWs = MAIN_TargetType;
					stateActivateBinding = emberAfPluginFindAndBindTargetStart(MAIN_EndpointStairLight);
					if(stateActivateBinding == EMBER_ZCL_STATUS_SUCCESS){
						LED_blinkLed(LED_2, LED_Green, 1, 300, 300);
					}
				}
			}
			break;

		case BUTTON_Press_4:
			if(buttonIndex == BUTTON_2){
				if(activateInitiatorTargetForPIRs == MAIN_TargetType){
					activateInitiatorTargetForPIRs = MAIN_InitiatorType;
					stateActivateBinding = emberAfPluginFindAndBindInitiatorStart(MAIN_EndpointPIRAndControlMode);
					if(stateActivateBinding == EMBER_ZCL_STATUS_SUCCESS){
						LED_blinkLed(LED_2, LED_Blue, 2, 300, 300);
					}
				} else if(activateInitiatorTargetForPIRs == MAIN_InitiatorType){
					activateInitiatorTargetForPIRs = MAIN_TargetType;
					stateActivateBinding = emberAfPluginFindAndBindTargetStart(MAIN_EndpointPIRAndControlMode);
					if(stateActivateBinding == EMBER_ZCL_STATUS_SUCCESS){
					LED_blinkLed(LED_2, LED_Blue, 1, 300, 300);
					}
				}
			}
			break;

		case BUTTON_Press_7:
			if(buttonIndex == BUTTON_2){
				stateUpperSW = MAIN_StateReboot;
				emberEventControlSetDelayMS(MAIN_stateUpperStairSWEventControl, 1000);
			}
			break;

		default:
			break;
	}
}

/*
 * @func MAIN_handleMotionSignal
 * @brief To process the motion signal
 * @param stateMotion - Motion or Unmotion
 * @retval None
 */
void MAIN_handleMotionSignal(PIR_stateMotion_e stateMotion)
{
	switch(stateMotion){
		case PIR_Motion:
			MAIN_synchronizeStateStairLight_ControlMode_PIRtimeWithBelowSW(MAIN_networkIdUpperSW,
																		   MAIN_EndpointPIRAndControlMode,
																		   PIR_CONTROLMODE_ENDPOINT_BELOWSW,
																		   ZCL_IAS_ZONE_CLUSTER_ID,
																		   ZCL_ZONE_ENROLL_RESPONSE_COMMAND_ID);
			if(MAIN_stateStairLight == MAIN_LightOFF){
				MAIN_synchronizeStateStairLight_ControlMode_PIRtimeWithBelowSW(MAIN_networkIdUpperSW,
																			   MAIN_EndpointStairLight,
																			   STAIRLIGHT_ENDPOINT_BELOWSW,
																			   ZCL_ON_OFF_CLUSTER_ID,
																			   ZCL_ON_COMMAND_ID);
			}
			break;

		case PIR_Unmotion:
			emberAfCorePrintln("Turning off the stair light!");
			break;

		default:
			break;
	}
}

/*
 * @func MAIN_selectControlMode
 * @brief To select the control mode "AUTO" or "MANUAL"
 * @param
 * 		  selectedControlMode - To the control mode "AUTO" or "MANUAL"
 * 		  modeLED - The color of LED to display the control mode
 * 		  interruptControl - To active/inactive the interrupt on PIR sensor
 * @retval None
 */
void MAIN_selectControlMode(MAIN_controlMode_e selectedControlMode,
							LED_ledColor_e modeLED,
							PIR_controlInterrupt_e interruptControl)
{
		// Select the control mode
	if(selectedControlMode == MAIN_ModeAuto){
		MAIN_controlMode = MAIN_ModeAuto;
	} else if(selectedControlMode == MAIN_ModeManual){
		MAIN_controlMode = MAIN_ModeManual;
	}

		// Turn on the mode led & configure Interrupt for PIR
	LED_turnOffLed(LED_1);
	LED_turnOnLed(LED_1, modeLED);
	PIR_controlInterruptPIRsensor(interruptControl);
}

/*
 * @func MAIN_synchronizeStateStairLight_ControlMode_PIRtimeWithBelowSW
 * @brief To synchronize state of stair light, control mode and PIR time with below stair switch using binding method
 * @param
 * 		  networkIdDevice - The networkID of device to send the report for below stair switch
 * 		  reportEndpoint - The endpoint of device to send the report for below stair switch
 * 		  receptionEndpoint - The endpoint of Below stair switch to receive this message
 * 		  clusterID - The identifier of cluster to send command
 * 		  commandID - The identifier of command to send command
 * @retval None
 */
void MAIN_synchronizeStateStairLight_ControlMode_PIRtimeWithBelowSW(EmberNodeId networkIdDevice,
										   	   	         	 	 	MAIN_endpoints_e reportEndpoint,
																	uint8_t receptionEndpoint,
																	EmberAfClusterId clusterID,
																	uint8_t commandID)
{
		// Get information of each row of binding table
	EmberBindingTableEntry rowInfBindingTable;

		// Report the state of stair light or synchronize PIR to upper stair switch
	for(uint8_t i = 0; i < EMBER_BINDING_TABLE_SIZE; i++){
		if(emberGetBinding(i, &rowInfBindingTable) == EMBER_SUCCESS){
			if(rowInfBindingTable.type == EMBER_UNICAST_BINDING && rowInfBindingTable.local == reportEndpoint
					&& rowInfBindingTable.remote == receptionEndpoint && rowInfBindingTable.clusterId == clusterID
					&& networkIdDevice != emberGetBindingRemoteNodeId(i))
			{
				switch(clusterID){
					case ZCL_ON_OFF_CLUSTER_ID:
						emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER),
											  	  ZCL_ON_OFF_CLUSTER_ID,
												  commandID,
												  "");
						emberAfSetCommandEndpoints(reportEndpoint, receptionEndpoint);
						(void) emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, emberGetBindingRemoteNodeId(i));
						break;

					case ZCL_IAS_ZONE_CLUSTER_ID:
						emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER),
												  ZCL_IAS_ZONE_CLUSTER_ID,
												  commandID,
												  "");
						emberAfSetCommandEndpoints(reportEndpoint, receptionEndpoint);
						(void) emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, emberGetBindingRemoteNodeId(i));
						break;

					default:
						break;
				}
			}
		} else if(emberGetBinding(i, &rowInfBindingTable) != EMBER_SUCCESS) break;
	}
}

/*
 * @func MAIN_rebootUpperStairSWEventHandler
 * @brief To reboot device after leaving network
 * @param None
 * @retval None
 */
void MAIN_rebootUpperStairSWEventHandler(void)
{
		// Inactivate this event
	emberEventControlSetInactive(MAIN_rebootUpperStairSWEventControl);

		// Reboot this device
	halReboot();
}

	/*------------------------------- END THIS FILE -------------------------------*/
