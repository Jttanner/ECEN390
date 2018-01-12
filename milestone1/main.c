/*
 * main.c
 *
 *  Created on: Jan 11, 2018
 *      Author: tshumwa2
 */

#include "queue.h"

#define SIZE 20
#define NAME "Best_Queue"



int main(){
    queue_runTest();
    queue_t myQueue;
    queue_init(&myQueue, SIZE, NAME);
    queue_push(&myQueue, 10);
    queue_push(&myQueue, 11);
    queue_push(&myQueue, 12);
    queue_push(&myQueue, 13);
    queue_push(&myQueue, 14);
    queue_print(&myQueue);
    queue_pop(&myQueue);
    queue_pop(&myQueue);
    queue_pop(&myQueue);
    queue_print(&myQueue);
    queue_push(&myQueue, 15);
    queue_print(&myQueue);
    return 0;
}

void isr_function() {}
