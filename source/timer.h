/**
*@file
*@brief  declaration of the timer module for the elevator
*/

#include <time.h>
#include <stdbool.h>
#include <stdlib.h>


/**
*@brief sets the current time to the start-time for the timer
*@param start is set to current time at call of function
*/
void timer_start();

/**
*@brief calculates the difference in time between the start time
*set by {@link timer_start()}
*@param timer_length the time between current time and "now"-time
*@param start is the value set by {@link timer_start(time_t start)}
*@param now is continually set to current time
*@return if the difference between \p now and \p start is less than \p timer_length it will return 0,
* else it will return 1
*/
 bool is_timer_finished();