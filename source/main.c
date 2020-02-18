#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>

#include "hardware.h"
#include "queue.h"

typedef enum {
    ELEVATOR_IDLE,      // Waiting for Orders
    ELEVATOR_AT_FLOOR,  // Door open at floor
    ELEVATOR_MOVING,    // Moving to request
    ELEVATOR_STOPPED    // Stop button pressed
} ElevatorState;

ElevatorState elevator_state = ELEVATOR_IDLE; // Ingen ordre

int main(){
    
    int elevator_floor; // Current (last known) elevator floor
    int next_request;   // The next floor request in the queue
    
    hardware_init();
    
    while(1){
        // READING FROM HARDWARE:
        for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++){
            // Check internal orders
            if(hardware_read_order(f, HARDWARE_ORDER_INSIDE)){
                queue_set_request(f, QUEUE_ORDER_INSIDE);
                hardware_command_order_light(f, HARDWARE_ORDER_INSIDE, 1);
            }
            // Check orders up
            if(hardware_read_order(f, HARDWARE_ORDER_UP)){
                queue_set_request(f, QUEUE_ORDER_UP);
                hardware_command_order_light(f, HARDWARE_ORDER_UP, 1);
            }
            // Check orders down
            if(hardware_read_order(f, HARDWARE_ORDER_DOWN)){
                queue_set_request(f, QUEUE_ORDER_DOWN);
                hardware_command_order_light(f, HARDWARE_ORDER_DOWN, 1);
            }
            // Check floor
            if(hardware_read_floor_sensor(f)){
                hardware_command_floor_indicator_on(f);
                elevator_floor = f;
            }
        
        }
        
        
        // CONTROLLING HARDWARE (FSM):
        
        next_request = queue_read_next();
        
        switch(elevator_state){
            case ELEVATOR_IDLE :
                // Action:
                    // ??
                // Transition:
                    // if request at same floor --> STOPPED
                    // if request at diff floor --> MOVING
            case ELEVATOR_MOVING :
            // Action:
                // move
            // Transition:
                // if reached floor with request --> STOPPED
            case ELEVATOR_AT_FLOOR :
            // Action:
                // wait for timer end, then:
                // close door, look for new orders
            // Transition:
                // always goto IDLE, or directly to new request?
            case ELEVATOR_STOPPED :
            // Action:
                // remove all requests,
                // wait until 3 seconds after stop button released,
                // don't take requests
            // Transition:
                // goto IDLE, or directly to new request?
            
        } // End switch()
        
        
    } // End while()
} // End main()