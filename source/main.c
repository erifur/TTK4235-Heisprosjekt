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
    
    // Flags and control variables used in the FSM:
    bool elevator_at_floor; // Is the elevator at a floor
    int elevator_floor; // Current (last known) elevator floor
    int next_request; // The next floor request in the queue
    HardwareMovement elevator_dir; // Elevator direction of movement
    ElevatorState elevator_state = ELEVATOR_IDLE; // Current state
    bool new_elevator_state = true; // Controls state initialization
    // Each state must set and reset this variable upon transition
    
    // Timer pointers used to control the door:
    time_t * p_start; // Start of timer
    time_t * p_now; // Check passed time
    p_start = (time_t*) malloc(sizeof(time_t));
    p_now = (time_t*) malloc(sizeof(time_t));
    
    // Initialization process, gets elevator to a defined state (a floor):
    hardware_init();
    elevator_at_floor = false; // Assuming unknown floor
    elevator_dir = HARDWARE_MOVEMENT_STOP
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
            
            // Check stop button
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
            if(hardware_read_floor_sensor(f)){ // at a floor
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
        
        next_request = queue_read_next();
        
        switch(elevator_state){
            case ELEVATOR_IDLE : // waiting for requests
            // Init:
                if(new_elevator_state){
                    new_elevator_state = false;
					printf("Elevator Idle \n");
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
                            
							// if went up last, go back down:
							if(elevator_dir == HARDWARE_MOVEMENT_UP){
								hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
							}
							// if went down last, go back up:
							if(elevator_dir == HARDWARE_MOVEMENT_DOWN){
								hardware_command_movement(HARDWARE_MOVEMENT_UP);
							}
							elevator_state = ELEVATOR_MOVING;
							new_elevator_state = false;
							// false, because motor is already started
						}
                    }
                }
            break;

            case ELEVATOR_MOVING : // moving to floor
            // Init:
                if(new_elevator_state){
					printf("Elevator Moving \n");
                    printf("Serving request at floor %i \n", next_request);
                    if(next_request > elevator_floor){ // request above
                        elevator_dir = HARDWARE_MOVEMENT_UP;
                        hardware_command_movement(HARDWARE_MOVEMENT_UP);
						printf("Going up \n");
                    }
                    if(next_request < elevator_floor){ // request below
                        elevator_dir = HARDWARE_MOVEMENT_DOWN;
                        hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
						printf("Going down \n");
                        printf("From %i to %i \n", elevator_floor, next_request);
                    }
                    new_elevator_state = false;
                }
            // Transition:
                if(elevator_at_floor){ // Elevator reached a floor
                    if(elevator_dir == HARDWARE_MOVEMENT_UP){ // going up
                        if(queue_read_floor(elevator_floor, QUEUE_MOVEMENT_UP)){
                            elevator_dir = HARDWARE_MOVEMENT_STOP; // signal stop
                        }
                    }
                    if(elevator_dir == HARDWARE_MOVEMENT_DOWN){ // going down
                        if(queue_read_floor(elevator_floor, QUEUE_MOVEMENT_DOWN)){
                            elevator_dir = HARDWARE_MOVEMENT_STOP; // signal stop
                        }
                    }
                    if(elevator_floor == next_request){
                        hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                        elevator_state = ELEVATOR_DOOR_OPEN;
                    }
                    if(elevator_dir == HARDWARE_MOVEMENT_STOP){ // receive stop
                        hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                        elevator_state = ELEVATOR_DOOR_OPEN;
                        new_elevator_state = true;
                    }
                    
                }
            break;

            case ELEVATOR_DOOR_OPEN : // manages the door
            // Init:
                if(new_elevator_state){
					hardware_command_door_open(1); // open door

					printf("Elevator Door Open \n");
                    timer_start(p_start); // Start timer
                    new_elevator_state = false;
                }
            // Action:
                if(hardware_read_obstruction_signal()){
                    timer_start(p_start);
					// restart timer while obstruction active
                }
            // Transition:
                if(is_timer_finished(p_start,p_now)){
                    // turn off all request lights:
					hardware_command_order_light(elevator_floor, HARDWARE_ORDER_UP, 0);
					hardware_command_order_light(elevator_floor, HARDWARE_ORDER_INSIDE, 0);
                    hardware_command_order_light(elevator_floor, HARDWARE_ORDER_DOWN, 0);
                    queue_clear_floor(elevator_floor);

                    hardware_command_door_open(0); //turns off door light
                    elevator_state = ELEVATOR_IDLE;
                    printf("From %i to %i \n", elevator_floor, next_request);
                    new_elevator_state = true;
                }
            break;
            
            case ELEVATOR_STOPPED : // active while stop button is pressed
            // Init:
                if(new_elevator_state){
					printf("Elevator Stopped \n");
                    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                    hardware_command_stop_light(1); //turns on stoplight
                    queue_clear_all_requests();	// part of specs.

                    for(int i = 0; i<HARDWARE_NUMBER_OF_FLOORS; i++){
                        hardware_command_order_light(i, HARDWARE_ORDER_DOWN, 0);
                        hardware_command_order_light(i, HARDWARE_ORDER_UP, 0);
                        hardware_command_order_light(i, HARDWARE_ORDER_INSIDE, 0);
                        printf("turning of lights at floor %i \n", i);
                    }

                    if(elevator_at_floor){
                        hardware_command_door_open(1);
                    }
                    new_elevator_state = false;
                }
            // Transition:
                if(!hardware_read_stop_signal()){ // Button no longer active
                    hardware_command_stop_light(0); //turns off
                    printf("turn off stop light \n");

                    if(elevator_at_floor){ // state will manage door
                        elevator_state = ELEVATOR_DOOR_OPEN;
                    }
                    if(!elevator_at_floor){ // just goto idle.
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
