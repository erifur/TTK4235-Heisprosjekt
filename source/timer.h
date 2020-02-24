/**
*@file
*@brief  declaration of the timer module for the elevator
*/

#include <time.h>
#include <stdbool.h>
#include <stdlib.h>


/**
*@brief sets the current time to the start-time for the timer
*@param[in] p_start is set to current time at call of function
**@warning this function is using a pointer that is dynamically allocated
*/
void timer_start();

/**
*@brief calculates the difference in time between the start time
*set by {@link timer_start()}
*@param timer_length the time between current time and "now"-time
*@param[in] p_start points to the value set by {@link timer_start(time_t* p_start)}
*@param[in] p_now is continually set to current time
*@return if the difference between \p p_now and \p p_start is less than \p timer_length it will return 0,
* else it will free the \p p_now and \p p_start and return 1
*@warning this function is using pointers that are dynamically allocated
*/
 bool is_timer_finished();