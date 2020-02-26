#include "timer.h"
#include <stdio.h>

static time_t time_start;
static time_t time_now;
double timer_length = 3.00;

void timer_start(){

    time(&start);   

 }
 
 bool is_timer_finished(){
    time(&now);

    if(difftime(time_now,time_start)<timer_length)
    {
        time(&now);
        return 0;
    }
    else
    {        
        return 1;   
    }
 }


