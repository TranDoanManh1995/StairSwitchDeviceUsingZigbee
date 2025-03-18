/*
 * Queue_lib.h
 *
 *  Author: TRAN DOAN MANH
 */

#ifndef _QUEUE_LIB_H_
#define _QUEUE_LIB_H_

	/*------------------------------- All libraries -------------------------------*/

#include "stdint.h"

	/*------------------------------- Declare macro, structure type & function prototypes -------------------------------*/

	// Macro
#define DIMENSION_DATABUFFER	255						// The dimension of data buffer in the Queue

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

#endif 

	/*------------------------------- END THIS FILE -------------------------------*/