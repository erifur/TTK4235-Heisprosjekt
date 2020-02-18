#include "timer.h"

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
