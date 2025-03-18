/*
 * I2C_lib.c
 *
 *  Created on: May 20, 2024
 *  Author: TRAN DOAN MANH
 *  Descriptor: To get value of temp-hum sensor
 */

	/*------------------------------- All libraries -------------------------------*/

#include "app/framework/include/af.h"
#include "em_i2c.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "source/driver/I2C_lib/I2C_lib.h"

	/*------------------------------- Initialize function pointer, variable, array -------------------------------*/



	/*------------------------------- All functions -------------------------------*/

/*
 * @func I2C_initializeI2C0
 * @brief To configure all specifications of I2C protocol
 * @param None
 * @retval None
 */
void I2C_initializeI2C0(void)
{
		// Enable clocks to the ADC0 and GPIO that is used for I2C0
	CMU_ClockEnable(cmuClock_I2C0, true);
	CMU_ClockEnable(cmuClock_GPIO, true);

	/*----------------- Configuring pin 0, 1 of GPIOB that act as SCL, SDA of I2C0 -------------------*/

		// Configuring pin 0, 1 of GPIOB at open drain - PB0 (SCL) and PB1 (SDA)
	GPIO_PinModeSet(gpioPortB, 0, gpioModeWiredAndPullUpFilter, 1);
	GPIO_PinModeSet(gpioPortB, 1, gpioModeWiredAndPullUpFilter, 1);
		// Route I2C0 pins to GPIO
	GPIO->I2CROUTE[0].SCLROUTE = (GPIO->I2CROUTE[0].SCLROUTE & ~_GPIO_I2C_SCLROUTE_MASK)
	                        	 | (gpioPortB << _GPIO_I2C_SCLROUTE_PORT_SHIFT
	                             | (0 << _GPIO_I2C_SCLROUTE_PIN_SHIFT));
	GPIO->I2CROUTE[0].SDAROUTE = (GPIO->I2CROUTE[0].SDAROUTE & ~_GPIO_I2C_SDAROUTE_MASK)
	                             | (gpioPortB << _GPIO_I2C_SDAROUTE_PORT_SHIFT
	                             | (1 << _GPIO_I2C_SDAROUTE_PIN_SHIFT));
	GPIO->I2CROUTE[0].ROUTEEN = GPIO_I2C_ROUTEEN_SDAPEN | GPIO_I2C_ROUTEEN_SCLPEN;

	/*----------------- Configuring I2C0 -------------------*/

	I2C_Init_TypeDef i2cInit = I2C_INIT_DEFAULT;
	I2C_Init(I2C0, &i2cInit);
}

/*
 * @func I2C_readTempHumSensorValue
 * @brief To read the value of TEMPHUM sensor
 * @param
 *       addressSlaveI2C - The address of slave I2C
 *       tempHumRegister - The address of register TempHum
 *       *dataSensor - The sensor data
 * @retval None
 */
uint16_t I2C_readTempHumSensorValue(uint16_t addressSlaveI2C, uint8_t tempHumRegister, uint8_t *dataSensor)
{
		// Transfer structure
	I2C_TransferSeq_TypeDef i2cTransfer;
	I2C_TransferReturn_TypeDef resultTransmission;

		// Initialize I2C0 to communicate with the slave I2C
	i2cTransfer.addr          = addressSlaveI2C;
	i2cTransfer.flags         = I2C_FLAG_WRITE_READ;
	i2cTransfer.buf[0].data   = &tempHumRegister;
	i2cTransfer.buf[0].len    = 1;
	i2cTransfer.buf[1].data   = dataSensor;
	i2cTransfer.buf[1].len    = 2;

		// Communicate with the slave I2C
	resultTransmission = I2C_TransferInit(I2C0, &i2cTransfer);
	while(resultTransmission == i2cTransferInProgress){
		resultTransmission = I2C_Transfer(I2C0);
	}

		// Get the value of TEMPHUM sensor
	if(resultTransmission == i2cTransferDone){
		valueTempHumSensor = (dataSensor[0] << 8) | dataSensor[1];
	}

	return valueTempHumSensor;
}

/*
 * @func I2C_getTempValueAfterProcessing
 * @brief To get the temp value after processing
 * @param None
 * @retval The processed temp value
 */
uint8_t I2C_getTempValueAfterProcessing(void)
{
		// Variables to get the temp value
	static uint8_t processedTemp;
	static uint16_t noProcessedTemp;

		// To get & process the temperature value of sensor
	noProcessedTemp = I2C_readTempHumSensorValue(ADDRESS_SLAVE_I2C, ADDRESS_SLAVEI2C_TEMPREG, sensorData);
	processedTemp = (uint8_t) ((175.72 * noProcessedTemp)/65536 - 46.85);

	return processedTemp;
}

/*
 * @func I2C_getHumValueAfterProcessing
 * @brief To get the hum value after processing
 * @param None
 * @retval The processed hum value
 */
uint8_t I2C_getHumValueAfterProcessing(void)
{
		// Variables to get the hum value
	uint8_t processedHum;
	uint16_t noPrecessedHum;

		// To get & process the humidity value of sensor
	noPrecessedHum = I2C_readTempHumSensorValue(ADDRESS_SLAVE_I2C, ADDRESS_SLAVEI2C_HUMREG, sensorData);
	processedHum = (uint8_t) ((125 * noPrecessedHum)/65536 - 6);

	return processedHum;
}

	/*------------------------------- END THIS FILE -------------------------------*/

