/**
 * @file
 * @brief Module for managing elevator requests and order of execution.
 */
#include <stdbool.h>
#include <stdio.h>

#define QUEUE_NUMBER_OF_FLOORS 4

/**
* @brief Used for interaction between main and queue involving elevator movement. Used primarily to read requests.
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
* @param[in] floor The floor the request was made at/to.
* @param[in] order_type The type of request, in the QueueOrder type.
*/
void queue_set_request(int floor, QueueOrder order_type);

/**
* @brief Removes ALL stored requests and empties queue. Should be used when stop button is pressed.
*/
void queue_clear_all_requests();

/**
* @brief Removes all requests concerning a given floor. Should be used when elevator stops at floor.
* @param[in] floor Remove requests at this floor.
*/
void queue_clear_floor(int floor);

/**
* @brief Looks for relevant requests given a floor and a direction of movement. Used to determine if the elevator should stop when passing by.
* @param[in] floor The floor in question
* @param[in] dir Direction of movement
* @return Whether the elevator should make a stop or not.
*/
bool queue_read_floor(int floor, QueueMovement dir);

/**
* @brief Simply returns the first floor in the queue; the next target for the elevator.
* @return Next target floor.
*/
int queue_read_next();
