/*
 * main.c
 *
 *  Created on: Jun 4, 2024
 *  Author: TRAN DOAN MANH
 *  Description: Main program
 */

	/*------------------------------- All libraries -------------------------------*/

#include "app/framework/include/af.h"
#include "app/framework/plugin/network-creator/network-creator.h"
#include "app/framework/plugin/network-creator-security/network-creator-security.h"
#include "source/app/main/main.h"
#include "source/app/TRANSMIT_lib/TRANSMIT_lib.h"
#include "source/driver/UART_lib/UART_lib.h"
#include "source/mid/MANAGE_lib/MANAGE_lib.h"
#include "source/mid/LED_lib/LED_lib.h"

	/*------------------------------- Initialize variables -------------------------------*/

	// Variables
bool ZCInfReport = false;			// Check if ZC has reported its information to ESP32

	/*------------------------------- All funtions -------------------------------*/

/*
 * @func emberAfMainInitCallback
 * @brief This function is called from the application's main function. It gives the
 * 		  application a chance to do any initialization required at system startup.
 * @param None
 * @retval None
 */
void emberAfMainInitCallback(void)
{
		// Initialize LED, Button & callback
	LED_initializeLed();
	UART_initializeUart(MAIN_handleReceivedDataFromESP32);
	BUTTON_initializeButton(MAIN_handleNumberButtonPresses);
	RECEIVE_initializeReceiveData(MAIN_handleReportsFromDevicesZigbeeNetwork);

		// Trigger event to process the state of Zigbee Coordinator
	stateZigbeeCoordinator = MAIN_StatePowerOn;
	emberEventControlSetDelayMS(MAIN_stateZigbeeCoordinatorEventControl, 10000);
}

/*
 * @func MAIN_stateZigbeeCoordinatorEventHandler
 * @brief To process all states of ZC
 * @param None
 * @retval None
 */
void MAIN_stateZigbeeCoordinatorEventHandler(void)
{
		// Inactivate this event
	emberEventControlSetInactive(MAIN_stateZigbeeCoordinatorEventControl);

		// To get current network state of Zigbee Coordinator
	EmberNetworkStatus networkStateZC;

		// Process all states of Zigbee Coordinator
	switch(stateZigbeeCoordinator){
		case MAIN_StatePowerOn:
			networkStateZC = emberAfNetworkState();
			if(networkStateZC == EMBER_NO_NETWORK){
				LED_blinkLed(LED_1, LED_Red, 1, 300, 300);
				emberEventControlSetDelayMS(MAIN_stateZigbeeCoordinatorEventControl, 3000);
			}
			if(ZCInfReport == false){
				MAIN_handleReportsFromDevicesZigbeeNetwork(NETWORKID_ZC,
														   ENDPOINT_ZC,
														   RECEIVE_DeviceZigbeeCoordinator,
														   (uint8_t *)"ZC");
				ZCInfReport = true;
			}
			break;

		case MAIN_StateIdle:
			break;

		default:
			break;
	}
}

/*
 * @func emberAfStackStatusCallback
 * @brief This function is called by the application framework from the stack status
 *		  handler.  This callbacks provides applications an opportunity to be notified
 * 		  of changes to the stack status and take appropriate action
 * @param networkStateZC - The Zigbee network state of Zigbee Coordinator
 * @retval True/False
 */
bool emberAfStackStatusCallback(EmberStatus networkStateZC)
{
		// Check the state of network to process
	if(networkStateZC == EMBER_NETWORK_UP){
		stateZigbeeCoordinator = MAIN_StateIdle;
		LED_turnOnLed(LED_1, LED_Blue);
	} else{
		stateZigbeeCoordinator = MAIN_StatePowerOn;
		LED_blinkLed(LED_1, LED_Red, 1, 300, 300);
		emberEventControlSetDelayMS(MAIN_stateZigbeeCoordinatorEventControl, 3000);
	}

		// This value is ignored by the framework.
	return false;
}

/*
 * @func MAIN_handleReportsFromDevicesZigbeeNetwork
 * @brief To process all data that receive from all devices in Zigbee network
 * @param
 * 		  networkIdDevice - The network address of device reports to ZC
 * 		  valueReportEndpoint - The endpoint of device reports to ZC
 * 		  typeReportMessage - The type of reported message
 * 		  contentReport - The content of reported message
 * @retval None
 */
