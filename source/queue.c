/**
* @file
* @brief Implementation of queue module
*/
#include "queue.h"

static bool requests_cab[QUEUE_NUMBER_OF_FLOORS];
static bool requests_up[QUEUE_NUMBER_OF_FLOORS-1];
static bool requests_down[QUEUE_NUMBER_OF_FLOORS-1];
static int queue[QUEUE_NUMBER_OF_FLOORS];

/**
* @brief Adds a floor to the first available spot in the queue, if it is not already there.
* @param floor The floor to be added
*/
static void queue_add_floor(int floor){
    for (int i = 0; i<QUEUE_NUMBER_OF_FLOORS; ++i){
        if (queue[i] == -1){ // Add floor to first free spot, then return
            printf("Added to queue: %i\n", floor);
            queue[i] = floor;
            return;
        }
    }
    return;
}

void queue_set_request(int floor, QueueOrder order_type){
    switch(order_type){
        case QUEUE_ORDER_UP :
            requests_up[floor] = true;
            // index=floor-1 because index 0 = floor 1
            break;
        case QUEUE_ORDER_INSIDE :
            requests_cab[floor] = true;
            break;
        case QUEUE_ORDER_DOWN :
            requests_down[floor - 1] = true;
            //index=floor-2 because starts at floor 2
            break;
    }
    // Check if floor already in queue:
    for (int i = 0; i<QUEUE_NUMBER_OF_FLOORS; ++i){
        if (queue[i] == floor){ return; }   // Return if floor in queue
    }
    queue_add_floor(floor);    // If not, add to queue
}

void queue_clear_all_requests(){
    // Reset cab and queue
    for (int i = 0; i<QUEUE_NUMBER_OF_FLOORS; ++i){
        queue[i] = -1;
        requests_cab[i] = false;
    }
    // Reset hall orders, seperate because shorter arrays
    for (int j = 0; j<(QUEUE_NUMBER_OF_FLOORS-1); ++j){
        requests_up[j] = false;
        requests_down[j] = false;
    }
    return;
}

/**
* @brief Ensures that if there are elements in the queue, they will appear first. Should be used immediately after removing an element from the queue.
*/
static void queue_push_to_front(){
    for (int i=0; i<(QUEUE_NUMBER_OF_FLOORS-1); ++i){
        if (queue[i] == -1){
            queue[i] = queue[i+1];
            queue[i+1] = -1;
        }
    }
}
void queue_clear_floor(int floor){
    // Remove floor from arrays:
    requests_up[floor] = false;
    requests_cab[floor] = false;
    requests_down[floor-1] = false;
    for (int i = 0; i<QUEUE_NUMBER_OF_FLOORS; ++i){
        if (queue[i] == floor){
            queue[i] = -1;
        }
    }
    // Rearrange queue:
    queue_push_to_front();
}


bool queue_read_floor(int floor, QueueMovement dir){
    // Cab order to floor, should always stop:
    for (int i = 0; i<QUEUE_NUMBER_OF_FLOORS; ++i){
        if (requests_cab[floor] == true){
            return true;
        }
    }
    // If moving up, take requests going up
    if (dir == QUEUE_MOVEMENT_UP){
        if (requests_up[floor] == true){
            return true;
        }
    }
    // If moving down, take requests going down
    if (dir == QUEUE_MOVEMENT_DOWN){
        if (requests_down[floor-1] == true){
            return true;
        }
    }
    return false;
}

int queue_read_next(){
    return queue[0];
}
