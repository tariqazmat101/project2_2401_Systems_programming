#include "defs.h"
#include <stdlib.h>
#include <stdio.h>

/* Event functions */

/**
 * Initializes an `Event` structure.
 *
 * Sets up an `Event` with the provided system, resource, status, priority, and amount.
 *
 * @param[out] event     Pointer to the `Event` to initialize.
 * @param[in]  system    Pointer to the `System` that generated the event.
 * @param[in]  resource  Pointer to the `Resource` associated with the event.
 * @param[in]  status    Status code representing the event type.
 * @param[in]  priority  Priority level of the event.
 * @param[in]  amount    Amount related to the event (e.g., resource amount).
 */
void event_init(Event *event, System *system, Resource *resource, int status, int priority, int amount) {
    event->system = system;
    event->resource = resource;
    event->status = status;
    event->priority = priority;
    event->amount = amount;
}

/* EventQueue functions */

/**
 * Initializes the `EventQueue`.
 *
 * Sets up the queue for use, initializing any necessary data.
 *
 * @param[out] queue  Pointer to the `EventQueue` to initialize.
 */
void event_queue_init(EventQueue *queue) {
    queue->head = NULL;
    queue->size = 0;
}

/**
 * Cleans up the `EventQueue`.
 *
 * Frees any memory and resources associated with the `EventQueue`.
 *
 * @param[in,out] queue  Pointer to the `EventQueue` to clean.
 */
void event_queue_clean(EventQueue *queue) {
    EventNode *current = queue->head;
    while (current != NULL) {
        EventNode *temp = current;
        current = current->next;
        free(temp);
    }
    queue->head = NULL;
    queue->size = 0;
}

/**
 * Pushes an `Event` onto the `EventQueue`.
 *
 * Adds the event to the queue, maintaining priority order (highest first).
 * Ensures that older events of the same priority come before newer ones.
 *
 * @param[in,out] queue  Pointer to the `EventQueue`.
 * @param[in]     event  Pointer to the `Event` to push onto the queue.
 */
void event_queue_push(EventQueue *queue, const Event *event) {
    // Create a new EventNode
    EventNode *new_node = (EventNode *)malloc(sizeof(EventNode));
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed for EventNode.\n");
        exit(EXIT_FAILURE);
    }
    // Copy the event data into the new node
    new_node->event = *event;
    new_node->next = NULL;

    // Special case: Empty queue or new node has higher priority than head
    if (queue->head == NULL || event->priority > queue->head->event.priority) {
        new_node->next = queue->head;
        queue->head = new_node;
    } else {
        // Traverse the queue to find the correct insertion point
        EventNode *current = queue->head;
        EventNode *previous = NULL;
        while (current != NULL && current->event.priority >= event->priority) {
            previous = current;
            current = current->next;
        }
        // Insert the new node
        previous->next = new_node;
        new_node->next = current;
    }
    queue->size++;
}

/**
 * Pops an `Event` from the `EventQueue`.
 *
 * Removes the highest-priority event from the queue.
 *
 * @param[in,out] queue  Pointer to the `EventQueue`.
 * @param[out]    event  Pointer to the `Event` structure to store the popped event.
 * @return               Non-zero if an event was successfully popped; zero otherwise.
 */
int event_queue_pop(EventQueue *queue, Event *event) {
    if (queue->head == NULL) {
        // Queue is empty
        return 0;
    }
    // Remove the head node
    EventNode *temp = queue->head;
    *event = temp->event; // Copy the event data
    queue->head = temp->next;
    free(temp);
    queue->size--;
    return 1; // Indicate that an event was successfully popped
}
