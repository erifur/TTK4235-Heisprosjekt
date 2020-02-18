/**
 * @file
 * @brief Module for managing elevator requests and order of execution. Should input each new request, and output them in such a way that all requests are handled, preferrably with speed.
 *
 */
#include <stdbool.h>
#include <stdio.h>

#define QUEUE_NUMBER_OF_FLOORS 4

/**
* @brief Used for interaction between main and queue regarding elevator movement. Used primarily to read requests.
*/
typedef enum{
    QUEUE_MOVEMENT_UP,
    QUEUE_MOVEMENT_DOWN
} QueueMovement;

/**
* @brief Used for interaction between main and queue regarding request type. Used primarily to set requests.
*/
typedef enum {
    QUEUE_ORDER_UP,
    QUEUE_ORDER_INSIDE,
    QUEUE_ORDER_DOWN
} QueueOrder;


/**
* @brief Sets a single request to active, should be used directly when an elevator request button is pressed.
* @param floor The floor the request was made at/to.
* @param order_type The type of request, in the QueueOrder type.
*/
void queue_set_request(int floor, QueueOrder order_type);
/**
* @brief Removes all requests on given floor. Should be used when elevator stops at floor.
* @param floor Remove requests at this floor.
*/
static void queue_add_floor(int floor);
/**
* @brief Removes ALL stored requests. Should be used when stop button is pressed.
*/
void queue_clear_all_requests();
/**
* @brief Adds a floor to the first available spot in the queue, if it is not already there.
* @param floor The floor to be added
*/
void queue_clear_floor(int floor);
/**
 * @brief Ensures that if there are elements in the queue, they will appear first. Should be used immediately after removing an element from the queue.
 */
static void queue_push_to_front();

/**
* @brief Used to look for relevant requests at specific floor. Used to determine if the elevator should stop when passing by.
* @param floor The relevant floor.
* @param dir Direction of movement, to decide if the elevator should stop at an order.
* @return Whether the elevator should make a stop or not.
*/
bool queue_read_floor(int floor, QueueMovement dir);
/**
* @brief Simply returns the first floor in the queue; the next target for the elevator.
* @return Next target floor.
*/
int queue_read_next();