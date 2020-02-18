#include "timer.h"

void timer_start(time_t* p_start){

    time(&*p_start);   

 }

 void free_timer(time_t* p_start, time_t* p_now)
 {
     /*
     p_start=NULL;
     p_now=NULL;
     check if necessary
     */

    free(p_start);
    free(p_now);
 }
 
 bool is_timer_finished(time_t* p_start, time_t* p_now){

    double timer_length = 3;
    time(&*p_now);

    if(difftime(*p_now,*p_start)<timer_length)
    {
        time(&*p_now);
        return 0;
    }
    else
    {        
        return 1;   
    }
 }


