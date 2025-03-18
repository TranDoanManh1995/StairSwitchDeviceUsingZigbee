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
#include "source/driver/ADC_lib/ADC_lib.h"
#include "source/driver/I2C_lib/I2C_lib.h"
#include "source/mid/KALMAN_lib/KALMAN_lib.h"

	/*------------------------------- Initialize variables -------------------------------*/

	// Variables
boolean jointedZigbeeNetwork = false;													// Check whether this device joined the Zigbee network
boolean belowSWreboot = false;															// To only reboot once when leaving the Zigbee network
MAIN_deviceTypeInBinding_e activateInitiatorTargetForSWs = MAIN_InitiatorType;			// To select activating "Target" or "Initiator" to bind two buttons on two stair SWs that controls the stair light
MAIN_deviceTypeInBinding_e activateInitiatorTargetForPIRs = MAIN_InitiatorType;			// To select activating "Target" or "Initiator" to bind two PIRs sensor on two stair SWs that synchronizes time of PIRs

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
	ADC_initializeADC0();
	I2C_initializeI2C0();
	KALMAN_initializeFilterKalman(90, 90, 0.01);
	PIR_initializePIRsensor(MAIN_handleMotionSignal);
	BUTTON_initializeButton(MAIN_handleNumberButtonPresses);
	NETWORK_initializeZigbeeNetwork(MAIN_handleZigbeeNetworkStateBelowSW);

		// Disable interrupt of PIR sensor
	PIR_controlInterruptPIRsensor(PIR_DisableInterrupt);

		// Trigger event to process the states of below stair switch
	stateBelowSW = MAIN_StatePowerOn;
	emberEventControlSetActive(MAIN_stateBelowStairSWEventControl);
}

/*
 * @func MAIN_stateBelowStairSWEventHandler
 * @brief To process the states below stair switch appropriately
 * @param None
 * @retval None
 */
void MAIN_stateBelowStairSWEventHandler(void)
{
		// Inactivate this event
	emberEventControlSetInactive(MAIN_stateBelowStairSWEventControl);

		// Get the network state of below stair switch
	EmberNetworkStatus networkStateBelowSW;

		// Process the states below stair switch
	switch(stateBelowSW){
		case MAIN_StatePowerOn:
			networkStateBelowSW = emberAfNetworkState();
			if(networkStateBelowSW == EMBER_NO_NETWORK){
				LED_blinkLed(LED_2, LED_Red, 3, 300, 300);
				NETWORK_findJoinZigbeeNetwork();
			}
			stateBelowSW = MAIN_StateIdle;
			break;

		case MAIN_StateReport:
			TRANSMIT_reportModelIDBelowSWForZC(MAIN_EndpointReportDeviceInf);
			MAIN_networkIdBelowSW = emberAfGetNodeId();
			emberEventControlSetActive(MAIN_readBrightnessValueEventControl);
			emberEventControlSetActive(MAIN_readTempHumValueEventControl);
			MAIN_selectControlMode(MAIN_EndpointPIRAndControlMode, MAIN_ModeManual, LED_Red, PIR_DisableInterrupt);
			stateBelowSW = MAIN_StateIdle;
			break;

		case MAIN_StateReboot:
			networkStateBelowSW = emberAfNetworkState();
			if(networkStateBelowSW == EMBER_JOINED_NETWORK){
					// Report the Zigbee network
				messageLeaveingNetwork[1] = 0x00;
				emberSendZigDevRequest(0x0000,
							   	   	   LEAVE_RESPONSE,
									   EMBER_AF_DEFAULT_APS_OPTIONS,
									   messageLeaveingNetwork,
									   sizeof(messageLeaveingNetwork));
					// Clear the binding table & Leave the Zigbee network
				emberClearBindingTable();
				emberLeaveNetwork();
			}
				// Reboot the below stair switch
			if(belowSWreboot == false){
				emberEventControlSetDelayMS(MAIN_rebootBelowStairSWEventControl, 5000);
				belowSWreboot = true;
			}
			stateBelowSW = MAIN_StateIdle;
			break;

		case MAIN_StateIdle:
			stateBelowSW = MAIN_StateIdle;
			break;

		default:
			break;
	}
}

/*
 * @func MAIN_handleZigbeeNetworkStateBelowSW
 * @brief To process the Zigbee network states of below stair switch
 * @param networkStateBelowSW - The Zigbee network state of below stair switch
 * @retval None
 */
