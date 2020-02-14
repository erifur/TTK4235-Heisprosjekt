#include "queue.h"


static bool requests_cab[QUEUE_NUMBER_OF_FLOORS] = {0};
static bool requests_up[QUEUE_NUMBER_OF_FLOORS-1] = {0};
static bool requests_down[QUEUE_NUMBER_OF_FLOORS-1] = {0};

static int queue[QUEUE_NUMBER_OF_FLOORS] = {0};

void queue_set_request(int floor, int order_type){
    switch(order_type){
        case QUEUE_ORDER_UP :
            requests_up[floor - 1] = true;
            // index=floor-1 because index 0 = floor 1
            break;
        case QUEUE_ORDER_INSIDE :
            requests_cab[floor - 1] = true;
            break;
        case QUEUE_ORDER_DOWN :
            requests_down[floor - 2] = true;
            //index=floor-2 because starts at floor 2
            break;
    }
    
    // Check if floor already in queue:
    for (int i = 0; i<QUEUE_NUMBER_OF_FLOORS; ++i){
        if (queue[i] == floor){ return; }   // Return if floor in queue
    }
    add_to_queue(floor);    // If not, add to queue
}

static void add_to_queue(int floor){
    for (int i = 0; i<QUEUE_NUMBER_OF_FLOORS; ++i){
        if (queue[i] == 0){ // Add floor to first free spot, then return
            queue[i] = floor;
            return;
        }
    }
    return;
}

void queue_clear_all_requests(){
    for (int i = 0; i<QUEUE_NUMBER_OF_FLOORS; ++i){
        queue[i] = 0;
        requests_cab[j] = false;
    }
    for (int j = 0; j<(QUEUE_NUMBER_OF_FLOORS-1); ++i){
        requests_up[j] = false;
        requests_down[j] = false;
    }
    return;
}

void queue_clear_floor(int floor){
    for (int i = 0; i<QUEUE_NUMBER_OF_FLOORS; ++i){
        if (queue[i] == floor){
            queue[i] = 0;
        }
    }
    for (int j = 0; i<QUEUE_NUMBER_OF_FLOORS-1; ++i){
        if (requests_cab[j] == floor){
            requests_cab[j] = 0;
        }
    }
}

bool queue_read(int floor, bool dir){
    // denne må diskuteres, trenger også retning?
}

int queue_read_next(){
    return queue[0];
}