/*
 * UART_lib.c
 * 
 *  Author: Tran Doan Manh
 *  Description: To communicate between ESP32 & Zigbee Coordinator
*/

    /*------------------------------- All libraries -------------------------------*/
    
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "stdio.h"
#include "string.h"
#include "esp_log.h"
#include "UART_lib.h"
#include "Queue_lib/Queue_lib.h"

    /*------------------------------- Declare & initialize variables, function pointer -------------------------------*/

    // Function pointer
UART_handleReceivedDataFromZC handleReceivedDataFromZC = NULL;

    // Variables
const char *TAG_UART = "UART_RECEIVE_DATA";                                             // Notice the program activities                                              
extern queue_p pQueue;                                                                  // The queue pointer to use the programmer queue
QueueHandle_t pQueueESP32;                                                              // The queue pointer to use the ESP32 queue                                 
UART_typeReceivedByte_e typeReceivedByte = UART_ByteStart;                              // Check the start byte of received report to get report
extern MANAGE_infAllDevices_s MANAGE_infAllDevices;                                     // Contain information of devices in Zigbee network
uint8_t dataSendByZC[DIMENSION_ARRAY];				                                    // An array to store data from queue buffer
uint8_t indexArray;                                                                     // The index of array"dataSendByZC"
uint8_t calculateXORbyte;							                                    // Calculate the CXOR byte in UART reception frame
uint8_t lengthReceptionPayload;                                                         // Get the length of received payload via UART  
uint8_t transmissionPayload[DIMENSION_ARRAY];			                                // An array contains data to transmit for device

	/*------------------------------- All funtions -------------------------------*/

            /*---------- Initialize UART function ----------*/
/*
 * @func UART_initializeUart2
 * @brief To initialize UART2
 * @param handleReceivedDataFromZCCallback - The callback function will be called to process received data from ZC
 * @retval none
 */
