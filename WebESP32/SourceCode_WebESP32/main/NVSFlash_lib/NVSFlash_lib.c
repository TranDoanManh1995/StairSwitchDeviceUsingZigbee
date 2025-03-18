/*
 * NVSFlash_lib.c
 * 
 *  Author: Tran Doan Manh
 *  Description: To register the user account & log in
*/

    /*------------------------------- All libraries -------------------------------*/
    
#include "nvs_flash.h"
#include "string.h"

	/*------------------------------- Initialize variables -------------------------------*/

   // Variables
char NVSFLASH_accountInf[20];           // Get user account information
                          
	/*------------------------------- All funtions -------------------------------*/

/*
 * @func NVSFLASH_getAccounInfFromNVSFlash
 * @brief To get the user account information from NVS Flash to log in
 * @param None
 * @retval None
 */
void NVSFLASH_getAccounInfFromNVSFlash(void)
{
        // Initialize NVS Flash
    esp_err_t err = nvs_flash_init();
    if(err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND){
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

        // Open NVS Flash
    printf("\nOpening Non-Volatile Storage (NVS) handle... ");
    nvs_handle_t my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);

        // Check error when opening NVS_Flash
    if(err != ESP_OK){
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else{
        printf("Opening NVS Flash successfully!\n");
            // Get the length of data is stored at key "userAccount"
        size_t size_userAccountData = 0;
        nvs_get_str(my_handle, "userAccount", NULL, &size_userAccountData);                   
        char *getUserAccount = malloc(size_userAccountData); 
            // Read the user account from NVS Flash                                    
        err = nvs_get_str(my_handle, "userAccount", getUserAccount, &size_userAccountData);
        switch(err){
            case ESP_OK:
                printf("Read the user account from NVS Flash successfully!\n");
                strcpy(NVSFLASH_accountInf, getUserAccount);
                printf("The user account: %s\n", NVSFLASH_accountInf);
                break;

            case ESP_ERR_NVS_NOT_FOUND:
                printf("The value is not initialized yet!\n");
                break;

            default:
                printf("Error (%s) reading!\n", esp_err_to_name(err));
                break;
        }
            // Close NVS_Flash
        nvs_close(my_handle);
    }
}

/*
 * @func NVSFLASH_saveAccountInfInNVSFlash
 * @brief To save the account information of user into NVS Flash
 * @param registrationInf - The registration information to create an user account on Website
 * @retval None
 */
void NVSFLASH_saveAccountInfInNVSFlash(char *registrationInf)
{
        // Initialize NVS Flash
    esp_err_t err = nvs_flash_init();
    if(err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND){
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

        // Open NVS Flash
    printf("\nOpening Non-Volatile Storage (NVS) handle... ");
    nvs_handle_t my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);

        // Check error when opening NVS_Flash
    if(err != ESP_OK){
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else{
        printf("Opening NVS Flash successfully!\n");
            // Check the user account
        printf("The user account: %s\n", registrationInf);
            // Set value of user account for key "userAccount"
        err = nvs_set_str(my_handle, "userAccount", registrationInf);
        switch(err){
            case ESP_OK:
                printf("Setting user account for the key \"userAccount\" sucessfully!\n");
                printf("Committing updates in NVS ... ");
                    // Storing user account into NVS Flash
                err = nvs_commit(my_handle);
                printf((err != ESP_OK) ? "Storing into NVS Flash is failed!\n" : "Storing into NVS Flash successfully!\n");
                break;

            default:
                printf("Error (%s) writing!\n", esp_err_to_name(err));
                break;
        }
            // Close NVS Flash
        nvs_close(my_handle);
    }
}

    /*------------------------------- END THIS FILE -------------------------------*/
