#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <stdbool.h>
#include <string.h>
#include "bbuf.h"

#define MAXIMUM_BUFFER_SIZE 10

void* candy_buffer_storage[MAXIMUM_BUFFER_SIZE];
sem_t space_available;   								// defining semaphore object space_available
sem_t full;									// defining semaphore object full
sem_t mutex;									// defining semaphore object mutex
int candy_insert = 0;
int candy_consumed = 0;							// to keep count of the number of candy produced and consumed
int candy_count = 0;

void bbuff_init(void){

	// initializing semaphore space_available, full, mutex
	sem_init(&mutex, 0, 1);						// initial value of mutex set to 1
	sem_init(&space_available, 0, MAXIMUM_BUFFER_SIZE);		// initial value of space_available is equal to size of buffer
	sem_init(&full, 0, 0);   				// initial value of value full
	
	
}

void bbuff_blocking_insert(void* pointer_to_candy){
	
	sem_wait(&space_available);
	sem_wait(&mutex);

	candy_buffer_storage[candy_insert] = pointer_to_candy;
	candy_insert = (candy_insert +1) % MAXIMUM_BUFFER_SIZE;
	candy_count++;

	sem_post(&mutex);
	sem_post(&full);
}
	
void* bbuff_blocking_extract(void){
	
			
	sem_wait(&full); 
	sem_wait(&mutex);

	void *pointer_to_candy = candy_buffer_storage[candy_consumed];

	candy_buffer_storage[candy_consumed] = NULL;

	candy_consumed = (candy_consumed + 1) % MAXIMUM_BUFFER_SIZE;

	candy_count--;

	sem_post(&mutex);
	sem_post(&space_available);  
	
	return pointer_to_candy;
}

_Bool bbuff_is_empty(void){


	if (candy_count == 0)
	{
	 	return true;
	}
	else
		return false;
}