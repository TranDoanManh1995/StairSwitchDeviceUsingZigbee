/*
 * KALMAN_lib.h
 *
 *  Created on: Dec 6, 2023
 *      Author: TRAN DOAN MANH
 */

#ifndef SOURCE_MID_FILTER_KALMAN_FILTER_KALMAN_H_
#define SOURCE_MID_FILTER_KALMAN_FILTER_KALMAN_H_

	/*------------------------------- All libraries -------------------------------*/



	/*------------------------------- Declare variables & function prototypes -------------------------------*/

	// Variables
float _err_measure;					// Uncertainty measurement
float _err_estimate;				// Uncertainty estimation
float _q;							// Process variance
float _current_estimate;			// Obtained value after filtering by Kalman filter
float _last_estimate;
float _kalman_gain;					// The gain of Kalman filter

	// Function prototypes
void KALMAN_initializeFilterKalman(float mea_e, float est_e, float q);
float KALMAN_updateEstimate(float mea);
void KALMAN_setMeasurementError(float mea_e);
void KALMAN_setEstimateError(float est_e);
void KALMAN_setProcessNoise(float q);
float KALMAN_getKalmanGain(void);
float KALMAN_getEstimateError(void);

	/*------------------------------- END THIS FILE -------------------------------*/

#endif /* SOURCE_MID_FILTER_KALMAN_FILTER_KALMAN_H_ */