void UART_initializeUart2(UART_handleReceivedDataFromZC handleReceivedDataFromZCCallback)
{
		// Register the callback function to handle received data
	if(handleReceivedDataFromZCCallback != NULL){
		handleReceivedDataFromZC = handleReceivedDataFromZCCallback;
	}

        // Select parameters to configure USART2
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

        // Install UART driver and configure USART2.
    uart_driver_install(UART_NUM_2, SIZE_RX_BUFFER  * 2, 0, 40, &pQueueESP32, 0);
    uart_param_config(UART_NUM_2, &uart_config);

        // Set USART2 pins 
    uart_set_pin(UART_NUM_2, UART2_TX_PIN, UART2_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

        // Create a task to handle RX_USART2 events from ISR
    xTaskCreate(UART_handleRXevents, "Handle RX events of UART2", 2048, NULL, 12, NULL);
}

            /*---------- All functions to receive data via UART ----------*/

/*
 * @func UART_handleRXevents
 * @brief To process all RX events
 * @param *pvParameters - 
 * @retval void
 */
void UART_handleRXevents(void *pvParameters)
{
        // Variables
    uart_event_t RXEventUART;                                             // RX events of UART
    uint8_t *receivedPayload = (uint8_t*) malloc(SIZE_RX_BUFFER);         // Allocate memory to store received data

        // Process RX_USART events 
    for(;;){
        if(xQueueReceive(pQueueESP32, (void *)&RXEventUART, (TickType_t)portMAX_DELAY)){
            bzero(receivedPayload, SIZE_RX_BUFFER);
            switch(RXEventUART.type){
            case UART_DATA: 
                lengthReceptionPayload = uart_read_bytes(UART_NUM_2, receivedPayload, RXEventUART.size, portMAX_DELAY);
                if(lengthReceptionPayload){
                    for (uint8_t i = 0; i < lengthReceptionPayload; i++){
                        QUEUE_pushDataIntoDataBuffer(pQueue, receivedPayload);
                        receivedPayload++;
                    }
                        // Handle instances of the received data from ESP32
		            UART_handleInstancesReceivedDataFromZigbeeCoordinator();
                }
                break;

            case UART_FIFO_OVF: 
                ESP_LOGI(TAG_UART, "HW fifo overflow!");
                // If fifo overflow happened, you should consider adding flow control for your application.
                // The ISR has already reset the rx FIFO,
                // As an example, we directly flush the rx buffer here in order to read more data.
                uart_flush_input(UART_NUM_2);
                xQueueReset(pQueueESP32);
                break;

            case UART_BUFFER_FULL:
                ESP_LOGI(TAG_UART, "Ring buffer full!");
                // If buffer full happened, you should consider increasing your buffer size
                // As an example, we directly flush the rx buffer here in order to read more data.
                uart_flush_input(UART_NUM_2);
                xQueueReset(pQueueESP32);
                break;

            case UART_BREAK:
                ESP_LOGI(TAG_UART, "UART rx break!");
                break;

            case UART_PARITY_ERR:
                ESP_LOGI(TAG_UART, "UART parity error!");
                break;

            case UART_FRAME_ERR:
                ESP_LOGI(TAG_UART, "UART frame error!");
                break;

            default:
                ESP_LOGI(TAG_UART, "UART event type: %d!", RXEventUART.type);
                break;
            }
        }
    }
    free(receivedPayload);
    receivedPayload = NULL;
    vTaskDelete(NULL);
}

/*
 * @func UART_handleInstancesReceivedDataFromZigbeeCoordinator
 * @brief To handle the instances of received data from ZigbeeCoordinator
 * @param None
 * @retval None
 */
void UART_handleInstancesReceivedDataFromZigbeeCoordinator(void)
{
		// Variable to get the state of received data
	UART_stateReceivedData_e receivedDataState;
	receivedDataState = UART_handleDataInQueue();

		// Process appropriately with the state of received data
	switch(receivedDataState){
		case UART_NoReceivedData:
            ESP_LOGI(TAG_UART, "UART_NoReceivedData !");
			break;

		case UART_ValidReceivedData:
            handleReceivedDataFromZC(&dataSendByZC[1]);
			break;

		case UART_InvalidReceivedData:
            ESP_LOGI(TAG_UART, "UART_InvalidReceivedData !");
			break;

		default:
			break;
	}
}

/*
 * @func UART_handleDataInQueue
 * @brief To get data from the queue buffer & process it
 * @param None
 * @retval The state of received data
 */
UART_stateReceivedData_e UART_handleDataInQueue(void)
{
		// Variable
	uint8_t dataQueue;													        // To get data from queue buffer
	UART_stateReceivedData_e stateReceivedData = UART_NoReceivedData;			// The state of received data

		// Get data from queue buffer & process it
	while((QUEUE_IsDataBufferEmpty(pQueue) != true) && (stateReceivedData == UART_NoReceivedData)){
		QUEUE_popDataFromDataBuffer(pQueue, &dataQueue);
		switch(typeReceivedByte){
			case UART_ByteStart:
				if(dataQueue == BYTE_START){
					indexArray = 0;
					calculateXORbyte = BYTE_CXOR;
					typeReceivedByte = UART_BytesData;
				} else{
					stateReceivedData = UART_InvalidReceivedData;
				}
				break;

			case UART_BytesData:
				if(indexArray < DIMENSION_ARRAY){
					dataSendByZC[indexArray] = dataQueue;
					if(indexArray > 0){
						calculateXORbyte ^= dataQueue;
					}
					if(++indexArray == *dataSendByZC){
						typeReceivedByte = UART_ByteCXOR;
					}
				} else{
					typeReceivedByte = UART_ByteStart;
					stateReceivedData = UART_InvalidReceivedData;
				}
				break;

			case UART_ByteCXOR:
				if(dataQueue == calculateXORbyte){
					stateReceivedData = UART_ValidReceivedData;
				} else{
					stateReceivedData = UART_InvalidReceivedData;
				}
                typeReceivedByte = UART_ByteStart;
                break;

			default:
				typeReceivedByte = UART_ByteStart;
				break;
		}
	}

	return stateReceivedData;
}

            /*---------- All functions to transmit data via UART ----------*/

/*
 * @func UART_GenerateTransmissionFrame
 * @brief To build the UART transmission frame
 * @param
 * 		  deviceNetworkID - The network address of device to control
 * 		  controlEndpoint - The endpoint of device to control
 * 		  controlCommandID - To identify control command
 * 		  commandType - The type of control command
 * 		  controlData - The control data to send for device
 * 		  lengthControlData - The length of control data to send for device
 * @retval None
 */
void UART_GenerateTransmissionFrame(uint16_t deviceNetworkID,
									uint8_t controlEndpoint,
									UART_commandID_e controlCommandID,
									UART_typeCommand_e commandType,
									uint8_t *controlData,
									uint8_t lengthControlData)
{
		// Declare & reset the array "transmissionPayload" that is transmit to Zigbee Coordinator
	memset(transmissionPayload, 0 , 16);

		// Variable to get the CXOR byte of UART transmission frame
	uint8_t byteXOR;

		// Build the array "transmissionPayload"
	*transmissionPayload = BYTE_START;
	*(transmissionPayload + 1) = lengthControlData + 7;
	*(transmissionPayload + 2) = deviceNetworkID & 0xFF;
	*(transmissionPayload + 3) = (deviceNetworkID >> 8) & 0xFF;
	*(transmissionPayload + 4) = controlEndpoint;
	*(transmissionPayload + 5) = controlCommandID;
	*(transmissionPayload + 6) = commandType;
	*(transmissionPayload + 7) = lengthControlData;
	for(uint8_t i = 0; i < lengthControlData; i++){
		*(transmissionPayload + 8 + i) = *(controlData + i);
	}
	byteXOR = UART_calculateXORbyteTransmissionFrame(transmissionPayload, (lengthControlData + 6));
	*(transmissionPayload + 8 + lengthControlData) = byteXOR;
    printf("Control commandID: %d\n", controlCommandID);
}

/*
 * @func UART_calculateXORbyteTransmissionFrame
 * @brief To calculate the CXOR byte of UART transmission frame
 * @param
 * 		  payloadToTransmit - UART transmission frame
 * 		  lengthPayload - The length of UART transmission frame
 * @retval The CXOR byte of UART transmission frame
 */
uint8_t UART_calculateXORbyteTransmissionFrame(uint8_t *payloadToTransmit, uint8_t lengthPayload)
{
		// Variable to calculate the CXOR byte of UART transmission frame
	uint8_t XORbyte = BYTE_CXOR;

		// Calculate the CXOR byte of UART transmission frame
	for(uint8_t i = 0; i < lengthPayload; i++){
		XORbyte ^= *(payloadToTransmit + i + 2);
	}

	return XORbyte;
}

/*
 * @func UART_createZigbeeNetwork
 * @brief To create the Zigbee network
 * @param
 * 		  valueDeviceNetworkID - The network address of device to control
 * 		  valueControlEndpoint - The endpoint of device to control
 * @retval None
 */
void UART_createZigbeeNetwork(uint16_t valueDeviceNetworkID, uint8_t valueControlEndpoint)
{
	UART_GenerateTransmissionFrame(valueDeviceNetworkID,
								   valueControlEndpoint,
								   UART_CreateZigbeeNetwork,
								   UART_TypeSet,
								   NULL,
								   0);
    uart_write_bytes(UART_NUM_2, 
                     (const uint8_t*)transmissionPayload, 
                     transmissionPayload[1] + 2);
}

/*
 * @func UART_openZigbeeNetwork
 * @brief To open the Zigbee network
 * @param
 * 		  valueDeviceNetworkID - The network address of device to control
 * 		  valueControlEndpoint - The endpoint of device to control
 * @retval None
 */
void UART_openZigbeeNetwork(uint16_t valueDeviceNetworkID, uint8_t valueControlEndpoint)
{
	UART_GenerateTransmissionFrame(valueDeviceNetworkID,
								   valueControlEndpoint,
								   UART_OpenZigbeeNetwork,
								   UART_TypeSet,
								   NULL,
								   0);
    uart_write_bytes(UART_NUM_2, 
                     (const uint8_t*)transmissionPayload, 
                     transmissionPayload[1] + 2);
}

/*
 * @func UART_closeZigbeeNetwork
 * @brief To close the Zigbee network
 * @param
 * 		  valueDeviceNetworkID - The network address of device to control
 * 		  valueControlEndpoint - The endpoint of device to control
 * @retval None
 */
void UART_closeZigbeeNetwork(uint16_t valueDeviceNetworkID, uint8_t valueControlEndpoint)
{
	UART_GenerateTransmissionFrame(valueDeviceNetworkID,
								   valueControlEndpoint,
								   UART_CloseZigbeeNetwork,
								   UART_TypeSet,
								   NULL,
								   0);
    uart_write_bytes(UART_NUM_2, 
                     (const uint8_t*)transmissionPayload, 
                     transmissionPayload[1] + 2);
}

/*
 * @func UART_removeStairSwitchInZigbeeNetwork
 * @brief To remove all stair switches in Zigbee network
 * @param
 * 		  valueDeviceNetworkID - The network address of device to control
 * 		  valueControlEndpoint - The endpoint of device to control
 *        deviceType - The type of device to control
 * @retval None
 */
void UART_removeStairSwitchInZigbeeNetwork(uint16_t valueDeviceNetworkID, 
                                           uint8_t valueControlEndpoint,
                                           MANAGE_typeDevice_e deviceType)
{
	UART_GenerateTransmissionFrame(valueDeviceNetworkID,
								   valueControlEndpoint,
								   UART_LeaveZigbeeNetwork,
								   UART_TypeSet,
								   (uint8_t *)&deviceType,
								   1);
    uart_write_bytes(UART_NUM_2, 
                     (const uint8_t*)transmissionPayload, 
                     transmissionPayload[1] + 2);
}

/*
 * @func UART_selectControlMode
 * @brief To select the control mode
 * @param controlMode - The control mode to control the stair light
 * @retval None
 */
void UART_selectControlMode(UART_controlMode_e controlMode)
{
	    // Variables
    uint16_t networkIdBelowSW;                 // Get networkID of below SW
    uint8_t controlEndpointBelowSW;            // Get endpoint to select the control mode

        // Send message to select the control mode
    for(uint8_t i = 0; i < MANAGE_infAllDevices.numberDevices; i++){
        if(MANAGE_infAllDevices.infDevice[i].typeDevice == MANAGE_BelowStairDevice){
            networkIdBelowSW = MANAGE_infAllDevices.infDevice[i].networkIdDevice;
            for(uint8_t j = 0; j < MANAGE_infAllDevices.infDevice[i].numberEndpoints; j++){
                if(MANAGE_infAllDevices.infDevice[i].infEndpoint[j].typeEndpoint == MANAGE_ControlMode){
                    controlEndpointBelowSW = MANAGE_infAllDevices.infDevice[i].infEndpoint[j].valueEndpoint;
                    UART_GenerateTransmissionFrame(networkIdBelowSW,
								                   controlEndpointBelowSW,
								                   UART_LeaveZigbeeNetwork,
								                   UART_TypeSet,
								                   (uint8_t *)&controlMode,
								                   1);
                    uart_write_bytes(UART_NUM_2, 
                                     (const uint8_t*)transmissionPayload, 
                                     transmissionPayload[1] + 2);
                }
            }
        }
    }
}

/*
 * @func UART_controlStairLight
 * @brief To control the stair light
 * @param 
 *       deviceType - The type of device to control
 *       controlLight- Turn on/off the stair light
 * @retval None
 */
void UART_controlStairLight(MANAGE_typeDevice_e deviceType, UART_controlLight_e controlLight)
{
	    // Variables
    uint16_t networkIdSW;                 // Get networkID of SW
    uint8_t controlEndpointSW;            // Get endpoint to control the stair light

        // Send message to select the control mode
    for(uint8_t i = 0; i < MANAGE_infAllDevices.numberDevices; i++){
        if(MANAGE_infAllDevices.infDevice[i].typeDevice == deviceType){
            networkIdSW = MANAGE_infAllDevices.infDevice[i].networkIdDevice;
            for(uint8_t j = 0; j < MANAGE_infAllDevices.infDevice[i].numberEndpoints; j++){
                if(MANAGE_infAllDevices.infDevice[i].infEndpoint[j].typeEndpoint == MANAGE_OnOffLight){
                    controlEndpointSW = MANAGE_infAllDevices.infDevice[i].infEndpoint[j].valueEndpoint;
                    UART_GenerateTransmissionFrame(networkIdSW,
								                   controlEndpointSW,
								                   UART_LeaveZigbeeNetwork,
								                   UART_TypeSet,
								                   (uint8_t *)&controlLight,
								                   1);
                    uart_write_bytes(UART_NUM_2, 
                                     (const uint8_t*)transmissionPayload, 
                                     transmissionPayload[1] + 2);
                }
            }
        }
    }
}

    /*------------------------------- END THIS FILE -------------------------------*/