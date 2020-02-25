#include "timer.h"
#include <stdio.h>

static time_t start;
static time_t now;
double timer_length = 3.00;

void timer_start(){

    time(&start);   

 }
 
 bool is_timer_finished(){
    time(&now);

    if(difftime(now,start)<timer_length)
    {
        time(&now);
        return 0;
    }
    else
    {        
        return 1;   
    }
 }


