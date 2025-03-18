/*
 * KALMAN_lib.c
 *
 *  Created on: Dec 6, 2023
 *  Author: TRAN DOAN MANH
 *  Descriptor: To filter the noise of light sensor
 */

	/*------------------------------- All libraries -------------------------------*/

#include "math.h"
#include "source/mid/KALMAN_lib/KALMAN_lib.h"

	/*------------------------------- Initialize function pointer, variable, array -------------------------------*/



	/*------------------------------- All functions -------------------------------*/

/*
 * @func KALMAN_initializeFilterKalman
 * @brief To initialize the Kalman filter
 * @param
 * 		 mea_e - The value of uncertainty measurement
 * 		 est_e - The value of uncertainty estimation
 * 		 q - The value of process variance
 * @retval None
 */
void KALMAN_initializeFilterKalman(float mea_e, float est_e, float q)
{
  _err_measure = mea_e;
  _err_estimate = est_e;
  _q = q;
}

/*
 * @func KALMAN_updateEstimate
 * @brief To get value after filtering by Kalman filter
 * @param valueToFilter - The value to filter noise
 * @retval
 */
float KALMAN_updateEstimate(float valueToFilter)
{
  _kalman_gain = _err_estimate/(_err_estimate + _err_measure);
  _current_estimate = _last_estimate + _kalman_gain * (valueToFilter - _last_estimate);
  _err_estimate =  (1.0 - _kalman_gain)*_err_estimate + fabs(_last_estimate-_current_estimate)*_q;
  _last_estimate = _current_estimate;

  return _current_estimate;
}

/*
 * @func KALMAN_setMeasurementError
 * @brief To set the new value of measurement uncertainty
 * @param mea_e - The new value of uncertainty measurement
 * @retval None
 */
void KALMAN_setMeasurementError(float mea_e)
{
  _err_measure = mea_e;
}

/*
 * @func KALMAN_setEstimateError
 * @brief To set the new value of uncertainty estimation
 * @param est_e - The new value of uncertainty estimation
 * @retval None
 */
void KALMAN_setEstimateError(float est_e)
{
  _err_estimate = est_e;
}

/*
 * @func KALMAN_setProcessNoise
 * @brief To set the new value of process variance
 * @param q - The new value of process variance
 * @retval None
 */
void KALMAN_setProcessNoise(float q)
{
  _q = q;
}

/*
 * @func KALMAN_getKalmanGain
 * @brief To get the gain of Kalman filter
 * @param None
 * @retval The gain of Kalman filter
 */
float KALMAN_getKalmanGain(void)
{
  return _kalman_gain;
}

/*
 * @func KALMAN_getEstimateError
 * @brief To get the value of uncertainty estimation
 * @param None
 * @retval The value of uncertainty estimation
 */
float KALMAN_getEstimateError(void)
{
  return _err_estimate;
}

	/*------------------------------- END THIS FILE -------------------------------*/

