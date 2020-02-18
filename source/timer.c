#include "timer.h"

time_t timer_start(){

    time_t start;
    time(&start);   

    return start;
 }


void timer_start(time_t* p_start){

    time(&*p_start);   

 }

 void end_timer(time_t* p_start, time_t* p_now)
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
    }
    else
    {
        end_timers(p_start,p_now);
        return 1;   
    }

    return 0;
 }


