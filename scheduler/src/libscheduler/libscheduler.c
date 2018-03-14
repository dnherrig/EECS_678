/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"

priqueue_t *job_queue;
scheme_t current_scheme;

int *core_array;
int core_array_size = 0;

int number_of_jobs = 0;
int total_jobs = 0;

/**
  Stores information making up a job to be scheduled including any statistics.

  You may need to define some global variables or a struct to store your job queue elements.
*/
typedef struct _job_t
{
	//baseline job variables. given by input
	int arrival_time;
	int run_time;
	int priority;

	//calculated job variables
	int job_id;
	int time_remaining;
	int wait_time;
	int wait_start;
	int response_time;

	//core number job is running on
	int executing_core_id;
} job_t;

int compare1(const void * a, const void * b) {
	int id1 = ((job_t*)a)->job_id;
	int id2 = ((job_t*)b)->job_id;
	if(id1 > id2) {
		return 1;
	}
	else if(id1 < id2) {
		return -1;
	}
	else {
		return 0;
	}
}

int compare2(const void * a, const void * b) {
	int rt1 = ((job_t*)a)->time_remaining;
	int rt2 = ((job_t*)b)->time_remaining;
	if(rt1 > rt2) {
		return 1;
	}
	else if(rt1 < rt2) {
		return -1;
	}
	else {
		return 0;
	}
}

int compare3(const void * a, const void * b) {
	int p1 = ((job_t*)a)->priority;
	int p2 = ((job_t*)b)->priority;
	if(p1 > p2) {
		return 1;
	}
	else if(p1 < p2) {
		return -1;
	}
	else {
		return 0;
	}
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
	//allocate array for given amount of cores
	core_array = malloc(sizeof(int) * cores);
	core_array_size = cores;
	//initialize each array index to be 0 (not used / false);
	for(int i = 0; i < cores; i++) {
		core_array[i] = 0;
	}

	job_queue = malloc(sizeof(priqueue_t));
	current_scheme = scheme;

	if(scheme == FCFS) {
		priqueue_init(job_queue, compare1);
	}
	else if(scheme == SJF) {
		priqueue_init(job_queue, compare1);//?
	}
	else if(scheme == PSJF) {
		priqueue_init(job_queue, compare2);
	}
	else if(scheme == PRI) {
		priqueue_init(job_queue, compare3);//?
	}
	else if(scheme == PPRI) {
		priqueue_init(job_queue, compare3);
	}
	else if(scheme == RR) {
		priqueue_init(job_queue, compare1);//?
	}
	else {
		//
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
	//return -1;

	//run through queue and update times
	for(int i = 0; i < priqueue_size(job_queue); i++) {
		//grab the job at i
		job_t *current_job = (job_t*)priqueue_at(job_queue , i);
		//if the job is currently being executed
		if(current_job->executing_core_id != -1) {
			current_job->time_remaining = current_job->run_time - (time -  current_job->wait_time - current_job->arrival_time);
		}
	}

	//set scheduler variables
	int has_scheduled_bool = 0;
	int scheduler_core = -1;

	//create new job
	job_t *new_job = malloc(sizeof(job_t));

	//basic job assignments
	//new_job->job_id = total_jobs;
	new_job->job_id = job_number;
	new_job->arrival_time = time;
	new_job->run_time = running_time;
	new_job->priority = priority;

	new_job->time_remaining = running_time;
	new_job->wait_time = time;//?
	new_job->wait_start = time;
	new_job->response_time = -1;//?

	//new_job->is_executing = 1;
	new_job->executing_core_id = -1;//?

	for(int i = 0; i < core_array_size; i++) {
		//if core i is idle
		if(core_array[i] == 0) {
			//update new job values
			new_job->executing_core_id = scheduler_core;
			new_job->wait_time = 0;
			new_job->response_time = 0;

			//add new job to the queue
			priqueue_offer(job_queue, new_job);

			//update scheduling variable values
			has_scheduled_bool = 1;
			core_array[i] = 1;
			scheduler_core = i;

			//break loop in this case
			break;
		}
	}

	//job hasn't been added yet and the current scheme is preemptive
	if(has_scheduled_bool == 0 && (current_scheme == PSJF || current_scheme == PPRI)) {
		//insert job into job queue and grab index where it is located
		int index = priqueue_offer(job_queue, new_job);

		//if the index is less than the amount of cores available
		if(index < core_array_size) {
			//traverse through the queue
			for(int i = (priqueue_size(job_queue)-1); i >= 0; i--) {
				//get current job
				job_t *current_job = (job_t*)priqueue_at(job_queue,i);

				//if the current job is being executed
				if(current_job->executing_core_id != -1) {

					//update scheduler variables
					scheduler_core = current_job->executing_core_id;
					core_array[scheduler_core] = 1;

					//update current job values
					current_job->executing_core_id = -1;
					current_job->wait_start = time;
					current_job->time_remaining = current_job->run_time - (time -  current_job->wait_time - current_job->arrival_time);

					//if the current job has executed during any cycles
					if(current_job->time_remaining == current_job->run_time) {
						current_job->response_time = -1;
						current_job->wait_start = -1;
						current_job->wait_time = -1;
					}

					//break loop on this case
					break;
				}
			}

			//update new job variables
			new_job->executing_core_id = scheduler_core;
			new_job->wait_time = 0;
			new_job->response_time = 0;

			//update scheduler variables
			has_scheduled_bool = 1;
		}
	}

	//cannot be scheduled right now
	if(has_scheduled_bool == 0) {
		priqueue_offer(job_queue, new_job);
	}

	//update global job variables
	number_of_jobs++;
	total_jobs++;

	//return core that new job should run on, otherwise -1
	return scheduler_core;
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
	//run through queue and print job_id and priority
	for(int i = 0; i < priqueue_size(job_queue); i++) {
		//grab the job at i
		job_t *current_job = (job_t*)priqueue_at(job_queue , i);
		//print job_id and priority
		printf("%d(%d) ", current_job->job_id, current_job->priority);
	}
}