void MAIN_handleReportsFromDevicesZigbeeNetwork(EmberNodeId networkIdDevice,
												RECEIVE_valueEndpoint valueReportEndpoint,
												RECEIVE_typeReportMessage_e typeReportMessage,
												uint8_t *contentReport)
{
		// Variables
	TOKENINF_typeDevice_e typeDevice;				// To get the type of device that reports to ZC
	bool saveInfDevice;								// Variable not to report the device information for ESP32 when it is powered again or resets
	uint16_t *valueBrightness_16bits;				// To get the 16 bits brightness value
	uint8_t valueBrightness_8bits[2] = {0};			// To push the 16 bits brightness value into the 8 bits array

		// Process appropriately with content of reported message
	switch(typeReportMessage){
		case RECEIVE_DeviceZigbeeCoordinator:
		case RECEIVE_DeviceJoinNetwork:
			saveInfDevice = MANAGE_saveInfDeviceIntoNVM3(networkIdDevice, (char *)contentReport);
			if(saveInfDevice == true){
				typeDevice =  MANAGE_getTypeDeviceInNVM3(networkIdDevice);
				UART_FRAMEreportJoinZigbeeNetwork(networkIdDevice, valueReportEndpoint, typeDevice);
				emberSerialWriteData(comPortUsart2, transmissionPayload, transmissionPayload[1] + 2);
			}
			break;

		case RECEIVE_ReportValueTemp:
			UART_FRAMEreportTemperatureValue(networkIdDevice, valueReportEndpoint, *contentReport);
			emberSerialWriteData(comPortUsart2, transmissionPayload, transmissionPayload[1] + 2);
			break;

		case RECEIVE_ReportValueHum:
			UART_FRAMEreportHumidityValue(networkIdDevice, valueReportEndpoint, *contentReport);
			emberSerialWriteData(comPortUsart2, transmissionPayload, transmissionPayload[1] + 2);
			break;

		case RECEIVE_ReportValueBrightness:
			valueBrightness_16bits = (uint16_t *)contentReport;
			valueBrightness_8bits[0] = *valueBrightness_16bits & 0xFF;
			valueBrightness_8bits[1] = (*valueBrightness_16bits >> 8) & 0xFF;
			UART_FRAMEreportBrightnessValue(networkIdDevice, valueReportEndpoint, valueBrightness_8bits);
			emberSerialWriteData(comPortUsart2, transmissionPayload, transmissionPayload[1] + 2);
			break;

		case RECEIVE_ReportControlMode:
			UART_FRAMEreportControlMode(networkIdDevice, valueReportEndpoint, *contentReport);
			emberSerialWriteData(comPortUsart2, transmissionPayload, transmissionPayload[1] + 2);
			break;

		case RECEIVE_ReportOnOffLight:
			UART_FRAMEreportOnOffLight(networkIdDevice, valueReportEndpoint, *contentReport);
			emberSerialWriteData(comPortUsart2, transmissionPayload, transmissionPayload[1] + 2);
			break;

		case RECEIVE_DeviceLeaveNetwork:
			MANAGE_removeInfDeviceInNVM3(networkIdDevice);
			UART_FRAMEreportLeaveZigbeeNetwork(networkIdDevice, valueReportEndpoint);
			emberSerialWriteData(comPortUsart2, transmissionPayload, transmissionPayload[1] + 2);
			break;

		default:
			break;
		}
}

/*
 * @func MAIN_handleReceivedDataFromESP32
 * @brief To process all received data from ESP32
 * @param receivedData
 * @retval None
 */
