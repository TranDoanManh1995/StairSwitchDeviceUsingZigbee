/*
 * HTTP_lib.c
 *   
 *  Author: Tran Doan Manh
 *  Description: To handle all requests from Web and respond
 */

    /*------------------------------- All libraries -------------------------------*/

#include "esp_http_server.h"
#include "esp_log.h"
#include "string.h"
#include "HTTP_lib.h"
#include "NVSFlash_lib/NVSFlash_lib.h"
#include "UART_lib/UART_lib.h"

    /*------------------------------- Initialize variables -------------------------------*/

const char *TAG_HTTP_SERVER = "HTTP_SERVER";                                                            // Notice the program activities
const char *TAG_HTTP_POST = "HTTP_POST";
const char *TAG_HTTP_GET = "HTTP_GET";
httpd_handle_t stairSWServer = NULL;                                                                    // The stair switch server 
HTTP_webInterface_e webInterface = HTTP_LoginInterface;                                                 // Convert between the login and control interface 
extern char NVSFLASH_accountInf[20];                                                                    // Get the account information in NVS Flash
extern const uint8_t LoginInterface_html_start[] asm("_binary_LoginInterface_html_start");              // Embed LoginInterface.html into Flash
extern const uint8_t LoginInterface_html_end[] asm("_binary_LoginInterface_html_end");
extern const uint8_t ControlInterface_html_start[] asm("_binary_ControlInterface_html_start");          // Embed ControlInterface.html into Flash
extern const uint8_t ControlInterface_html_end[] asm("_binary_ControlInterface_html_end");
extern int networkStateUpperSW, networkStateBelowSW, controlMode, stateStairLight;                      // Get the reported data from Zigbee Coordinator
extern int valueTemp, valueHum, valueBrightness; 

    /************************************************** GET Method ************************************************/

    /*------------------------------ ESP32 sends login_Interface.html or control_Interface.html for Web ------------------------------*/

static esp_err_t HTTP_SmartHomeInterface(httpd_req_t *req)
{
    if(webInterface == HTTP_LoginInterface){
        httpd_resp_set_type(req, "text/html"); 
        httpd_resp_send(req, (const char *)LoginInterface_html_start, LoginInterface_html_end - LoginInterface_html_start);
    } else if(webInterface == HTTP_ControlInterface){
        httpd_resp_set_type(req, "text/html");
        httpd_resp_send(req, (const char *)ControlInterface_html_start, ControlInterface_html_end - ControlInterface_html_start);
        webInterface = HTTP_LoginInterface;
    }

    return ESP_OK;
}
static const httpd_uri_t GET_SmartHomeInterface = {
    .uri       = "/SmartHomeInterface",
    .method    = HTTP_GET,
    .handler   = HTTP_SmartHomeInterface,
    .user_ctx  = NULL
};

    /*------------------------------ ESP32 sends sensor data to update on Web ------------------------------*/

static esp_err_t HTTP_sensorData(httpd_req_t *req)
{
    static char sensorData[100];
    memset(sensorData, '\0', sizeof(sensorData));
    sprintf(sensorData, 
            "{\"tempValue\": \"%d\",\"humValue\": \"%d\",\"brightnessValue\": \"%d\"}", 
            valueTemp, valueHum, valueBrightness);
    httpd_resp_send(req, sensorData, strlen(sensorData));
    
    return ESP_OK;
}
static const httpd_uri_t GET_sensorData = {
    .uri       = "/getSensorData",
    .method    = HTTP_GET,
    .handler   = HTTP_sensorData,
    .user_ctx  = NULL
};

    /*------------------------------ ESP32 send status of stair light & control mode to update on Web ------------------------------*/

static esp_err_t HTTP_controlModeAndLightState(httpd_req_t *req)
{
    static char modeAndStatus[100];
    memset(modeAndStatus, '\0', sizeof(modeAndStatus));
    sprintf(modeAndStatus,
            "{\"stateLight\": \"%d\",\"networkStateUpperSW\": \"%d\",\"networkStateBelowSW\": \"%d\",\"controlMode\": \"%d\"}",
            stateStairLight, networkStateUpperSW, networkStateBelowSW, controlMode);
    httpd_resp_send(req, modeAndStatus, strlen(modeAndStatus));

    return ESP_OK;
}
static const httpd_uri_t GET_controlModeAndLightState = {
    .uri       = "/getControlModeAndLightState",
    .method    = HTTP_GET,
    .handler   = HTTP_controlModeAndLightState,
    .user_ctx  = NULL
};

    /********************************************* POST Method ******************************************/
   
    /*------------------------------ ESP32 get the login data is sent by Web ------------------------------*/

static esp_err_t HTTP_loginAccount(httpd_req_t *req)
{
        // Get that account information is sent from Web
    char loggedInAccount[20];
    httpd_req_recv(req, loggedInAccount, req->content_len);

        // To get the account information of user from NVS Flash
    NVSFLASH_getAccounInfFromNVSFlash();

        /* Check between account information and registration data 
        to allow or not allow user to access ControlInterface.html */
    if(strcmp(loggedInAccount, NVSFLASH_accountInf) == 0) {
        webInterface = HTTP_ControlInterface;
    }
        
        // End response         
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}
static const httpd_uri_t POST_loginAccount = {
    .uri       = "/loginAccount",
    .method    = HTTP_POST,
    .handler   = HTTP_loginAccount,
    .user_ctx  = NULL
};

    /*------------------------------ ESP32 get the registration data is sent by Web ------------------------------*/

