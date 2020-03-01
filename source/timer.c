#include "timer.h"
#include <stdio.h>

static time_t time_start;
static time_t time_now;
static double timer_length = 3.00;

void timer_start(){

    time(&time_start);   

 }
 
 bool is_timer_finished(){
    time(&time_now);

    if(difftime(time_now,time_start)<timer_length)
    {
        time(&time_now);
        return 0;
    }
    else
    {        
        return 1;   
    }
 }


