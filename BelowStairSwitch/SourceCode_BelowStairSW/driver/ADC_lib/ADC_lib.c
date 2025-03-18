/*
 * ADC_lib.c
 *
 *  Created on: Dec 6, 2023
 *  Author: TRAN DOAN MANH
 *	Descriptor: To get value of light sensor
 */

	/*------------------------------- All libraries -------------------------------*/

#include "app/framework/include/af.h"
#include "math.h"
#include "em_iadc.h"
#include "em_cmu.h"
#include "source/driver/ADC_lib/ADC_lib.h"

	/*------------------------------- Initialize variables -------------------------------*/

	// Variables
static volatile uint32_t valueLightSensor = 0;

	/*------------------------------- All functions -------------------------------*/

/*
 * @func ADC_initializeADC0
 * @brief To configure all specifications of ADC protocol
 * @param None
 * @retval None
 */
void ADC_initializeADC0(void)
{
		// Enable clocks to the ADC0 and GPIO that is used for ADC0
	CMU_ClockEnable(cmuClock_IADC0, true);
	CMU_ClockEnable(cmuClock_GPIO, true);

		/*----------------- Configuring I2C1 -------------------*/

		// Reset IADC to reset configuration in case it has been modified by other code
	IADC_reset(IADC0);
		// Declare initialization structures of IDAC
	IADC_Init_t 		init = IADC_INIT_DEFAULT;
	IADC_AllConfigs_t 	initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
	IADC_InitSingle_t 	initSingle = IADC_INITSINGLE_DEFAULT;
	IADC_SingleInput_t 	initSingleInput = IADC_SINGLEINPUT_DEFAULT;
		//---------- Modify init configuration ----------
	init.warmup = iadcWarmupKeepWarm;
		//---------- Modify initAllConfigs configuration ---------
	initAllConfigs.configs[0].reference = iadcCfgReferenceVddx;
	initAllConfigs.configs[0].vRef = 3000;
	initAllConfigs.configs[0].osrHighSpeed = iadcCfgOsrHighSpeed32x;
		//---------- Modify initSingle configuration ----------
	initSingle.dataValidLevel = _IADC_SINGLEFIFOCFG_DVL_VALID1;
		//---------- Modify clock division configuration ----------
	CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_FSRCO);						// Select clock for IADC, FSRCO - 20MHz
	init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0);	// Set the HFSCLK prescale value
	initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0,	// Divides CLK_SRC_ADC to set the CLK_ADC frequency
	                                             	 	 	 	 	   CLK_ADC_FREQ,
																	   0,
																	   iadcCfgModeNormal,
																	   init.srcClkPrescale);
		//---------- To use GPIO as an input for the IADC ----------
	GPIO->CDBUSALLOC |= GPIO_CDBUSALLOC_CDODD0_ADC0;							// Allocate the analog bus for ADC0 inputs
	initSingleInput.posInput   = iadcPosInputPortCPin5;							// Assign ports and pins to positive and negative inputs in signed-end mode
	initSingleInput.negInput   = iadcNegInputGnd;
		//---------- Initialize IADC ----------
	IADC_init(IADC0, &init, &initAllConfigs);									// Initialize the IADC
	IADC_initSingle(IADC0, &initSingle, &initSingleInput);						// Initialize the Single conversion inputs
}

/*
 * @func ADC_readLightSensorValue
 * @brief To read the value of light sensor
 * @param None
 * @retval Light value
 */
uint32_t ADC_readLightSensorValue(void)
{
    	// Initialize conversion variable
	volatile uint32_t valueVoltage = 0;
	volatile uint32_t valueResistance = 0;

		// Start IADC conversion
    IADC_command(IADC0, iadcCmdStartSingle);

    	// Wait for conversion to be complete
    while((IADC0->STATUS & (_IADC_STATUS_CONVERTING_MASK | _IADC_STATUS_SINGLEFIFODV_MASK)) != IADC_STATUS_SINGLEFIFODV);

    	// Get ADC result
    valueVoltage = IADC_pullSingleFifoResult(IADC0).data;
    valueResistance= 10000 * (3300 - valueVoltage)/valueVoltage;
    valueLightSensor = (uint32_t)(336 * pow(10, 5) * pow(valueResistance, -1.4));

    return valueLightSensor;
}

	/*------------------------------- END THIS FILE -------------------------------*/
