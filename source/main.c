#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>

#include "hardware.h"
#include "queue.h"

typedef enum {
    ELEVATOR_IDLE,      // Waiting for orders
    ELEVATOR_DOOR_OPEN, // Door open at floor
    ELEVATOR_MOVING,    // Moving to request
    ELEVATOR_STOPPED    // Stop button pressed
} ElevatorState;

ElevatorState elevator_state = ELEVATOR_IDLE; // No request

static bool elevator_decide_stop(HardwareMovement elevator_dir){
    for(int f = 0; f<HARDWARE_NUMBER_OF_FLOORS; ++f){
        if(hardware_read_floor_sensor(f)){ // Elevator at floor
            if(elevator_dir == HARDWARE_MOVEMENT_UP){ // elevator going up
                if(queue_read_floor(f, QUEUE_MOVEMENT_UP)){ // relevant request
                    return true;
                }
            }
            if(elevator_dir == HARDWARE_MOVEMENT_DOWN){ // elevator going down
                if(queue_read_floor(f, QUEUE_MOVEMENT_DOWN)){ // relevant request
                    return true;
                }
            }
        }
    }
}


int main(){
    
    int elevator_floor; // Current (last known) elevator floor
    int next_request;   // The next floor request in the queue
    HardwareMovement elevator_dir;   // Elevator direction of movement
    
    bool new_elevator_state = true; // Controls state initialization
    // Each state must set and reset this variable upon transition
    
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
                    // none
                // Transition:
                if(next_request != 0){ // There is a request
                    if(next_request == elevator_floor){ // request at floor
                        hardware_command_door_open(1);
                        elevator_state = ELEVATOR_DOOR_OPEN;
                    }
                    else if(next_request > elevator_floor){ // request above
                        elevator_dir = HARDWARE_MOVEMENT_UP;
                        hardware_command_movement(HARDWARE_MOVEMENT_UP);
                        elevator_state = ELEVATOR_MOVING;
                    }
                    else if(next_request < elevator_floor){ // request below
                        elevator_dir = HARDWARE_MOVEMENT_DOWN;
                        hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
                        elevator_state = ELEVATOR_MOVING;
                    }
                }
            case ELEVATOR_MOVING : // moving to floor
            // Action:
                // move
            // Transition:
                if(elevator_decide_stop(elevator_dir, elevator_floor)){
                    elevator_dir = HARDWARE_MOVEMENT_STOP;
                    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                    queue_clear_floor(elevator_floor);
                    
                    //clear requests at floor, stop elevator, open door, change state
                }
                // if reached floor with request --> STOPPED
            case ELEVATOR_DOOR_OPEN : // doing request
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