#include <time.h>
#ifdef LINUX
#include <header_for_linux_sleep_function.h>
#endif
#ifdef WINDOWS
#include <header_for_windows_sleep_function.h>
#endif
/**
*@file
*@brief  declaration of the timer module for the elevator  
*/ 


/**
*@brief uses a sleep function to wait three seconds
*@param timer_value the timer value in seconds
*@return the function always returns zero after \ p timer_value amount of seconds
*@warning this function uses different libraries depending if it's running on Linux or Windows
*/
int timer();
