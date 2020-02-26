/**
*@file
*@brief  Declaration of the timer module for the elevator
*@param time_start The time at when the timer was started
*@param time_now The time right now
*/

#include <time.h>
#include <stdbool.h>
#include <stdlib.h>


/**
*@brief Sets the current \p time_start to the time right now, 
*which is used as reference to calculate time passed.
*/
void timer_start();

/**
*@brief Calculates the difference in seconds between  \p time_start and \p time_now
*@return If the difference between \p time_now and \p time_start is less than \p timer_length it will return 0,
*and update p_now to the current time, else it will return 1
*/
 bool is_timer_finished();