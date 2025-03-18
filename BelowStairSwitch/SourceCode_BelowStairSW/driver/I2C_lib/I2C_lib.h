/*
 * I2C_lib.h
 *
 *  Created on: May 20, 2024
 *      Author: TRAN DOAN MANH
 */

#ifndef SOURCE_DRIVER_I2C_LIB_I2C_LIB_H_
#define SOURCE_DRIVER_I2C_LIB_I2C_LIB_H_

	/*------------------------------- All libraries -------------------------------*/

#include "app/framework/include/af.h"

	/*------------------------------- Declare macros, variables, function prototypes -------------------------------*/

	// Macros
#define ADDRESS_SLAVE_I2C					0x80
#define ADDRESS_SLAVEI2C_TEMPREG			0xE3
#define ADDRESS_SLAVEI2C_HUMREG				0xE5

	// Variables
uint8_t sensorData[2];					// An array to store the received data from TEMPHUM sensor
uint16_t valueTempHumSensor;			// A variable to store the received data of TEMPHUM sensor

	// Function prototypes
void I2C_initializeI2C0(void);
uint16_t I2C_readTempHumSensorValue(uint16_t addressSlaveI2C, uint8_t tempHumRegister, uint8_t *dataSensor);
uint8_t I2C_getTempValueAfterProcessing(void);
uint8_t I2C_getHumValueAfterProcessing(void);

	/*------------------------------- END THIS FILE -------------------------------*/

#endif /* SOURCE_DRIVER_I2C_LIB_I2C_LIB_H_ */