void MAIN_handleReceivedDataFromESP32(uint8_t* receivedData)
{
		// Variables
	MAIN_frameUART_s *frameControlData = (MAIN_frameUART_s *)receivedData;													// Structure variable to process received data
	EmberNodeId networkIdDevice = frameControlData->lowByteNetworkID | (frameControlData->highByteNetworkID << 8);			// Get the type of device to control appropriately
	EmberNodeId networkIdDeviceToControl;																					// Get networkID of device in NVM3 to control
	TOKENINF_typeDevice_e typeDevice = MANAGE_getTypeDeviceInNVM3(networkIdDevice);										// Get type of device

		// Process control data is sent by ESP32
	switch(typeDevice){
		case TOKENINF_ZC:
			if(frameControlData->commandID == UART_CreateZigbeeNetwork){
				emberAfPluginNetworkCreatorNetworkForm(1, 0xABCD, 10, 15);
			} else if(frameControlData->commandID == UART_OpenZigbeeNetwork){
				emberAfPluginNetworkCreatorSecurityOpenNetwork();
				LED_blinkLed(LED_2, LED_Green, 1, 300, 300);
			} else if(frameControlData->commandID == UART_CloseZigbeeNetwork){
				emberAfPluginNetworkCreatorSecurityCloseNetwork();
				LED_blinkLed(LED_2, LED_Red, 1, 300, 300);
			} else if(frameControlData->commandID == UART_LeaveZigbeeNetwork){
				if(frameControlData->controlData[0] == TOKENINF_UpperStair){
					networkIdDeviceToControl = MANAGE_getNetworkIdDeviceInNVM3(TOKENINF_UpperStair);
					TRANSMIT_removeDeviceInZigbeeNetwork(networkIdDeviceToControl);
					LED_blinkLed(LED_2, LED_Yellow, 1, 300, 300);
				} else if(frameControlData->controlData[0] == TOKENINF_BelowStair){
					networkIdDeviceToControl = MANAGE_getNetworkIdDeviceInNVM3(TOKENINF_BelowStair);
					TRANSMIT_removeDeviceInZigbeeNetwork(networkIdDeviceToControl);
					LED_blinkLed(LED_2, LED_Yellow, 2, 300, 300);
				} else if(frameControlData->controlData[0] == TOKENINF_AllDevices){
					uint8_t indexDevice = 1;			// Because Zigbee Coordinator is at first location of array
					while(indexDevice < MANAGE_infAllDevices.numberDevices){
						TRANSMIT_removeDeviceInZigbeeNetwork(MANAGE_infAllDevices.infDevice[indexDevice].networkIdDevice);
						indexDevice++;
					}
					LED_blinkLed(LED_2, LED_Yellow, 3, 300, 300);
				}
			}
			break;

		case TOKENINF_BelowStair:
			if(frameControlData->commandID == UART_TurnOnOffLight){
				if(frameControlData->controlData[0] == MAIN_LightON){
					TRANSMIT_turnOnOffStairLight(networkIdDevice, frameControlData->valueEndpoint, ZCL_ON_COMMAND_ID);
				} else if(frameControlData->controlData[0] == MAIN_LightOFF){
					TRANSMIT_turnOnOffStairLight(networkIdDevice, frameControlData->valueEndpoint, ZCL_OFF_COMMAND_ID);
				}
			} else if(frameControlData->commandID == UART_ControlMode){
				if(frameControlData->controlData[0] == MAIN_ModeAuto){
					TRANSMIT_selecStairLightControlMode(networkIdDevice, frameControlData->valueEndpoint, ZCL_ON_COMMAND_ID);
				} else if(frameControlData->controlData[0] == MAIN_ModeManual){
					TRANSMIT_selecStairLightControlMode(networkIdDevice, frameControlData->valueEndpoint, ZCL_OFF_COMMAND_ID);
				}
			}
			break;

		case TOKENINF_UpperStair:
			if(frameControlData->commandID == UART_TurnOnOffLight){
				if(frameControlData->controlData[0] == MAIN_LightON){
					TRANSMIT_turnOnOffStairLight(networkIdDevice, frameControlData->valueEndpoint, ZCL_ON_COMMAND_ID);
				} else if(frameControlData->controlData[0] == MAIN_LightOFF){
					TRANSMIT_turnOnOffStairLight(networkIdDevice, frameControlData->valueEndpoint, ZCL_OFF_COMMAND_ID);
				}
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
		// Variable to get the networkID of device in NVM3
	EmberNodeId networkIdDevice;

		// Handles the number of button presses
	switch(numberPresses){
		case BUTTON_Press_1:
			if(buttonIndex == BUTTON_1){
				emberAfPluginNetworkCreatorSecurityOpenNetwork();
				LED_blinkLed(LED_2, LED_Green, 1, 300, 300);
			} else{
				networkIdDevice = MANAGE_getNetworkIdDeviceInNVM3(TOKENINF_UpperStair);
				TRANSMIT_removeDeviceInZigbeeNetwork(networkIdDevice);
				LED_blinkLed(LED_2, LED_Yellow, 1, 300, 300);
			}
			break;

		case BUTTON_Press_2:
			if(buttonIndex == BUTTON_1){
				emberAfPluginNetworkCreatorSecurityCloseNetwork();
				LED_blinkLed(LED_2, LED_Red, 1, 300, 300);
			} else{
				networkIdDevice = MANAGE_getNetworkIdDeviceInNVM3(TOKENINF_BelowStair);
				TRANSMIT_removeDeviceInZigbeeNetwork(networkIdDevice);
				LED_blinkLed(LED_2, LED_Yellow, 2, 300, 300);
			}
			break;

		case BUTTON_Press_3:
			if(buttonIndex == BUTTON_1){
				emberAfPluginNetworkCreatorNetworkForm(1, 0xABCD, 10, 15);
			} else{
				uint8_t indexDevice = 1;			// Because Zigbee Coordinator is at first location of array
				while(indexDevice < MANAGE_infAllDevices.numberDevices){
					TRANSMIT_removeDeviceInZigbeeNetwork(MANAGE_infAllDevices.infDevice[indexDevice].networkIdDevice);
					indexDevice++;
				}
				LED_blinkLed(LED_2, LED_Yellow, 3, 300, 300);
			}
			break;

		default:
			break;
	}
}

	/*------------------------------- END THIS FILE -------------------------------*/
