#include <time.h>
/*
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

/*
/**
*@brief uses a sleep function to wait three seconds
*@param timer_value the timer value in seconds
*@return the function always returns zero after \ p timer_value amount of seconds
*@warning this function uses different libraries depending if it's running on Linux or Windows
*/
/*
int timer();

*/


time_t timer_start(){

    time_t start;
    time(&start);   

    return start;
 }

 bool is_timer_finished(time_t start){

     double timer_length = 3;
     time_t now;
     time(&now);

    while(difftime(now,start)<timer_length)
    {
        //printf("start: %s", ctime(&start));
        //printf("now: %s", ctime(& now));

        //printf("difftime: %0.2f \n", (float)difftime(now,start));
        time(&now);
    }

    return 1;
 }


