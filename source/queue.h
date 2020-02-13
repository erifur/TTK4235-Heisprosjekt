typedef enum {
    QUEUE_ORDER_UP,
    QUEUE_ORDER_INSIDE,
    QUEUE_ORDER_DOWN
} QueueOrder;

void queue_set_request(int floor, int order_type);

void queue_clear_all_requests();

void queue_clear_floor(int floor);

bool queue_read(int floor);

int queue_read_next();