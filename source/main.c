#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>

#include "hardware.h"
#include "queue.h"
#include "timer.h"

typedef enum {
    ELEVATOR_IDLE,      // Waiting for orders
    ELEVATOR_DOOR_OPEN, // Door open at floor
    ELEVATOR_MOVING,    // Moving to request
    ELEVATOR_STOPPED    // Stop button pressed
} ElevatorState;

ElevatorState elevator_state = ELEVATOR_IDLE; // No request

int main(){
    
    bool elevator_at_floor; // Is the elevator at a floor
    int elevator_floor; // Current (last known) elevator floor
    int next_request;   // The next floor request in the queue
    HardwareMovement elevator_dir;   // Elevator direction of movement
    
    time_t * p_start; //pointer for start of timer
    p_start = (time_t*) malloc(sizeof(time_t));

    time_t * p_now; //pointer for checking for passed time x amount of seconds
    p_now = (time_t*) malloc(sizeof(time_t));
    
    bool new_elevator_state = true; // Controls state initialization
    // Each state must set and reset this variable upon transition
    
    hardware_init();
    
    while(1){
        // READING FROM HARDWARE:
        for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++){
            if(hardware_read_stop_signal()){ // Take no requests
                elevator_state = ELEVATOR_STOPPED;
                new_elevator_state = true;
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
				// CHECK OBSTRUCTION SPECIFICATION; IS THIS CORRECT?:
                if(hardware_read_obstruction_signal()){
                    elevator_state = ELEVATOR_DOOR_OPEN;
                    new_elevator_state = true;
                }
            }
        } // end for()

        if(!hardware_read_floor_sensor(elevator_floor)){
            elevator_at_floor = false; // no longer at floor
        }
        
        // CONTROLLING HARDWARE (FSM):
        // Idea: Actions are continuous in a state, transitions are
        // event-based, move between states, and happen only IF(...)
        
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
					if(next_request != elevator_floor){ // request elsewhere
						elevator_state = ELEVATOR_MOVING;
						new_elevator_state = true;
					}
                    if(next_request == elevator_floor){ // request at floor
                        if(elevator_at_floor){ // exactly at floor, just open door
							elevator_state = ELEVATOR_DOOR_OPEN;
							new_elevator_state = true;
						}
						if(!elevator_at_floor){ // between floors
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
							
							// Note: don't change elevator_dir, in case stop
							// button is pressed again before floor reached.
						}
                    }
                }
                
            case ELEVATOR_MOVING : // moving to floor
            // Init:
                if(new_elevator_state){
					printf("Elevator Moving \n");
                    if(next_request > elevator_floor){ // request above
                        elevator_dir = HARDWARE_MOVEMENT_UP;
                        hardware_command_movement(HARDWARE_MOVEMENT_UP);
						printf("Going up \n");
                    }
                    if(next_request < elevator_floor){ // request below
                        elevator_dir = HARDWARE_MOVEMENT_DOWN;
                        hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
						printf("Going down \n");
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
                    if(elevator_dir == HARDWARE_MOVEMENT_STOP){ // receive stop
                        hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                        elevator_state = ELEVATOR_DOOR_OPEN;
                        new_elevator_state = true;
                    }
                }
                
            case ELEVATOR_DOOR_OPEN : // manages the door
            // Init:
                if(new_elevator_state){
					hardware_command_door_open(1); // open door

					printf("Elevator Door Open \n");

                    // turn off all request lights:
					hardware_command_order_light(current_floor, HARDWARE_ORDER_UP, 0);
					hardware_command_order_light(current_floor, HARDWARE_ORDER_INSIDE, 0);
                    hardware_command_order_light(current_floor, HARDWARE_ORDER_DOWN, 0);

                    queue_clear_floor(elevator_floor);
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
                    hardware_command_door_open(0); //turns off door light
                    elevator_state = ELEVATOR_IDLE;
                    new_elevator_state = true;
                }
                
            case ELEVATOR_STOPPED : // active while stop button is pressed
            // Init:
                if(new_elevator_state){
					printf("Elevator Stopped \n");
                    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                    hardware_command_stop_light(1); //turns on stoplight
                    queue_clear_all_requests();	// part of specs.
					
                    if(elevator_at_floor){
                        hardware_command_door_open(1);
                    }
                    new_elevator_state = false;
                }
            // Transition:
                if(!hardware_read_stop_signal()){ // Button no longer active
                    hardware_command_stop_light(0); //turns off 
                    printf("turn of stop light \n");

                    if(elevator_at_floor){ // state will manage door
                        elevator_state = ELEVATOR_DOOR_OPEN;
                    }
                    if(!elevator_at_floor){ // just goto idle.
                        elevator_state = ELEVATOR_IDLE;
                    }
					new_elevator_state = true;
                }
        
        
        } // End switch()
    } // End while()
    free_timer(p_start, p_now);
    return 0;
} // End main()
