#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>

#include "hardware.h"
#include "queue.h"

typedef enum {
    ELEVATOR_IDLE,
    ELEVATOR_AT_FLOOR,
    ELEVATOR_MOVING,
    ELEVATOR_STOPPED
} ElevatorState;

static ElevatorState elevator_state = ELEVATOR_IDLE;



int main(){
    
    
    while(1){
        // READING:

        
        
        // WRITING (FSM):
        switch(elevator_state){
            case ELEVATOR_IDLE :
                
            case ELEVATOR_AT_FLOOR :
            
            case ELEVATOR_MOVING :
            
            case ELEVATOR_STOPPED :
            
            
            
        }
    }
}