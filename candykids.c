
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <time.h>
#include <assert.h>
#include "bbuf.h"
#include "stats.h"



_Bool stop_thread = false;

typedef struct  {
    int factory_number;
    double time_stamp_in_ms;
} candy_t;




double current_time_in_ms(void)
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec * 1000.0 + now.tv_nsec/1000000.0;
}



void *candy_produced_thread(void *thread)
{	
	int factory = *(int*) thread;

	srand(time(NULL));

	while (!stop_thread)
	{
		int random_wait_time = rand()% 4;
		printf("\tFactory %d ships candy & waits %ds\n", factory, random_wait_time);

		    candy_t *candy = malloc(sizeof(*candy));
   			candy->factory_number = factory;
    		candy->time_stamp_in_ms = current_time_in_ms();

    		stats_record_produced(factory);
    		bbuff_blocking_insert(candy);

    		
    		sleep(random_wait_time);
    		//free (candy);

	}

	printf("Candy-Factory  %d Done!\n", factory);
	pthread_exit(NULL);
}

void *candy_consumed_thread(void *thread)
{
	srand(time(NULL));

	while(true)
	{
		int random_sleep_time = rand()%2;


		candy_t* c_k = bbuff_blocking_extract();

		if (c_k != NULL)
		{
			int factory_count = c_k->factory_number;
			double t = c_k->time_stamp_in_ms;
			double time = current_time_in_ms() - t;

		
			stats_record_consumed(factory_count, time);
			free(c_k);
		}
			
		sleep(random_sleep_time);

	}

	pthread_exit(NULL);

}

 
int main(int argc, char* argv[])
{
	// 1.  Extract arguments......

	if (argc != 4)
	{
		printf("Error! 3 arguments needed.\n");
		exit(1);
	}

		int number_of_factory = atoi(argv[1]);
		int number_of_kids = atoi(argv[2]);
		int seconds = atoi(argv[3]);

	if (!(number_of_factory > 0) || !(number_of_kids > 0) || !(seconds > 0))
	{
		printf("Error: All arguments must be positive.\n");
		exit(1);
	}

	// 2.  Initialize modules..........
	bbuff_init();
	stats_init(number_of_factory);

	// 3.  Launch candy-factory threads.....

	pthread_t factory_thread_array[number_of_factory]; 
	
	int thread_variable[number_of_factory]; 
	
	for (int i=0; i<number_of_factory; i++)
	{
		thread_variable[i] = i;
		//pthread_create(&factory_thread_array[i],NULL, candy_produced_thread ,thread_variable[i])
	}

	//---------------------------/

	int check_x = 0;

	for (int j = 0; j < number_of_factory; j++) 
	{

        check_x= pthread_create(&factory_thread_array[j], NULL, candy_produced_thread, &thread_variable[j]);
		if(check_x != 0) 
		{
			printf ("Error! while creating factory thread\n");
			exit (1);
		}
	}
	

	// 4.  Launch kid threads................
	int check_y = 0;
	pthread_t kid_thread_array[number_of_kids];

	for (int s = 0; s < number_of_kids; s++)
	{
		check_y = pthread_create(&kid_thread_array[s], NULL, candy_consumed_thread, NULL);

		if (check_y!=0)
		{
			printf ("Error! while creating  kid thread \n");
			exit (1);
		}
	}


    // 5.  Wait for requested time........
	int tt= 0;
	printf("Time %ds\n", tt);
	while (tt < seconds)
	{
		sleep(1);
		printf("Time %ds\n", tt+1);

		tt++;
	}


    // 6.  Stop candy-factory threads.............

	stop_thread = true;
	printf("Stopping Candy Facories.......\n");
	for(int k =0; k < number_of_factory; k++)
	{
		pthread_join(factory_thread_array[k], NULL);
	}

     // 7.  Wait until no more candy

	while (true)
	{
		if (bbuff_is_empty() == false)
		{
			printf("Waiting for all candy to be consumed.......\n");
			sleep(1);
		}
		else
		{
			break;
		}
	}

	// 8.  Stop kid threads..........
	printf("Stopping kids.\n");
	for (int q = 0; q < number_of_kids; q++) 
	{
		pthread_cancel(kid_thread_array[q]);
		pthread_join(kid_thread_array[q], NULL);
	}

    // 9.  Print statistics

	stats_display();



    // 10. Cleanup any allocated memory

	stats_cleanup();
	
	return 0;
}

