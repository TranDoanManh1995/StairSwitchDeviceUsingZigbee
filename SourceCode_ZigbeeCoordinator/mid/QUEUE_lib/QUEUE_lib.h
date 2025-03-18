/*
 * QUEUE_lib.h
 *
 *  Created on: Nov 12, 2024
 *      Author: TRAN DOAN MANH
 */

#ifndef SOURCE_MID_QUEUE_LIB_QUEUE_LIB_H_
#define SOURCE_MID_QUEUE_LIB_QUEUE_LIB_H_

	/*------------------------------- All libraries -------------------------------*/

#include "app/framework/include/af.h"
#include "stdbool.h"

	/*------------------------------- Declare macro, structure type & function prototypes -------------------------------*/

	// Macro
#define DIMENSION_DATABUFFER			255			// The dimension of data buffer in the Queue

	// Queue structure
typedef struct queue_s {
	uint8_t	dataBuffer[DIMENSION_DATABUFFER];  			// Data buffer to store data
	uint8_t frontIndex;          						// First index of data buffer
	uint8_t rearIndex;         							// Rear index of data buffer
	uint8_t count;										// Count the number of elements that buffer stored
}queue_t, *queue_p;

	// Function prototypes
void QUEUE_initializeQueue(queue_p va_queuePointer);
bool QUEUE_IsDataBufferEmpty(queue_p va_queuePointer);
bool QUEUE_IsDataBufferFull(queue_p va_queuePointer);
bool QUEUE_pushDataIntoDataBuffer(queue_p va_queuePointer, uint8_t *dataToPush);
bool QUEUE_popDataFromDataBuffer(queue_p va_queuePointer, uint8_t *getData);

	/*------------------------------- END THIS FILE -------------------------------*/

#endif /* SOURCE_MID_QUEUE_LIB_QUEUE_LIB_H_ */
