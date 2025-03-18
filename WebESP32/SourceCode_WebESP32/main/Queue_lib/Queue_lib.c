/*
 * Queue_lib.c
 *
 *  Author: TRAN DOAN MANH
 *  Descriptor: To store data received from Zigbee Coordinator via UART protocol
 */

	/*------------------------------- All libraries -------------------------------*/

#include "stdbool.h"
#include "Queue_lib.h"

	/*------------------------------- All functions -------------------------------*/

/*
 * @func QUEUE_initializeQueue
 * @brief To initialize all indexes of data buffer
 * @param va_queuePointer - The pointer variable of structure "queue_s"
 * @retval None
 */
void QUEUE_initializeQueue(queue_p va_queuePointer)
{
	va_queuePointer->frontIndex = 0;
	va_queuePointer->rearIndex = -1;
	va_queuePointer->count = 0;
}

/*
 * @func QUEUE_IsBuffurEmpty
 * @brief To check whether the data buffer is empty
 * @param va_queuePointer - The pointer variable of structure "queue_s"
 * @retval True/False
 */
bool QUEUE_IsDataBufferEmpty(queue_p va_queuePointer)
{
	return (va_queuePointer->count == 0);
}

/*
 * @func QUEUE_IsDataBufferFull
 * @brief To check whether the data buffer is full
 * @param va_queuePointer - The pointer variable of structure "queue_s"
 * @retval True/False
 */
bool QUEUE_IsDataBufferFull(queue_p va_queuePointer)
{
	return (va_queuePointer->count == DIMENSION_DATABUFFER);
}

/*
 * @func QUEUE_pushDataIntoDataBuffer
 * @brief To push data into data buffer
 * @param
 * 		  va_queuePointer - The pointer variable of structure "queue_s"
 * 		  dataToPush - Data to push into data buffer
 * @retval True/False
 */
bool QUEUE_pushDataIntoDataBuffer(queue_p va_queuePointer, uint8_t *dataToPush)
{
		// Check data buffer is full
	if(!QUEUE_IsDataBufferFull(va_queuePointer)){
		va_queuePointer->dataBuffer[++va_queuePointer->rearIndex] = *dataToPush;
		va_queuePointer->count++;
		return true;
	}

		// The data buffer is full
	return false;
}

/*
 * @func QUEUE_popDataFromDataBuffer
 * @brief To get & remove data from data buffer
 * @param
 * 		  va_queuePointer - The pointer variable of structure "queue_s"
 * 		  get_data - A pointer variable to store data is gotten from data buffer
 * @retval True/False
 */
bool QUEUE_popDataFromDataBuffer(queue_p va_queuePointer, uint8_t *get_data)
{
		// Check data buffer is empty
	if(!QUEUE_IsDataBufferEmpty(va_queuePointer)){
        *get_data = va_queuePointer->dataBuffer[va_queuePointer->frontIndex];
        for (uint8_t i = va_queuePointer->frontIndex; i < va_queuePointer->count; i++){
        	va_queuePointer->dataBuffer[i] = va_queuePointer->dataBuffer[i+1];
        }
        va_queuePointer->rearIndex--;
        va_queuePointer->count--;
        return true;
    }

		// The data buffer is empty
    return false;
}

	/*------------------------------- END THIS FILE -------------------------------*/