/*
 * main.c
 *
 *  Author: Tran Doan Manh
 *  Description: The main program
*/

    /*------------------------------- All libraries -------------------------------*/

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "malloc.h"
#include "HTTP_lib/HTTP_lib.h"
#include "UART_lib/UART_lib.h"
#include "Queue_lib/Queue_lib.h"

    /*------------------------------- Decalre function prototype, macros & variables -------------------------------*/

    // Function prototype
void MAIN_handleReceivedDataFromZC(uint8_t* ZCreceivedData);

    // Macros
#define SSID_WIFI                       "TranQuocBao"               // SSID of Wifi to connect
#define PASSWORD_WIFI                   "tom260122"                 // Passwork of Wifi to connect 
#define NUMBER_RETRIES_MAXIMUM          5                           // The maximum number of retries to connect to Wifi when not connecting sucessfully
#define WIFI_CONNECTED_BIT              BIT0                        // Connect successfully to the Access Point with an IP address
#define WIFI_FAIL_BIT                   BIT1                        // Connect unsuccessfully after the maximum amount of retries

    // Variables
EventGroupHandle_t eventWifi;                                   // Catch Wifi events                         
const char *TAG_WIFI = "CONFIGURE_WIFI";                        // Display information when configuring Wifi 
const char *TAG_RECEIVED_DATA = "ZC_RECEIVED_DATA";             // Display data is sent by ZC        
uint8_t numberRetries = 0;                                      // Initialize the number of times to connect to Wifi
queue_p pQueue;                                                 // Queue pointer to use the programmer queue 
extern MANAGE_infAllDevices_s MANAGE_infAllDevices;             // Contain information of devices in Zigbee network
int networkStateUpperSW = 0, networkStateBelowSW = 0,           // Get the reported data from Zigbee Coordinator
    controlMode = 0, stateStairLight = 0;                           
int valueTemp = 0, valueHum = 0, valueBrightness = 0; 


    /*------------------------------- All funtions -------------------------------*/

/*
 * @func MAIN_handleWifiEvents
 * @brief To process TCP/IP and Wifi events
 * @param 
 *        arg - ...
 *        event_base - The base ID of the event to register the handler for event_id
 *        event_id - The ID of the event to register the handler for event data
 *        event_data - To get ID address
 * @retval None
 */
static void MAIN_handleWifiEvents(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START){
        esp_wifi_connect();                        
    } else if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED){
        if(numberRetries < NUMBER_RETRIES_MAXIMUM){
            esp_wifi_connect();
            numberRetries++;
            ESP_LOGI(TAG_WIFI, "Retry to connect to the AP!");
        } else{
            xEventGroupSetBits(eventWifi, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG_WIFI, "Connect to the AP fail!");
    } else if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP){
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG_WIFI, "Got IP:" IPSTR, IP2STR(&event->ip_info.ip));
        numberRetries = 0;
        xEventGroupSetBits(eventWifi, WIFI_CONNECTED_BIT);
    }
}

/*
 * @func MAIN_configureStationModeForESP32
 * @brief To configure the station mode for ESP32
 * @param None
 * @retval None
 */
void MAIN_configureStationModeForESP32(void)
{
        // FREETOS event group
    eventWifi = xEventGroupCreate();

        /*-------------------- Init Phase -----------------------*/

        // Initialize LightWeight TCP/IP and create LwIP task. It sends TCP/IP events to Event task
    ESP_ERROR_CHECK(esp_netif_init());

        // Initialize System Event task. It receives WIFI, TCP/IP events and send that events to Application task
    ESP_ERROR_CHECK(esp_event_loop_create_default());

        // Application task can register for a callback that listen for WiFi and TCP/IP events
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &MAIN_handleWifiEvents,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &MAIN_handleWifiEvents,
                                                        NULL,
                                                        &instance_got_ip));

        // Binding WiFi driver and TCP/IP stack (esp-netif or lwIP)
    esp_netif_create_default_wifi_sta();

        // Create WiFi driver task and initialise WiFi driver
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

        /*-------------------- Configure Phase -----------------------*/

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = SSID_WIFI,
            .password = PASSWORD_WIFI,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

        /*-------------------- Start Phase -----------------------*/

        /* Start WiFi and connect to an AP Wi-Fi driver posts WIFI_EVENT_STA_START 
        to the event task relays the WIFI_EVENT_STA_START to the application task */
    ESP_ERROR_CHECK(esp_wifi_start());                        
    ESP_LOGI(TAG_WIFI, "wifi_init_sta finished!");

        /* After initialising and starting WiFi, the application task 
        goes to Blocked state waiting for bits in event group to be set*/
    EventBits_t bits = xEventGroupWaitBits(eventWifi,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

        /* xEventGroupWaitBits() returns the bits before the call returned, 
        hence we can test which event actually happened. */
    if(bits & WIFI_CONNECTED_BIT){
        ESP_LOGI(TAG_WIFI, "Connected to SSID: %s, PASSWORD %s!", SSID_WIFI, PASSWORD_WIFI);
    } else if(bits & WIFI_FAIL_BIT){
        ESP_LOGI(TAG_WIFI, "Failed to connect to SSID: %s, PASSWORD: %s!", SSID_WIFI, PASSWORD_WIFI);
    } else{
        ESP_LOGE(TAG_WIFI, "Unexpected event!");
    }
}

/*
 * @func app_main
 * @brief main
 * @param None
 * @retval None
 */
