#include "stats.h"
#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <assert.h>



typedef struct {

	int factory_number;
	int candy_produced_count;
	int candy_consumed_count;	
	double maximum_time_delay;
	double minimum_time_delay;
	double total_time_delay;
	double average_time_delay;
} candy_statistics;

sem_t lock;

int number_of_producers= 0;

candy_statistics* stats = NULL;

void stats_init(int producer)
{
	assert(producer >= 0);
	number_of_producers = producer;
	stats = malloc(sizeof(*stats) * producer);
	sem_init(&lock , 0, 1);

	int i =0;

	while (i < producer)
	{
		stats[i].factory_number = i;
		stats[i].candy_produced_count = 0;
		stats[i].candy_consumed_count = 0;
		stats[i].total_time_delay = 0;
		stats[i].maximum_time_delay= 0;
		stats[i].minimum_time_delay = -1;
		stats[i].average_time_delay = 0;
		
		i++;
	}
}


void stats_record_consumed(int factory_number, double delay_in_ms)
{
	assert(factory_number >= 0);
	assert(factory_number < number_of_producers);

	sem_wait(&lock);
	
	
	if (stats[factory_number].minimum_time_delay == -1)
	{
		stats[factory_number].minimum_time_delay = delay_in_ms;
	}


	if ( stats[factory_number].maximum_time_delay < delay_in_ms)
	{
		stats[factory_number].maximum_time_delay = delay_in_ms;
	}
	if (stats[factory_number].minimum_time_delay > delay_in_ms)
	{
		stats[factory_number].minimum_time_delay = delay_in_ms;
	}

	stats[factory_number].total_time_delay = stats[factory_number].total_time_delay + delay_in_ms;

	double total_time = stats[factory_number].total_time_delay;
	double x = stats[factory_number].candy_consumed_count;

	stats[factory_number].average_time_delay = total_time / x;
	stats[factory_number].candy_consumed_count++;
	sem_post(&lock);
	
}


void stats_record_produced(int factory_number)
{
	assert(factory_number >= 0);
	assert(factory_number < number_of_producers);
	sem_wait(&lock);
	stats[factory_number].candy_produced_count++;
	sem_post(&lock);
}

void stats_display(void)
{
	
	printf ("Statistics:\n");
	printf("%10s%8s%8s%15s%15s%15s\n", "Factory#", "#Made", "#Eaten", "Min Delay[ms]", "Avg Delay[ms]", "Max Delay[ms]");
	//Data Row
	for (int i = 0; i<number_of_producers; i++)
	{		
		sem_wait(&lock);
		printf("%10d%8d%8d%15.5f%15.5f%15.5f\n", 
			stats[i].factory_number,
			stats[i].candy_produced_count, 
			stats[i].candy_consumed_count, 
			stats[i].minimum_time_delay,
			stats[i].average_time_delay, 
			stats[i].maximum_time_delay);

		if (stats[i].candy_produced_count != stats[i].candy_consumed_count)
		{
			printf("ERROR:Mismatch between number made and eaten.\n");
			
		}
		sem_post(&lock);
	}
	
}

void stats_cleanup(void)
{

	//free stats and point to NULL

	free(stats);
	stats = NULL;
}





