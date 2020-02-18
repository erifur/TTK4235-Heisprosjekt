/**
*@file
*@brief  declaration of the timer module for the elevator  
*/

#include <time.h>


/**
*@brief sets the current time to the start-time for the timer
*@param start current time
*@return
*/
time_t timer_start();

/**
*@brief calculates the difference in time between the start time
* /// set by {@link timer_start()} 
*@param timer_length the time between current time and "now"-time
*@return when the function is finished it retuns 0
*/

bool is_timer_finished();