void MAIN_handleZigbeeNetworkStateBelowSW(NETWORK_stateZigbeeNetwork_e networkStateBelowSW)
{
	switch(networkStateBelowSW){
		case NETWORK_Jointed:
			LED_blinkLed(LED_2, LED_Pink, 5, 300, 300);
			jointedZigbeeNetwork = true;
			stateBelowSW = MAIN_StateReport;
			emberEventControlSetDelayMS(MAIN_stateBelowStairSWEventControl, 1000);
			break;

		case NETWORK_JoinFail:
			jointedZigbeeNetwork = false;
			stateBelowSW = MAIN_StateIdle;
			emberEventControlSetDelayMS(MAIN_stateBelowStairSWEventControl, 1000);
			break;

		case NETWORK_JoinSuccess:
			LED_blinkLed(LED_2, LED_Blue, 3, 300, 300);
			jointedZigbeeNetwork = true;
			stateBelowSW = MAIN_StateReport;
			emberEventControlSetDelayMS(MAIN_stateBelowStairSWEventControl, 1000);
			break;

		case NETWORK_LostParent:
			LED_blinkLed(LED_2, LED_Yellow, 3, 300, 300);
			jointedZigbeeNetwork = true;
			stateBelowSW = MAIN_StateIdle;
			emberEventControlSetDelayMS(MAIN_stateBelowStairSWEventControl, 1000);
			break;

		case NETWORK_OutNetwork:
			if(jointedZigbeeNetwork){
				LED_blinkLed(LED_2, LED_Yellow, 5, 300, 300);
				stateBelowSW = MAIN_StateReboot;
				emberEventControlSetDelayMS(MAIN_stateBelowStairSWEventControl, 1000);
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
				stateStairLight = GPIO_PinOutGet(LED2_PORT, LED2_BLUE_PIN);
				if(stateStairLight == MAIN_LightOFF){
					MAIN_turnOnOffStairLight(MAIN_LightON, MAIN_EndpointStairLight, MAIN_networkIdBelowSW, ZCL_ON_COMMAND_ID);
				} else if(stateStairLight == MAIN_LightON){
					MAIN_turnOnOffStairLight(MAIN_LightOFF, MAIN_EndpointStairLight, MAIN_networkIdBelowSW, ZCL_OFF_COMMAND_ID);
				}
			}
			break;

		case BUTTON_Press_2:
			if(buttonIndex == BUTTON_2){
				if(MAIN_controlMode == MAIN_ModeManual){
					MAIN_selectControlMode(MAIN_EndpointPIRAndControlMode, MAIN_ModeAuto, LED_Blue, PIR_EnableInterrupt);
					MAIN_synchronizeStateStairLight_ControlMode_PIRtimeWithUpperSW(MAIN_networkIdBelowSW,
																				   MAIN_EndpointPIRAndControlMode,
																				   PIR_CONTROLMODE_ENDPOINT_UPPERSW,
																				   ZCL_ON_OFF_CLUSTER_ID,
																				   ZCL_ON_COMMAND_ID);
				} else if(MAIN_controlMode == MAIN_ModeAuto){
					MAIN_selectControlMode(MAIN_EndpointPIRAndControlMode, MAIN_ModeManual, LED_Red, PIR_DisableInterrupt);
					MAIN_synchronizeStateStairLight_ControlMode_PIRtimeWithUpperSW(MAIN_networkIdBelowSW,
																				   MAIN_EndpointPIRAndControlMode,
																				   PIR_CONTROLMODE_ENDPOINT_UPPERSW,
																				   ZCL_ON_OFF_CLUSTER_ID,
																				   ZCL_OFF_COMMAND_ID);
				}
			}
			break;

		case BUTTON_Press_3:
			if(buttonIndex == BUTTON_2){
				if(activateInitiatorTargetForSWs == MAIN_InitiatorType){
					activateInitiatorTargetForSWs = MAIN_TargetType;
					stateActivateBinding = emberAfPluginFindAndBindTargetStart(MAIN_EndpointStairLight);
					if(stateActivateBinding == EMBER_ZCL_STATUS_SUCCESS){
						LED_blinkLed(LED_2, LED_Green, 1, 300, 300);
					}
				} else if(activateInitiatorTargetForSWs == MAIN_TargetType){
					activateInitiatorTargetForSWs = MAIN_InitiatorType;
					stateActivateBinding = emberAfPluginFindAndBindInitiatorStart(MAIN_EndpointStairLight);
					if(stateActivateBinding == EMBER_ZCL_STATUS_SUCCESS){
						LED_blinkLed(LED_2, LED_Green, 2, 300, 300);
					}
				}
			}
			break;

		case BUTTON_Press_4:
			if(buttonIndex == BUTTON_2){
				if(activateInitiatorTargetForPIRs == MAIN_InitiatorType){
					activateInitiatorTargetForPIRs = MAIN_TargetType;
					stateActivateBinding = emberAfPluginFindAndBindTargetStart(MAIN_EndpointPIRAndControlMode);
					if(stateActivateBinding == EMBER_ZCL_STATUS_SUCCESS){
						LED_blinkLed(LED_2, LED_Blue, 1, 300, 300);
					}
				} else if(activateInitiatorTargetForPIRs == MAIN_TargetType){
					activateInitiatorTargetForPIRs = MAIN_InitiatorType;
					stateActivateBinding = emberAfPluginFindAndBindInitiatorStart(MAIN_EndpointPIRAndControlMode);
					if(stateActivateBinding == EMBER_ZCL_STATUS_SUCCESS){
					LED_blinkLed(LED_2, LED_Blue, 2, 300, 300);
					}
				}
			}
			break;

		case BUTTON_Press_7:
			if(buttonIndex == BUTTON_2){
				stateBelowSW = MAIN_StateReboot;
				emberEventControlSetDelayMS(MAIN_stateBelowStairSWEventControl, 1000);
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
		// Get the status of stair light
	stateStairLight = GPIO_PinOutGet(LED2_PORT, LED2_BLUE_PIN);

		// Process the motion signal
	switch(stateMotion){
		case PIR_Motion:
			MAIN_synchronizeStateStairLight_ControlMode_PIRtimeWithUpperSW(MAIN_networkIdBelowSW,
																		   MAIN_EndpointPIRAndControlMode,
																		   PIR_CONTROLMODE_ENDPOINT_UPPERSW,
																		   ZCL_IAS_ZONE_CLUSTER_ID,
																		   ZCL_ZONE_ENROLL_RESPONSE_COMMAND_ID);
			if(stateStairLight == MAIN_LightOFF){
				MAIN_turnOnOffStairLight(MAIN_LightON, MAIN_EndpointStairLight, MAIN_networkIdBelowSW, ZCL_ON_COMMAND_ID);
			}
			break;

		case PIR_Unmotion:
			if(stateStairLight == MAIN_LightON){
				MAIN_turnOnOffStairLight(MAIN_LightOFF, MAIN_EndpointStairLight, MAIN_networkIdBelowSW, ZCL_OFF_COMMAND_ID);
			}
			break;

		default:
			break;
	}
}

/*
 * @func MAIN_selectControlMode
 * @brief To select the control mode "AUTO" or "MANUAL"
 * @param
 * 		  reportEndpoint - The endpoint of device to send the report for ZC
 * 		  selectedControlMode - To the control mode "AUTO" or "MANUAL"
 * 		  modeLED - The color of LED to display the control mode
 * 		  interruptControl - To active/inactive the interrupt on PIR sensor
 * @retval None
 */
void MAIN_selectControlMode(MAIN_endpoints_e reportEndpoint,
							MAIN_controlMode_e selectedControlMode,
							LED_ledColor_e modeLED,
							PIR_controlInterrupt_e interruptControl)
{
		// Select the control mode
	if(selectedControlMode == MAIN_ModeAuto){
		MAIN_controlMode = MAIN_ModeAuto;
	} else if(selectedControlMode == MAIN_ModeManual){
		MAIN_controlMode = MAIN_ModeManual;
	}

		// Report the control mode for ZC, turn on the mode led & configure Interrupt for PIR
	TRANSMIT_reportControlModeForZC(reportEndpoint, selectedControlMode);
	LED_turnOffLed(LED_1);
	LED_turnOnLed(LED_1, modeLED);
	PIR_controlInterruptPIRsensor(interruptControl);
}

/*
 * @func MAIN_turnOnOffStairLight
 * @brief To turn on/off the stair light
 * @param
 * 		  stairLightState - The state of stair light to control
 * 		  valueReportEndpoint - The endpoint of device to send the report for ZC & upper stair switch
 * 		  deviceNetworkID - The networkID of device to send the report for upper stair switch
 * 		  reportCommand - The command to control the stair light
 * @retval None
 */
void MAIN_turnOnOffStairLight(MAIN_stateStairLight_e stairLightState,
						  	  MAIN_endpoints_e valueReportEndpoint,
							  EmberNodeId deviceNetworkID,
							  uint8_t reportCommandID)
{
		// Control the stair light
	if(stairLightState == MAIN_LightON){
		LED_turnOnLed(LED_2, LED_Blue);
	} else if(stairLightState == MAIN_LightOFF){
		LED_turnOffLed(LED_2);
	}

		// Report the state of stair light for ZC & upper stair switch
	TRANSMIT_reportStateStairLightForZC(valueReportEndpoint, !stairLightState);			// Reverse to be suitable with the state of stair light: LightON = 1, LightOFF = 0
	MAIN_synchronizeStateStairLight_ControlMode_PIRtimeWithUpperSW(deviceNetworkID,
																   valueReportEndpoint,
																   STAIRLIGHT_ENDPOINT_UPPERSW,
																   ZCL_ON_OFF_CLUSTER_ID,
																   reportCommandID);
}

/*
 * @func MAIN_synchronizeStateStairLight_ControlMode_PIRtimeWithUpperSW
 * @brief To synchronize state of stair light, control mode and PIR time with upper stair switch using binding method
 * @param
 * 		  networkIdDevice - The networkID of device to send the report for upper stair switch
 * 		  reportEndpoint - The endpoint of device to send the report for upper stair switch
 * 		  receptionEndpoint - The endpoint of upper stair switch to receive this message
 * 		  clusterID - The identifier of cluster to send command
 * 		  commandID - The identifier of command to send command
 * @retval None
 */
void MAIN_synchronizeStateStairLight_ControlMode_PIRtimeWithUpperSW(EmberNodeId networkIdDevice,
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
 * @func MAIN_readBirghtnessValueEventHandler
 * @brief To read the brightness value
 * @param None
 * @retval None
 */
void MAIN_readBirghtnessValueEventHandler(void)
{
		// Inactivate this event
	emberEventControlSetInactive(MAIN_readBrightnessValueEventControl);

		// Variables to get the brightness value
	static uint16_t valueBrightness = 0, valueNewKalmanBrightness = 0,  valueOldKalmanBrightness = 0;
	valueBrightness = ADC_readLightSensorValue();
	valueNewKalmanBrightness = KALMAN_updateEstimate(valueBrightness);

		// Check to report the brightness value
	if(abs(valueNewKalmanBrightness - valueOldKalmanBrightness) > 30){
		emberAfCorePrintln("The brightness value: %d lux!", valueNewKalmanBrightness);
		valueOldKalmanBrightness = valueNewKalmanBrightness;
		TRANSMIT_reportBrightnessValueForZC(MAIN_EndpointLightSensor, valueNewKalmanBrightness);
	}

		// Recall this event every CYCLE_READ_BRIGHTNESS_VALUE
	emberEventControlSetDelayMS(MAIN_readBrightnessValueEventControl, CYCLE_READ_BRIGHTNESS_VALUE);
}

/*
 * @func MAIN_readTempHumValueEventHandler
 * @brief To read the temperature & humidity value
 * @param None
 * @retval None
 */
void MAIN_readTempHumValueEventHandler(void)
{
		// Inactivate this event
	emberEventControlSetInactive(MAIN_readTempHumValueEventControl);

		// Variables to get the temparature & humidity value
	static uint8_t valueNewTemp = 0, valueOldTemp = 0, valueNewHum = 0, valueOldHum = 0;
	valueNewTemp = I2C_getTempValueAfterProcessing();
	valueNewHum = I2C_getHumValueAfterProcessing();
	if(valueNewHum > 100) valueNewHum = 100;			// Because this humidity sensor doesn't activate properly

		// Check to report the temparature & humidity value
	if(abs(valueNewTemp - valueOldTemp) > 2){
		emberAfCorePrintln("The temperature value: %d oC!", valueNewTemp);
		valueOldTemp = valueNewTemp;
		TRANSMIT_reportTemperatureValueForZC(MAIN_EndpointTempSensor, valueNewTemp);
	}
	if(abs(valueNewHum - valueOldHum) > 2){
		emberAfCorePrintln("The humidity value: %d %%!", valueNewHum);
		valueOldHum = valueNewHum;
		TRANSMIT_reportHumidityValueForZC(MAIN_EndpointHumSensor, valueNewHum);
	}

		// Recall this event every CYCLE_READ_TEMPHUM_VALUE
	emberEventControlSetDelayMS(MAIN_readTempHumValueEventHandler, CYCLE_READ_TEMPHUM_VALUE);
}

/*
 * @func MAIN_rebootBelowStairSWEventHandler
 * @brief To reboot device after leaving network
 * @param None
 * @retval None
 */
void MAIN_rebootBelowStairSWEventHandler(void)
{
		// Inactivate this event
	emberEventControlSetInactive(MAIN_rebootBelowStairSWEventControl);

		// Reboot this device
	halReboot();
}

	/*------------------------------- END THIS FILE -------------------------------*/
