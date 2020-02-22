#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>

#include "hardware.h"
#include "queue.h"
#include "timer.h"

// The four elevator states resprecented as constants:
typedef enum {
    ELEVATOR_IDLE,      // Waiting for orders
    ELEVATOR_DOOR_OPEN, // Door open at floor
    ELEVATOR_MOVING,    // Moving to request
    ELEVATOR_STOPPED    // Stop button pressed
} ElevatorState;



int main(){
    
    // Flags and control variables used in the FSM
    bool elevator_at_floor; // Is the elevator at a floor
    int elevator_floor; // Current (last known) elevator floor
    int next_request; // The next floor request in the queue
    HardwareMovement elevator_dir; // Used to check current/last direction
    ElevatorState elevator_state = ELEVATOR_IDLE; // Current state
    bool new_elevator_state = true; // Controls state initialization
    // Each state must set and reset this variable upon transition
    
    // Pointers to time variables used to control the door
    time_t * p_start = (time_t*) malloc(sizeof(time_t));; // Start of timer
    time_t * p_now = (time_t*) malloc(sizeof(time_t));; // Check passed time
    
    // Initialization process, gets elevator to a defined state (a floor):
    hardware_init();
    elevator_at_floor = false; // Assuming unknown floor
    elevator_dir = HARDWARE_MOVEMENT_STOP;
    while(!elevator_at_floor){
        for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++){
            if(hardware_read_floor_sensor(f)){
                elevator_at_floor = true;
                hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                elevator_dir = HARDWARE_MOVEMENT_STOP;
            }
        }
        if((!elevator_at_floor) && (elevator_dir == HARDWARE_MOVEMENT_STOP)){
            hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
            elevator_dir = HARDWARE_MOVEMENT_DOWN;
        }
    }
    
    
    while(1){ // ELEVATOR PROGRAM START

        // READING FROM HARDWARE:
        for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++){
            
            // Check stop button. Forces state into stopped.
            if(hardware_read_stop_signal()){
                if(elevator_state != ELEVATOR_STOPPED){
                    new_elevator_state = true;
                    elevator_state = ELEVATOR_STOPPED;
                }
                break;
            }
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
                elevator_at_floor = true;
            }
        } // End read loop
        
        if(!hardware_read_floor_sensor(elevator_floor)){
            elevator_at_floor = false; // no longer at floor
        }
        
        // CONTROLLING HARDWARE (FSM):
        
        // Each state has an initialization, a transition, and sometimes an action.
        // Initialization is run once upon entry into the state.
        // Action is run continuously when in the state.
        // Transition is run once, ends state and decides next state.
        
        next_request = queue_read_next();
        
        switch(elevator_state){
            case ELEVATOR_IDLE :
            // Init:
                if(new_elevator_state){
                    new_elevator_state = false;
                }
            // Transition:
                if(next_request != -1){ // There is a request
                    
                    // Request at different floor
					if(next_request != elevator_floor){
						elevator_state = ELEVATOR_MOVING;
						new_elevator_state = true;
					}
                    // Request at current floor
                    if(next_request == elevator_floor){
                        
                        // Elevator still at floor, just open door
                        if(elevator_at_floor){
							elevator_state = ELEVATOR_DOOR_OPEN;
							new_elevator_state = true;
						}
                        
						if(!elevator_at_floor){
                            // Special case; if elevator stopped between
                            // floors and then recalled to last floor.
                            // Uses memory of last elevator_dir to return.
                            
							// If went up last, go back down:
							if(elevator_dir == HARDWARE_MOVEMENT_UP){
								hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
							}
							// If went down last, go back up:
							if(elevator_dir == HARDWARE_MOVEMENT_DOWN){
								hardware_command_movement(HARDWARE_MOVEMENT_UP);
							}
							elevator_state = ELEVATOR_MOVING;
							new_elevator_state = false;
							// False because motor is already started
						}
                    }
                }
                break;

            case ELEVATOR_MOVING :
            // Init:
                if(new_elevator_state){
                    if(next_request > elevator_floor){ // Request above
                        elevator_dir = HARDWARE_MOVEMENT_UP;
                        hardware_command_movement(HARDWARE_MOVEMENT_UP);
                    }
                    if(next_request < elevator_floor){ // Request below
                        elevator_dir = HARDWARE_MOVEMENT_DOWN;
                        hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
                    }
                    new_elevator_state = false;
                }
            // Transition:
                if(elevator_at_floor){ // Elevator reached a floor
                    if(elevator_dir == HARDWARE_MOVEMENT_UP){
                        if(queue_read_floor(elevator_floor, QUEUE_MOVEMENT_UP)){
                            elevator_dir = HARDWARE_MOVEMENT_STOP; // set stop
                        }
                    }
                    if(elevator_dir == HARDWARE_MOVEMENT_DOWN){
                        if(queue_read_floor(elevator_floor, QUEUE_MOVEMENT_DOWN)){
                            elevator_dir = HARDWARE_MOVEMENT_STOP; // set stop
                        }
                    }
                    if(elevator_floor == next_request){
                        elevator_dir = HARDWARE_MOVEMENT_STOP; // set stop
                    }
                    
                    if(elevator_dir == HARDWARE_MOVEMENT_STOP){ // handle stop
                        hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                        elevator_state = ELEVATOR_DOOR_OPEN;
                        new_elevator_state = true;
                    }
                    
                }
                break;
            
            case ELEVATOR_DOOR_OPEN :
            // Init:
                if(new_elevator_state){
					hardware_command_door_open(1);
                    timer_start(p_start); // Start door timer
                    new_elevator_state = false;
                }
            // Action:
                if(hardware_read_obstruction_signal()){
                    timer_start(p_start);
					// Reset door timer while obstruction switch is active
                }
            // Transition:
                if(is_timer_finished(p_start,p_now)){
                    
                    // Remove requests and turn off request lights
					hardware_command_order_light(elevator_floor, HARDWARE_ORDER_UP, 0);
					hardware_command_order_light(elevator_floor, HARDWARE_ORDER_INSIDE, 0);
                    hardware_command_order_light(elevator_floor, HARDWARE_ORDER_DOWN, 0);
                    queue_clear_floor(elevator_floor);
                    
                    hardware_command_door_open(0);
                    elevator_state = ELEVATOR_IDLE;
                    new_elevator_state = true;
                }
                break;
            
            case ELEVATOR_STOPPED :
            // Init:
                if(new_elevator_state){
                    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                    hardware_command_stop_light(1);
                    queue_clear_all_requests();

                    for(int i = 0; i<HARDWARE_NUMBER_OF_FLOORS; i++){
                        hardware_command_order_light(i, HARDWARE_ORDER_DOWN, 0);
                        hardware_command_order_light(i, HARDWARE_ORDER_UP, 0);
                        hardware_command_order_light(i, HARDWARE_ORDER_INSIDE, 0);
                    }

                    if(elevator_at_floor){
                        hardware_command_door_open(1);
                    }
                    new_elevator_state = false;
                }
            // Transition:
                if(!hardware_read_stop_signal()){
                    hardware_command_stop_light(0);

                    if(elevator_at_floor){ // open_door state will handle door
                        elevator_state = ELEVATOR_DOOR_OPEN;
                    }
                    if(!elevator_at_floor){ // no door, goto idle
                        elevator_state = ELEVATOR_IDLE;
                    }
					new_elevator_state = true;
                }
                break;
        } // End switch()
    } // End while()
    free_timer(p_start, p_now);
    return 0;
} // End main()
