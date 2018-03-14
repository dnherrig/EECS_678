/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"


/**
  Stores information making up a job to be scheduled including any statistics.

  You may need to define some global variables or a struct to store your job queue elements.
*/
typedef struct _job_t
{
	int job_id;
	int arrival_time;
	int run_time;
	int priority;
} job_t;








int compareFCFS(const void * a, const void * b)
{
	//first come first serve first
	//first job is exicuted, then the second, then the third and so on . if a job arrives it is queued to go next

	//if()

	//more important
	//return (-1);
	//same importance
	//return (0);
	//least importance
	return (1);
}

int compareSJF(const void * a, const void * b)
{
	//looks at the runtime lenght of all jobs that arive at a time and pick the one with the smallest runtime size
	//compare size, put shortest left
	//let jobs running continue to run

	return ( (*(job_t*)a -> runtime) - (*(job_t*)b -> run_time ));
}

int comparePSJF(const void * a, const void * b)
{
	//looks at the runtime length of all the jobs that arive at a time and picks the one with the smallest runtime size. if a job arives with a smaller size it will interupt and start a new
	//interupt then compare compare job length


	return ( *(int*)b - *(int*)a );
}

int comparePRI(const void * a, const void * b)
{
	//takes in the the job and runs it until completion, if a process completes, then the process with the highest priority will run
	//simmilar to SJF
	//compare priority put highest left

	return ( *(int*)b - *(int*)a );
}

int comparePPRI(const void * a, const void * b)
{
	//takes in the the job and runs it, if a job arrives of higher priority then priortize that one
	//simmilar to PSJF

	return ( *(int*)b - *(int*)a );
}

int compareRR(const void * a, const void * b)
{
	//uses quantum size and only runs programs for that long or until they expire, rotates through all of the jobs
	return ( *(int*)b - *(int*)a );
}


/**
  Initalizes the scheduler.

  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called once once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.

  @param cores the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/
void scheduler_start_up(int cores, scheme_t scheme)
{
	priqueue_t = q;


	if((scheme == FCFS))
	{
		//first come first serve first
		//first job is exicuted, then the second, then the third and so on . if a job arrives it is queued to go next

		//enqueue once completed dequeue

		priqueue_init(&q, compareFCFS);

		// for(int i = 0; i < cores; i++)
		// {
		//
		//
		//
		// }




		//printf("FCFS\n");
	}
	else if((scheme == SJF))
	{
		//looks at the runtime lenght of all jobs that arive at a time and pick the one with the smallest runtime size



		priqueue_init(&q, compareSJF);




		//printf("SJF\n");
	}
	else if((scheme == PSJF))
	{
		//looks at the runtime length of all the jobs that arive at a time and picks the one with the smallest runtime size. if a job arives with a smaller size it will interupt and start a new

		priqueue_init(&q, comparePSJF);





		//printf("PSJF\n");
	}
	else if((scheme == PRI))
	{
		//takes in the the job and runs it until completion, if a process completes, then the process with the highest priority will run
		//simmilar to SJF
		priqueue_init(&q, comparePRI);
		//printf("PRI\n");
	}
	else if((scheme == PPRI))
	{
		//takes in the the job and runs it, if a job arrives of higher priority then priortize that one
		//simmilar to PSJF

		priqueue_init(&q, comparePPRI);



		//printf("PPRI\n");
	}
	else if((scheme == RR))
	{
		//uses quantum size and only runs programs for that long or until they expire, rotates through all of the jobs

		priqueue_init(&q, compareRR);


		printf("RR\n");
	}
	else
	{
		printf("Invalid scheme\n");
	}

}


/**
  Called when a new job arrives.

  If multiple cores are idle, the job should be assigned to the core with the
  lowest id.
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the core the job should be
  scheduled on. If another job is already running on the core specified,
  this will preempt the currently running job.
  Assumptions:
    - You may assume that every job wil have a unique arrival time.

  @param job_number a globally unique identification number of the job arriving.
  @param time the current time of the simulator.
  @param running_time the total number of time units this job will run before it will be finished.
  @param priority the priority of the job. (The lower the value, the higher the priority.)
  @return index of core job should be scheduled on
  @return -1 if no scheduling changes should be made.

 */
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
	return -1;
}


/**
  Called when a job has completed execution.

  The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.

  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{
	return -1;
}


/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.

  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.

  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{
	return -1;
}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
	return 0.0;
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
	return 0.0;
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
	return 0.0;
}


/**
  Free any memory associated with your scheduler.

  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{

}


/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:

    2(-1) 4(0) 1(-1)

  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue()
{

}
