#include "timer.h"

static time_t p_start;
static time_t p_now;
double timer_length = 3;

void timer_start(){

    time(&p_start);   

 }
 
 bool is_timer_finished(){
    time(&p_now);

    if(difftime(p_now,p_start)<timer_length)
    {
        time(&p_now);
        return 0;
    }
    else
    {        
        return 1;   
    }
 }


