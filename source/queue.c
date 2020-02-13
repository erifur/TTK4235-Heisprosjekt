#include "queue.h"


static bool requests[QUEUE_NUMBER_OF_FLOORS] = {0};
static int queue[QUEUE_NUMBER_OF_FLOORS] = {0};


void queue_set_request(int floor, int order_type){
    switch(order_type){
        case QUEUE_ORDER_UP :
            requests[floor + 2 * QUEUE_NUMBER_OF_FLOORS - 2] = true;
            break;
        case QUEUE_ORDER_INSIDE :
            requests[floor - 1] = true;
            break;
        case QUEUE_ORDER_DOWN :
            requests[floor + QUEUE_NUMBER_OF_FLOORS - 2] = true;
            break;
    }
    bool number_in_queue = false
    for (int i = 0; i<QUEUE_NUMBER_OF_FLOORS; ++i){
        if (queue[i] == floor){ return; }
    }
    add_to_queue(floor);
}

static void add_to_queue(int floor){
    for (int i = 0; i<QUEUE_NUMBER_OF_FLOORS; ++i){
        if (queue[i] == 0){
            queue[i] = floor;
            return;
        }
    }
    return;
}

void queue_clear_all_requests(){
    for (int i = 0; i<QUEUE_NUMBER_OF_FLOORS; ++i){
        queue[i] = 0;
        
    }
}

void queue_clear_floor(int floor){
    
}

bool queue_read(int floor){
    
}

int queue_read_next(){
    
}