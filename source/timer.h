/**
*@file
*@brief  Declaration of the timer module for the elevator
*/

#include <time.h>
#include <stdbool.h>
#include <stdlib.h>


/**
*@brief Sets the current time to a static variable in the timer module, 
*which is used as reference to calculate time passed.
*/
void timer_start();

/**
*@brief Calculates the difference in seconds between the time when the timer was started and the time now. 
*The time now is stored in a static variable in the timer module.
*@return If the difference in time is less than the set timer length it will return 0.
*If the timer is finished, which will be the opposite case, it will return 1.
*/
 bool is_timer_finished();