void app_main(void)
{
        // Initialize NVS to store Wifi Configuration
    esp_err_t ret = nvs_flash_init();
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

        // To configure ESP32 at Station Mode
    ESP_LOGI(TAG_WIFI, "Configure the station mode for ESP32!");
    MAIN_configureStationModeForESP32();

        // Initialize the queue
    pQueue = (queue_p)calloc(1, sizeof(queue_t));  
    QUEUE_initializeQueue(pQueue);

        // Initialize USART2
    UART_initializeUart2(MAIN_handleReceivedDataFromZC);

        // Run server that control the Zigbee devices
    HTTP_startStairSwitchServer();
}

/*
 * @func MAIN_handleReceivedDataFromZC
 * @brief To process all received data from Zigbee Coordinator
 * @param ZCreceivedData
 * @retval None
 */
void MAIN_handleReceivedDataFromZC(uint8_t* ZCreceivedData)
{
        // Save the new device information
    MANAGE_saveInfDeviceToManage(ZCreceivedData);

        // Variables
	UART_frameUART_s *frameReportData = (UART_frameUART_s *)ZCreceivedData;                                              // Structure variable to process received data
    uint16_t valueDeviceNetworkID = frameReportData->lowByteNetworkID | (frameReportData->highByteNetworkID << 8);         // Get the networkID of device
    uint8_t deviceIndex = MANAGE_getDeviceIndex(valueDeviceNetworkID);                                                     // Get index of device in array
    MANAGE_typeDevice_e deviceType = MANAGER_getDeviceType(valueDeviceNetworkID);                                          // Get type of device
    uint8_t endpointIndex = MANAGE_infAllDevices.infDevice[deviceIndex].numberEndpoints;                               // Get location to save new endpoint of device in array "infEndpoint"
        
        // Handle received data from Zigbee Coordinator
    switch(deviceType) {
        case MANAGE_UpperStairDevice:
            if(frameReportData->commandID == UART_JoinZigbeeNetwork){
                printf("UpperSW joins Zigbee network\n");
                networkStateUpperSW = 1;
            } else if(frameReportData->commandID == UART_LeaveZigbeeNetwork){
                printf("UpperSW leaves Zigbee network\n");
                networkStateUpperSW = 0;
                MANAGE_removeInfDevice(deviceIndex);
            }
            break;

        case MANAGE_BelowStairDevice:
            if(frameReportData->commandID == UART_JoinZigbeeNetwork){
                printf("BelowSW joins Zigbee network\n");
                networkStateBelowSW = 1;
            } else if(frameReportData->commandID == UART_LeaveZigbeeNetwork){
                printf("BelowSW leaves Zigbee network\n");
                networkStateBelowSW = 0;
                MANAGE_removeInfDevice(deviceIndex);
            } else if(frameReportData->commandID == UART_ReportValueTemperature){
                valueTemp = frameReportData->reportData[0];
                printf("Temperature value: %d\n", valueTemp);
                static bool saveTempEndpoint = 1;
                if(saveTempEndpoint == 1){
                    MANAGE_saveEndpointDevice(deviceIndex, 
                                              endpointIndex, 
                                              frameReportData->valueEndpoint, 
                                              MANAGE_TempSensor);
                    saveTempEndpoint = 0;
                }
            } else if(frameReportData->commandID == UART_ReportValueHumidity){
                valueHum = frameReportData->reportData[0];
                printf("Humidity value: %d\n", valueHum);
                static bool saveHumEndpoint = 1;
                if(saveHumEndpoint == 1){
                    MANAGE_saveEndpointDevice(deviceIndex, 
                                              endpointIndex, 
                                              frameReportData->valueEndpoint, 
                                              MANAGE_HumSensor);
                    saveHumEndpoint = 0;
                }
            } else if(frameReportData->commandID == UART_ReportValueBrightness){
                valueBrightness = frameReportData->reportData[0] | (frameReportData->reportData[1] << 8);
                printf("Brightness value: %d\n", valueBrightness);
                static bool saveBrightnessEndpoint = 1;
                if(saveBrightnessEndpoint == 1){
                    MANAGE_saveEndpointDevice(deviceIndex, 
                                              endpointIndex, 
                                              frameReportData->valueEndpoint, 
                                              MANAGE_LightSensor);
                    saveBrightnessEndpoint = 0;
                }
            } else if(frameReportData->commandID == UART_ControlMode && frameReportData->typeCommand == UART_TypeReport){
                controlMode = frameReportData->reportData[0];
                printf("Control mode: %d\n", controlMode);
                static bool saveControlModeEndpoint = 1;
                if(saveControlModeEndpoint == 1){
                    MANAGE_saveEndpointDevice(deviceIndex, 
                                              endpointIndex, 
                                              frameReportData->valueEndpoint, 
                                              MANAGE_ControlMode);
                    saveControlModeEndpoint = 0;
                }
            } else if(frameReportData->commandID == UART_TurnOnOffLight && frameReportData->typeCommand == UART_TypeReport){
                stateStairLight = frameReportData->reportData[0];
                printf("State of stair light: %d\n", stateStairLight);
                static bool saveStairLightEndpoint = 1;
                if(saveStairLightEndpoint == 1){
                    MANAGE_saveEndpointDevice(deviceIndex, 
                                              endpointIndex, 
                                              frameReportData->valueEndpoint, 
                                              MANAGE_OnOffLight);
                    saveStairLightEndpoint = 0;
                }
            }
            break;

        default:
            break;
    }
}

    /*------------------------------- END THIS FILE -------------------------------*/
