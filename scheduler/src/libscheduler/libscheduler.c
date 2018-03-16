/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"

priqueue_t *job_queue;
scheme_t current_scheme;

int *core_array;
int core_array_size = 0;

int number_of_jobs = 0;
int total_jobs = 0;
int quantum_time_count = 0;

float total_wait_time = 0.0;
float total_response_time = 0.0;
float total_turnaround_time = 0.0;

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
	//float avg_response_time;
	float turnaround_time;
	float _waiting_time_for_avg;
	float _response_time_for_avg;
	int start_time;
	int time_remaining;
	int wait_time;
	int initial_wait;
	int response_time;
	int quantum_time;
	int previously_scheduled;


	//core number job is running on
	int executing_core_id;
} job_t;

int FCFScompare(const void * a, const void * b) {

	//first come first serve first
	//first job is exicuted, then the second, then the third and so on . if a job arrives it is queued to go next

	int id1 = ((job_t*)a)->job_id;
	int id2 = ((job_t*)b)->job_id;
	if(id1 > id2) {
		//return 1 because it allways sets a newly arived job to the back
		//should always go here
		return 1;
	}
	else if(id1 < id2) {
		return -1;
	}
	else {
		return 0;
	}
}

int SJFcompare(const void * a, const void * b) {
	//looks at the runtime length of all jobs that arive at a time and pick the one with the smallest runtime size
	//compare runtime put highest right
	int rt1 = ((job_t*)a)->run_time;
	int rt2 = ((job_t*)b)->run_time;
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

int PSJFcompare(const void * a, const void * b) {
	//looks at the runtime length of all jobs that arive at a time and pick the one with the smallest runtime size
	//compare runtime put highest right
	//should work the same as SFJcompare
	int rt1 = ((job_t*)a)->time_remaining;//was run time
	int rt2 = ((job_t*)b)->time_remaining;//was run time
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

int PRIcompare(const void * a, const void * b) {
	//takes in the the job and runs it until completion, if a process completes, then the process with the highest priority will run
	//simmilar to SJF
	//compare priority put highest left
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

int PPRIcompare(const void * a, const void * b) {
	//takes in the the job and runs it, if a job arrives of higher priority then priortize that one
	//simmilar to PSJF
	//compare priority put highest left
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

int RRcompare(const void * a, const void * b) {

	//uses quantum size and only runs programs for that long or until they expire, rotates through all of the jobs
	//always places new tasks in the back, scheduler_quantum_expired will handle swithing tasks at quantum

	int id1 = ((job_t*)a)->quantum_time;
	int id2 = ((job_t*)b)->quantum_time;
	if(id1 > id2) {
		//return 1 because it allways sets a newly arived job to the back
		//should always go here
		return 1;
	}
	else if(id1 < id2) {
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
		priqueue_init(job_queue, FCFScompare);
	}
	else if(scheme == SJF) {
		priqueue_init(job_queue, SJFcompare);//?
	}
	else if(scheme == PSJF) {
		priqueue_init(job_queue, PSJFcompare);
	}
	else if(scheme == PRI) {
		priqueue_init(job_queue, PRIcompare);//?
	}
	else if(scheme == PPRI) {
		priqueue_init(job_queue, PPRIcompare);
	}
	else if(scheme == RR) {
		priqueue_init(job_queue, RRcompare);//?
	}
	else {
		printf("Invalid scheme, should never reach here\n");
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
	//update time remaining for currently executed jobs
	for(int i = 0; i < priqueue_size(job_queue); i ++) {
		//grab a job in the queue
		job_t *current_job = (job_t*)priqueue_at(job_queue, i);

		//if the job is being executed
		if(current_job->executing_core_id != -1) {
			current_job->time_remaining = current_job->run_time - (time - current_job->wait_time - current_job->arrival_time);
		}
	}

	//set scheduler variables
	int has_scheduled_bool = 0;
	int scheduler_core = -1;

	//create new job
	job_t *new_job = malloc(sizeof(job_t));

	//basic job assignments
	new_job->job_id = job_number;
	new_job->arrival_time = time;
	new_job->run_time = running_time;
	new_job->priority = priority;
	new_job->previously_scheduled = 0;
	new_job->quantum_time = quantum_time_count;
	quantum_time_count++;

	//used for timing things
	new_job->time_remaining = running_time;
	new_job->wait_time = -1;
	new_job->initial_wait = -1;
	new_job->response_time = -1;

	//new_job->is_executing = 1;
	new_job->executing_core_id = -1;//?

	//check to see if any jobs can be added to a core
	for(int i = 0; i < core_array_size; i++) {
		//if core i is idle
		if(core_array[i] == 0) {
			//update new job values
			scheduler_core = i;
			new_job->executing_core_id = scheduler_core;
			new_job->wait_time = 0;
			new_job->response_time = 0;

			if(new_job->previously_scheduled == 0)
			{
				new_job->previously_scheduled = 1;
				new_job->start_time = time;
			}

			//printf("base\n");
			//add new job to the queue
			priqueue_offer(job_queue, new_job);

			//update scheduling variable values
			has_scheduled_bool = 1;
			core_array[i] = 1;

			//break loop in this case
			break;
		}
	}

	//if not scheduled by now... check of the current scheme is preemptive
	if(has_scheduled_bool == 0 && (current_scheme == PSJF || current_scheme == PPRI)) {
		//printf("%d INDEX \n", index);

		priqueue_offer(job_queue, new_job);
		int index = indexFinderHelper(job_queue, new_job);

		if(index < core_array_size) {
			for(int i = (priqueue_size(job_queue) - 1); i >= 0; i--) {
				//grab job at i in the queue
				job_t *current_job = (job_t*)priqueue_at(job_queue, i);

				//if the job is currently being executed
				if(current_job->executing_core_id != -1) {
					//grab the core id for swap / override
					scheduler_core = current_job->executing_core_id;
					core_array[scheduler_core] = 1;
					current_job->executing_core_id = -1;

					//update the job's time variables
					current_job->initial_wait = time;
					current_job->time_remaining = current_job->run_time - (time - current_job->wait_time - current_job->arrival_time);

					break;
				}
			}

			//give the new job its core assignment
			new_job->executing_core_id = scheduler_core;

			if(new_job->previously_scheduled == 0)
			{
				new_job->previously_scheduled = 1;
				new_job->start_time = time;
			}

			//update new job's times
			new_job->wait_time = 0;
			new_job->response_time = 0;
		}

		//the new job has been scheduled
		has_scheduled_bool = 1;
	}

	//cannot be scheduled right now
	if(has_scheduled_bool == 0) {
		//printf("last\n");
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
	//set core with core_id to open
	core_array[core_id] = 0;
	//used for determining idle core
	int idle = -1;

	//remove finished job
	for(int i=0; i < number_of_jobs; i++)
	{
		job_t *current_job  = (job_t*)priqueue_at(job_queue , i);
		if(current_job -> job_id == job_number)
		{

			current_job -> _waiting_time_for_avg =  (float)((time)-(current_job -> arrival_time) - (current_job ->run_time));
			total_wait_time = ((current_job -> _waiting_time_for_avg) + total_wait_time);

			current_job -> turnaround_time =  (float)(time - current_job ->arrival_time);
			total_turnaround_time = ((current_job -> turnaround_time) + total_turnaround_time);

			current_job -> _response_time_for_avg =  fabs((double)((current_job -> arrival_time) - (current_job->start_time)));
			total_response_time = ((current_job -> _response_time_for_avg) + total_response_time);

			//printf("JOB %d COMPLETE... RESPONSE TIME = %d\n", current_job->job_id, current_job->_response_time_for_avg);
			
			// if(total_wait_time - total_response_time <= 1) {
			// 	total_response_time = total_wait_time;
			// }

			//if(current_scheme == RR)
			//{


				// current_job -> roundRobin_waiting_time =  (float)((time)-(current_job -> arrival_time) - (current_job ->run_time));
				// total_wait_time = ((current_job -> roundRobin_waiting_time) + total_wait_time);

				// current_job -> roundRobin_response_time =  (float)(time - current_job ->arrival_time);
				// total_response_time = ((current_job -> roundRobin_response_time) + total_response_time);
			//}
			//else
			//{
				//current_job -> wait_time =  (float)(time - current_job ->arrival_time);
				//total_wait_time = ((current_job -> wait_time) + total_wait_time);

				// current_job -> response_time =  (float)(time - current_job ->arrival_time);
				// total_response_time = ((current_job -> response_time) + total_response_time);
			//}

			idle = core_id;
			priqueue_remove(job_queue, current_job);
			number_of_jobs --;

			break;
		}
	}

	//peek at the front of the queue
	job_t *front_job = (job_t*)priqueue_peek(job_queue);

	//if the queue isn't empty
	if(front_job != NULL) {

		//look for next executed job
		job_t *current_job = (job_t*)priqueue_at(job_queue, 0);

		for(int i = 1; i < number_of_jobs; i++) {
			if(current_job->executing_core_id != -1) {
				current_job = (job_t*)priqueue_at(job_queue, i);
			}
		}

		//if the current job is currently being executed
		if(current_job->executing_core_id != -1) {
			return -1;
		}
		else {
			//check times and update accordingly
			if(current_job->start_time != time && current_job->time_remaining == current_job->run_time && current_scheme != RR) {
				current_job->start_time = time;
			}

			if(current_job->response_time == -1) {
				current_job->wait_time = time - current_job->arrival_time;
				current_job->response_time = time - current_job->arrival_time;
			}

			if(current_job->initial_wait != -1) {
				current_job->wait_time = current_job->wait_time + (time - current_job->initial_wait);
				current_job->initial_wait = -1;
			}
		}

		//set the current job up to be executed by the core next
		current_job->executing_core_id = idle;
		core_array[core_id] = 1;
		if(current_job->previously_scheduled == 0)
		{
			current_job ->previously_scheduled = 1;
			current_job-> start_time = time;
		}

		//return job id core should run
		return current_job->job_id;
	}

	//old new job finder
	/*for(int i=0; i < number_of_jobs; i++)
	{
		job_t *current_job  = (job_t*)priqueue_at(job_queue , i);
		if(current_job -> executing_core_id == -1)
		{
			//schedule on this core
			current_job->executing_core_id = core_id;

			if(current_job-> previously_scheduled == 0)
			{
				current_job -> previously_scheduled = 1;
				current_job -> avg_response_time =  (float)(time - current_job -> arrival_time);
				total_response_time = ((current_job -> avg_response_time) + total_response_time);
			}
			core_array[core_id] = 1;
			//printf("%d JOB ID\n", current_job->job_id);
			return(current_job -> job_id);
		}
	}*/

	//core shall remain idle
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
	//printf("The QUANTUM JUST EXPIRED\n" );

	core_array[core_id] = 0;
	//int otherJobFound = 0;
	//int pickNext = 0;
	job_t *save_job1;
	//job_t *save_job2;
	job_t *save_job3;
	//int prevPriority;
	//job_t *job_just_paused;
	//job_t *prev_job;
	//job_t *first_job;


	for(int i=0; i < number_of_jobs; i++)
	{
		job_t *current_job  = (job_t*)priqueue_at(job_queue , i);
		save_job1 = current_job;
		if(current_job -> executing_core_id == core_id)
		{
			//save_job1 = current_job;
			priqueue_remove(job_queue, current_job);
			save_job1 -> executing_core_id = -1;
			save_job1 -> quantum_time = quantum_time_count;
			quantum_time_count++;
			number_of_jobs --;
			break;
		}
	}

	printf("Placing Job: %d at the back\n", save_job1->job_id );

	priqueue_offer(job_queue, save_job1);
	number_of_jobs ++;

	printf("The number of Jobs is %d\n", number_of_jobs );
	save_job3 = (job_t*)priqueue_at(job_queue , 0);
	printf("the first job is %d \n", save_job3 -> job_id);
	save_job3 = (job_t*)priqueue_at(job_queue , number_of_jobs-1);
	printf("the last job is %d \n", save_job3 -> job_id);


	if(number_of_jobs == 0)
	{
		return -1;
	}
	else
	{
		core_array[core_id] = 1;
		for(int i = 0; i< number_of_jobs; i++)
		{
			job_t *current_job  = (job_t*)priqueue_at(job_queue , i);
			if(current_job -> executing_core_id == -1)
			{
				if(current_job->previously_scheduled == 0)
				{
					current_job ->previously_scheduled = 1;
					current_job-> start_time = time;
				}

				current_job -> executing_core_id = core_id;
				return(current_job -> job_id);
			}

		}
		 return(-1);
	}

							//try 3, very close
							// for(int i = 0; i < number_of_jobs; i++)
							// {
							// 	job_t *current_job  = (job_t*)priqueue_at(job_queue , i);
							//
							//
							// 	if((pickNext == 1)&&(current_job -> executing_core_id == -1))
							// 	{
							// 		core_array[core_id] = 1;
							// 		current_job -> executing_core_id = core_id;
							// 		return(current_job -> job_id);
							// 	}
							// 	if(current_job -> executing_core_id == core_id)
							// 	{
							// 		current_job -> executing_core_id = -1;
							// 		save_job1 = current_job;
							// 		pickNext = 1;
							// 	}
							//
							// }
							//
							// for(int i = 0; i < number_of_jobs; i++)
							// {
							// 	job_t *current_job  = (job_t*)priqueue_at(job_queue , i);
							//
							//
							// 	if((pickNext == 1)&&(current_job -> executing_core_id == -1))
							// 	{
							// 		core_array[core_id] = 1;
							// 		current_job -> executing_core_id = core_id;
							// 		return(current_job -> job_id);
							// 	}
							//
							// }


	// if (pickNext == 1)
	// {
	// 	core_array[core_id] = 1;
	// 	save_job1 = (job_t*)priqueue_at(job_queue , 0);
	// 	save_job1 -> executing_core_id = core_id;
	// 	return(save_job1 -> job_id);
	// }


//return (-1);

}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
	float totalAvgWaitTime = (total_wait_time/(total_jobs));
	return totalAvgWaitTime;
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
	float totalAvgTurnAroundTime = (total_turnaround_time/(total_jobs));
	return totalAvgTurnAroundTime;
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
	float totalAvgTurnResponseTime = (total_response_time/(total_jobs));
	//totalAvgTurnResponseTime = totalAvgTurnResponseTime * (-1.0);
	return totalAvgTurnResponseTime;
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
	//printf("\n" );
	//run through queue and print job_id and priority
	for(int i = 0; i < priqueue_size(job_queue); i++) {
		//grab the job at i
		job_t *current_job = (job_t*)priqueue_at(job_queue , i);
		//print job_id and priority
		//printf("Job id:(%d), Quantumtime:(%d), Core id: (%d)\n", current_job->job_id, current_job->quantum_time, current_job-> executing_core_id);
		printf("%d(%d)", current_job->job_id, current_job->priority);
	}
}