static esp_err_t HTTP_registerAccount(httpd_req_t *req)
{
        // Get that account information is sent from Web
    char registeredAccount[20];
    httpd_req_recv(req, registeredAccount, req->content_len);

        // Store that account information of user into NVS Flash
    NVSFLASH_saveAccountInfInNVSFlash(registeredAccount);

        // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}
static const httpd_uri_t POST_registerAccount = {
    .uri       = "/registerAccount",
    .method    = HTTP_POST,
    .handler   = HTTP_registerAccount,
    .user_ctx  = NULL
};

    /*------------------------------ ESP32 receives the network creation command from Web & sends it for ZC ------------------------------*/

static esp_err_t HTTP_createZigbeeNetwork(httpd_req_t *req)
{
        // Send that control message for ZC device
    UART_createZigbeeNetwork(NETWORKID_ZC, ENDPOINT_ZC);

        // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}
static const httpd_uri_t POST_createZigbeeNetwork = {
    .uri       = "/createZigbeeNetwork",
    .method    = HTTP_POST,
    .handler   = HTTP_createZigbeeNetwork,
    .user_ctx  = NULL
};

    /*------------------------------ ESP32 receives command to open the Zigbee network from Web & sends it for ZC ------------------------------*/

static esp_err_t HTTP_openZigbeeNetwork(httpd_req_t *req)
{
        // Send that control message for ZC device
    UART_openZigbeeNetwork(NETWORKID_ZC, ENDPOINT_ZC);

        // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}
static const httpd_uri_t POST_openZigbeeNetwork = {
    .uri       = "/openZigbeeNetwork",
    .method    = HTTP_POST,
    .handler   = HTTP_openZigbeeNetwork,
    .user_ctx  = NULL
};

    /*------------------------------ ESP32 receives command to stop opening Zigbee network from Web & sends it for ZC ------------------------------*/

static esp_err_t HTTP_stopOpeningZigbeeNetwork(httpd_req_t *req)
{
        // Send that control message for ZC device
    UART_closeZigbeeNetwork(NETWORKID_ZC, ENDPOINT_ZC);

        // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}
static const httpd_uri_t POST_stopOpeningZigbeeNetwork = {
    .uri       = "/stopOpeningZigbeeNetwork",
    .method    = HTTP_POST,
    .handler   = HTTP_stopOpeningZigbeeNetwork,
    .user_ctx  = NULL
};

    /*------------------------------ ESP32 receives command to remove devices in Zigbee network from Web & sends it for ZC ------------------------------*/

static esp_err_t HTTP_allSWsLeaveNetwork(httpd_req_t *req)
{
        // Send that control message for ZC device
    UART_removeStairSwitchInZigbeeNetwork(NETWORKID_ZC, ENDPOINT_ZC, MANAGE_AllDevices);

        // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}
static const httpd_uri_t POST_allSWsLeaveNetwork = {
    .uri       = "/allSWsLeaveNetwork",
    .method    = HTTP_POST,
    .handler   = HTTP_allSWsLeaveNetwork,
    .user_ctx  = NULL
};

static esp_err_t HTTP_upperSWLeaveNetwork(httpd_req_t *req)
{
        // Send that control message for ZC device
    UART_removeStairSwitchInZigbeeNetwork(NETWORKID_ZC, ENDPOINT_ZC, MANAGE_UpperStairDevice);

        // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}
static const httpd_uri_t POST_upperSWLeaveNetwork = {
    .uri       = "/upperSWLeaveNetwork",
    .method    = HTTP_POST,
    .handler   = HTTP_upperSWLeaveNetwork,
    .user_ctx  = NULL
};

static esp_err_t HTTP_belowSWLeaveNetwork(httpd_req_t *req)
{
        // Send that control message for ZC device
    UART_removeStairSwitchInZigbeeNetwork(NETWORKID_ZC, ENDPOINT_ZC, MANAGE_BelowStairDevice);

        // Send that control message for ZC device
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}
static const httpd_uri_t POST_belowSWLeaveNetwork = {
    .uri       = "/belowSWLeaveNetwork",
    .method    = HTTP_POST,
    .handler   = HTTP_belowSWLeaveNetwork,
    .user_ctx  = NULL
};

    /*------------------------------ ESP32 receives command to select control mode from Web & sends it for ZC ------------------------------*/

static esp_err_t HTTP_selectAutoMode(httpd_req_t *req)
{
        // Send that control message for ZC device
    UART_selectControlMode(UART_ModeAuto);

        // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}
static const httpd_uri_t POST_selectAutoMode = {
    .uri       = "/selectAutoMode",
    .method    = HTTP_POST,
    .handler   = HTTP_selectAutoMode,
    .user_ctx  = NULL
};

static esp_err_t HTTP_selectManualMode(httpd_req_t *req)
{
        // Send that control message for ZC device
    UART_selectControlMode(UART_ModeManual);

        // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}
static const httpd_uri_t POST_selectManualMode = {
    .uri       = "/selectManualMode",
    .method    = HTTP_POST,
    .handler   = HTTP_selectManualMode,
    .user_ctx  = NULL
};

    /*------------------------------ ESP32 receives command to control stair light from Web & sends it for ZC ------------------------------*/

static esp_err_t HTTP_turnOnStairLightUpperSW(httpd_req_t *req)
{
        // Send that control message for ZC device
    UART_controlStairLight(MANAGE_UpperStairDevice, UART_TurnOnLight);

        // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}
static const httpd_uri_t POST_turnOnStairLightUpperSW = {
    .uri       = "/turnOnStairLightUpperSW",
    .method    = HTTP_POST,
    .handler   = HTTP_turnOnStairLightUpperSW,
    .user_ctx  = NULL
};

static esp_err_t HTTP_turnOffStairLightUpperSW(httpd_req_t *req)
{
        // Send that control message for ZC device
    UART_controlStairLight(MANAGE_UpperStairDevice, UART_TurnOffLight);

        // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}
static const httpd_uri_t POST_turnOffStairLightUpperSW = {
    .uri       = "/turnOffStairLightUpperSW",
    .method    = HTTP_POST,
    .handler   = HTTP_turnOffStairLightUpperSW,
    .user_ctx  = NULL
};

static esp_err_t HTTP_turnOnStairLightBelowSW(httpd_req_t *req)
{
        // Send that control message for ZC device
    UART_controlStairLight(MANAGE_BelowStairDevice, UART_TurnOnLight);

        // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}
static const httpd_uri_t POST_turnOnStairLightBelowSW = {
    .uri       = "/turnOnStairLightBelowSW",
    .method    = HTTP_POST,
    .handler   = HTTP_turnOnStairLightBelowSW,
    .user_ctx  = NULL
};

static esp_err_t HTTP_turnOffStairLightBelowSW(httpd_req_t *req)
{
        // Send that control message for ZC device
    UART_controlStairLight(MANAGE_BelowStairDevice, UART_TurnOffLight);

        // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}
static const httpd_uri_t POST_turnOffStairLightBelowSW = {
    .uri       = "/turnOffStairLightBelowSW",
    .method    = HTTP_POST,
    .handler   = HTTP_turnOffStairLightBelowSW,
    .user_ctx  = NULL
};

    /************************************************** Start & stop the stair light server ************************************************/

/*
 * @func HTTP_startStairSwitchServer
 * @brief To start the stair switch server
 * @param None
 * @retval None
 */
void HTTP_startStairSwitchServer(void)
{
        // Configure the method "HTTP"
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

        // Set URI handlers to receive the requests from Web & process
    ESP_LOGI(TAG_HTTP_SERVER, "Starting server to control Zigbee devices on port: '%d'!", config.server_port);
    if(httpd_start(&stairSWServer, &config) == ESP_OK){
        ESP_LOGI(TAG_HTTP_SERVER, "Registering URI handlers!");
        httpd_register_uri_handler(stairSWServer, &GET_SmartHomeInterface);
        httpd_register_uri_handler(stairSWServer, &GET_sensorData);
        httpd_register_uri_handler(stairSWServer, &GET_controlModeAndLightState);
        httpd_register_uri_handler(stairSWServer, &POST_loginAccount);
        httpd_register_uri_handler(stairSWServer, &POST_registerAccount);
        httpd_register_uri_handler(stairSWServer, &POST_createZigbeeNetwork);
        httpd_register_uri_handler(stairSWServer, &POST_openZigbeeNetwork);
        httpd_register_uri_handler(stairSWServer, &POST_stopOpeningZigbeeNetwork);
        httpd_register_uri_handler(stairSWServer, &POST_allSWsLeaveNetwork);
        httpd_register_uri_handler(stairSWServer, &POST_upperSWLeaveNetwork);
        httpd_register_uri_handler(stairSWServer, &POST_belowSWLeaveNetwork);
        httpd_register_uri_handler(stairSWServer, &POST_selectAutoMode);
        httpd_register_uri_handler(stairSWServer, &POST_selectManualMode);
        httpd_register_uri_handler(stairSWServer, &POST_turnOnStairLightUpperSW);
        httpd_register_uri_handler(stairSWServer, &POST_turnOffStairLightUpperSW);
        httpd_register_uri_handler(stairSWServer, &POST_turnOnStairLightBelowSW);
        httpd_register_uri_handler(stairSWServer, &POST_turnOffStairLightBelowSW);
    } else{
        ESP_LOGI(TAG_HTTP_SERVER, "Error starting server!");
    } 
}

/*
 * @func HTTP_stopStairSwitchServer
 * @brief To stop the stair switch server
 * @param None
 * @retval None
 */
void HTTP_stopStairSwitchServer(void)
{
    httpd_stop(stairSWServer);
}

    /*------------------------------- END THIS FILE -------------------------------*/

