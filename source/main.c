#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>

#include "hardware.h"
#include "queue.h"

typedef enum {
    ELEVATOR_IDLE,      // Waiting for orders
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
        // Idea: Actions are continuous in a state, transitions are
        // event-based, move between states, and happen only IF(...)
        
        next_request = queue_read_next();
        
        switch(elevator_state){
            case ELEVATOR_IDLE : // waiting for requests
                // Action:
                    // ??
                // Transition:
                    // if request at same floor --> STOPPED
                    // if request at diff floor --> MOVING, set dir
            case ELEVATOR_MOVING : // going to floor
            // Action:
                // move
            // Transition:
                // if reached floor with request --> STOPPED
            case ELEVATOR_AT_FLOOR : // doing request
            // Action:
                // wait for timer end, then:
                // check obstruction, close door, look for new orders
            // Transition:
                // always goto IDLE, or directly to new request?
            case ELEVATOR_STOPPED : // stop button
            // Action:
                // remove all requests, [open door],
                // wait until 3 seconds after stop button released,
                // don't take requests, [check obs., close door]
            // Transition:
                // goto IDLE meanwhile, or wait for new request?
        } // End switch()
        
        
    } // End while()
} // End main()