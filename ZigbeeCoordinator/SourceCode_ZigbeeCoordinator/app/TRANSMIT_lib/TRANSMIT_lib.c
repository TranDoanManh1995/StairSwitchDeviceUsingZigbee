/*
 * TRANSMIT_lib.c
 *
 *  Created on: Nov 16, 2024
 *  Author: TRAN DOAN MANH
 *  Description: To transmit the control data for all devices in Zigbee network
 */

	/*------------------------------- All libraries -------------------------------*/

#include "app/framework/include/af.h"
#include "app/util/zigbee-framework/zigbee-device-common.h"
#include "source/app/main/main.h"
#include "source/app/TRANSMIT_lib/TRANSMIT_lib.h"

	/*------------------------------- Initialize variables, functions -------------------------------*/


	/*------------------------------- All functions -------------------------------*/

/*
 * @func TRANSMIT_removeDeviceInZigbeeNetwork
 * @brief To remove the device in Zigbee network
 * @param deviceNetworkID - The networkID of device to remove
 * @retval None
 */
void TRANSMIT_removeDeviceInZigbeeNetwork(EmberNodeId deviceNetworkID)
{
		// Variables
	EmberEUI64 IEEEaddressChildDevice = {0,0,0,0,0,0,0,0};					// Remove child device of a device
	uint8_t modeLeavingNetwork = EMBER_ZIGBEE_LEAVE_WITHOUT_REJOIN;			// Select mode of leaving Zigbee network

		// Send the "Leave" command
	emberLeaveRequest(deviceNetworkID,
					  IEEEaddressChildDevice,
					  modeLeavingNetwork,
					  EMBER_APS_OPTION_RETRY | EMBER_APS_OPTION_ENABLE_ROUTE_DISCOVERY);
}

/*
 * @func TRANSMIT_turnOnOffStairLight
 * @brief To turn on/off the stair light
 * @param
 * 		  deviceNetworkID - The networkID of device to control
 * 		  controlEndpoint - Endpoint of device to control
 * 		  controlLight - To turn on/off the stair light
 * @retval None
 */
void TRANSMIT_turnOnOffStairLight(EmberNodeId deviceNetworkID, uint8_t controlEndpoint, uint8_t controlLight)
{
	emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
		                      | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER),
		                      ZCL_ON_OFF_CLUSTER_ID,
							  controlLight,
		                      "");
	emberAfSetCommandEndpoints(ENDPOINT_ZC, controlEndpoint);
	(void) emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, deviceNetworkID);
}

/*
 * @func TRANSMIT_selecStairLightControlMode
 * @brief To select the stair light control mode
 * @param
 * 		  deviceNetworkID - The networkID of device to control
 * 		  controlEndpoint - Endpoint of device to control
 * 		  controlMode - To select the control mode of stair light
 * @retval None
 */
void TRANSMIT_selecStairLightControlMode(EmberNodeId deviceNetworkID,
										 uint8_t controlEndpoint,
										 uint8_t controlMode)
{
	emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
		                       | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER),
		                       ZCL_ON_OFF_CLUSTER_ID,
							   controlMode,
		                       "");
	emberAfSetCommandEndpoints(ENDPOINT_ZC, controlEndpoint);
	(void) emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, deviceNetworkID);
}

	// Three functions to remove a device from the Zigbee network
//	emberSendRemoveDevice(0xFFFF, 0x00000000, 0x00000000);
//	status = emberSendZigbeeLeave(0xFFFF, EMBER_ZIGBEE_LEAVE_WITHOUT_REJOIN);
//	Leave_device_onNetwork(des_NodeID);

	/*------------------------------- END THIS FILE -------------------------------*/

