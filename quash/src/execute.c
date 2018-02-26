/**
 * @file execute.c
 *
 * @brief Implements interface functions between Quash and the environment and
 * functions that interpret an execute commands.
 *
 * @note As you add things to this file you may want to change the method signature
 */

#include "execute.h"

#include <stdio.h>

#include "quash.h"

//additional libraries
#include <unistd.h>
#include "deque.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>

//used for pipes
static int pipes[2][2];
int pipeWhere;

//number of jobs
int numJob = 1;

//bool for if jobQueue is initialized
bool isInitialized = false;

IMPLEMENT_DEQUE_STRUCT(pidQueueStruct, pid_t);
IMPLEMENT_DEQUE(pidQueueStruct, pid_t);

pidQueueStruct pidQueue;

typedef struct jobType {
  int id;
  char* cmd;
  pidQueueStruct myQueue;
}jobType;

IMPLEMENT_DEQUE_STRUCT(jobQueueStruct, jobType);
IMPLEMENT_DEQUE(jobQueueStruct, jobType);

jobQueueStruct jobQueue;

// Remove this and all expansion calls to it
/**
 * @brief Note calls to any function that requires implementation
 */
#define IMPLEMENT_ME()                                                  \
  fprintf(stderr, "IMPLEMENT ME: %s(line %d): %s()\n", __FILE__, __LINE__, __FUNCTION__)

/***************************************************************************
 * Interface Functions
 ***************************************************************************/

// Return a string containing the current working directory. *****
char* get_current_directory(bool* should_free) {
  // COMPLETED: Get the current working directory. This will fix the prompt path.
  // HINT: This should be pretty simple
  // Change this to true if necessary
  *should_free = false;
  return getcwd(NULL, 0);
  //IMPLEMENT_ME();
}

// Returns the value of an environment variable env_var *****
const char* lookup_env(const char* env_var) {
  // COMPLETED: Lookup environment variables. This is required for parser to be able
  // to interpret variables from the command line and display the prompt
  // correctly
  // HINT: This should be pretty simple
  //IMPLEMENT_ME();
  // COMPLETED: Remove warning silencers
  //(void) env_var; // Silence unused variable warning

  return getenv(env_var);

}

// Check the status of background jobs
void check_jobs_bg_status() {
  // COMPLETED: Check on the statuses of all processes belonging to all background
  // jobs. This function should remove jobs from the jobs queue once all
  // processes belonging to a job have completed.
  //IMPLEMENT_ME();
  // COMPLETED: Once jobs are implemented, uncomment and fill the following line
  // print_job_bg_complete(job_id, pid, cmd);
  int i = 0;
  int j = 0;

  if(is_empty_jobQueueStruct(&jobQueue)) {
    return;
  }
  else {
    int x = length_jobQueueStruct(&jobQueue);

    while(i < x) {
      jobType current = pop_front_jobQueueStruct(&jobQueue);
      pid_t tempPid = peek_front_pidQueueStruct(&current.myQueue);

      j = 0;
      int y = length_pidQueueStruct(&current.myQueue);
      while(j < y) {
        pid_t myPid = pop_front_pidQueueStruct(&current.myQueue);
        int statID;

        if(waitpid(myPid, &statID, WNOHANG) == 0) {
          //printf("keep waiting\n");
          push_back_pidQueueStruct(&current.myQueue, myPid);
        }
        else if(waitpid(myPid, &statID, 0) == -1) {
          //printf("error\n");
          //exit(EXIT_FAILURE);
        }
        else {
          //printf("done\n");
        }

        j++;
      }

      if(is_empty_pidQueueStruct(&current.myQueue)) {
        print_job_bg_complete(current.id, tempPid, current.cmd);
        free(current.cmd);
        destroy_pidQueueStruct(&current.myQueue);
      }
      else {
        push_back_jobQueueStruct(&jobQueue,current);
      }

      i++;
    }
  }
}

// Prints the job id number, the process id of the first process belonging to
// the Job, and the command string associated with this job
void print_job(int job_id, pid_t pid, const char* cmd) {
  printf("[%d]\t%8d\t%s\n", job_id, pid, cmd);
  fflush(stdout);
}

// Prints a start up message for background processes
void print_job_bg_start(int job_id, pid_t pid, const char* cmd) {
  printf("Background job started: ");
  print_job(job_id, pid, cmd);
}

// Prints a completion message followed by the print job
void print_job_bg_complete(int job_id, pid_t pid, const char* cmd) {
  printf("Completed: \t");
  print_job(job_id, pid, cmd);
}

/***************************************************************************
 * Functions to process commands
 ***************************************************************************/
// Run a program reachable by the path environment variable, relative path, or
// absolute path *****
void run_generic(GenericCommand cmd) {
  // Execute a program with a list of arguments. The `args` array is a NULL
  // terminated (last string is always NULL) list of strings. The first element
  // in the array is the executable
  char* exec = cmd.args[0];
  char** args = cmd.args;

  // COMPLETED: Remove warning silencers
  //(void) exec; // Silence unused variable warning
  //(void) args; // Silence unused variable warning

  // COMPLETED: Implement run generic
  //IMPLEMENT_ME();

  execvp(exec, args);

  //fprintf(stderr, "ERROR: Failed to execute the program: %s", exec);
  //exit(EXIT_FAILURE);
  //perror("ERROR: Failed to execute program");
}

// Print strings *****
void run_echo(EchoCommand cmd) {
  char** str = cmd.args;

  // COMPLETED: Remove warning silencers
  //(void) str; // Silence unused variable warning

  // COMPLETED: Implement echo
  //IMPLEMENT_ME();

  //printf("ECHO CHECK\n");
  while(str != NULL) {
    printf("%s ", *str);
    if(*(str+1) != NULL) {
      str++;
    }
    else {
      break;
    }
  }
  printf("\n");
  fflush(stdout);
}

// Sets an environment variable *****
void run_export(ExportCommand cmd) {
  // Write an environment variable
  const char* env_var = cmd.env_var;
  const char* val = cmd.val;

  // COMPLETED: Remove warning silencers
  //(void) str; // Silence unused variable warning

  // COMPLETED: Implement echo
  //IMPLEMENT_ME();

  if(setenv(env_var, val, 1)) {
    fprintf(stderr, "ERROR: Failed to update environment variable, %s, to value, %s\n", env_var, val);
    //exit(EXIT_FAILURE);
  }
}

// Changes the current working directory *****
void run_cd(CDCommand cmd) {
  const char* dir = cmd.dir;

  if(dir == NULL) {
    return;
  }

  // COMPLETED: Change directory

  // COMPLETED: Update the PWD environment variable to be the new current working
  // directory and optionally update OLD_PWD environment variable to be the old
  // working directory.

  if(chdir(dir) != 0) {
    return;
  }

  const char* temp = getenv("PWD");

  if(setenv("OLD_PWD", temp, 1) != 0) {
    return;
  }
  if(setenv("PWD", dir, 1) != 0) {
    return;
  }
  //IMPLEMENT_ME();
}

// Sends a signal to all processes contained in a job *****
void run_kill(KillCommand cmd) {
  int signal = cmd.sig;
  int job_id = cmd.job;

  // COMPLETED: Remove warning silencers
  //(void) signal; // Silence unused variable warning
  //(void) job_id; // Silence unused variable warning

  // COMPLETED: Kill all processes associated with a background job

  int i = 0;
  int j = 0;

  //printf("CHECK 0\n");
  while(i < length_jobQueueStruct(&jobQueue)) {
    jobType current;
    current = peek_front_jobQueueStruct(&jobQueue);
    //printf("CHECK 1\n");
    //printf("%s\n", current.id);

    if(current.id == job_id)
    {
      //printf("CHECK 2\n");
      int check = length_pidQueueStruct(&current.myQueue);
      while(j < check)
        {
          //printf("CHECK 3\n");
          pid_t current2 = peek_front_pidQueueStruct(&current.myQueue);
          kill(current2, signal);
          pop_front_pidQueueStruct(&current.myQueue);
          j++;
        }
    }
    else
    {
      //printf("CHECK 4\n");
      push_back_jobQueueStruct(&jobQueue, pop_front_jobQueueStruct(&jobQueue));
      //pop
    }
    i++;
  }
  //IMPLEMENT_ME();
}

// Prints the current working directory to stdout
void run_pwd() {
  // COMPLETED: Kill all processes associated with a background job
  char currentWorkingDirectory[1024];
   if (getcwd(currentWorkingDirectory, sizeof(currentWorkingDirectory)) != NULL)
   {
     fprintf(stdout, "%s\n", currentWorkingDirectory);
   }
   else
   {
     printf("ERROR");
   }
  // IMPLEMENT_ME();
  // Flush the buffer before returning
  fflush(stdout);
}

// Prints all background jobs currently in the job list to stdout
void run_jobs() {
  // COMPLETED: Print background jobs
  int i = 0;
  int x = length_jobQueueStruct(&jobQueue);
  while(i < x) {
    jobType current = pop_front_jobQueueStruct(&jobQueue);
    print_job(current.id, peek_front_pidQueueStruct(&current.myQueue), current.cmd);
    push_back_jobQueueStruct(&jobQueue, current);
    i++;
  }
  // Flush the buffer before returning
  fflush(stdout);
}

/***************************************************************************
 * Functions for command resolution and process setup
 ***************************************************************************/

/**
 * @brief A dispatch function to resolve the correct @a Command variant
 * function for child processes.
 *
 * This version of the function is tailored to commands that should be run in
 * the child process of a fork.
 *
 * @param cmd The Command to try to run
 *
 * @sa Command
 */
void child_run_command(Command cmd) {
  CommandType type = get_command_type(cmd);

  switch (type) {
  case GENERIC:
    run_generic(cmd.generic);
    break;

  case ECHO:
    run_echo(cmd.echo);
    break;

  case PWD:
    run_pwd();
    break;

  case JOBS:
    run_jobs();
    break;

  case EXPORT:
  case CD:
  case KILL:
  case EXIT:
  case EOC:
    break;

  default:
    fprintf(stderr, "Unknown command type: %d\n", type);
  }
}

/**
 * @brief A dispatch function to resolve the correct @a Command variant
 * function for the quash process.
 *
 * This version of the function is tailored to commands that should be run in
 * the parent process (quash).
 *
 * @param cmd The Command to try to run
 *
 * @sa Command
 */
void parent_run_command(Command cmd) {
  CommandType type = get_command_type(cmd);

  switch (type) {
  case EXPORT:
    run_export(cmd.export);
    break;

  case CD:
    run_cd(cmd.cd);
    break;

  case KILL:
    run_kill(cmd.kill);
    break;

  case GENERIC:
  case ECHO:
  case PWD:
  case JOBS:
  case EXIT:
  case EOC:
    break;

  default:
    fprintf(stderr, "Unknown command type: %d\n", type);
  }
}

/**
 * @brief Creates one new process centered around the @a Command in the @a
 * CommandHolder setting up redirects and pipes where needed
 *
 * @note Processes are not the same as jobs. A single job can have multiple
 * processes running under it. This function creates a process that is part of a
 * larger job.
 *
 * @note Not all commands should be run in the child process. A few need to
 * change the quash process in some way
 *
 * @param holder The CommandHolder to try to run
 *
 * @sa Command CommandHolder
 */
 //changed from jobType to int. Made pidQueue a global var.
void create_process(CommandHolder holder) {
  //read flags from parser
  bool p_in  = holder.flags & PIPE_IN;
  bool p_out = holder.flags & PIPE_OUT;
  bool r_in  = holder.flags & REDIRECT_IN;
  bool r_out = holder.flags & REDIRECT_OUT;
  bool r_app = holder.flags & REDIRECT_APPEND;

  // COMPLETED: Remove warning silencers
  // (void) p_in;  // Silence unused variable warning
  // (void) p_out; // Silence unused variable warning
  // (void) r_in;  // Silence unused variable warning
  // (void) r_out; // Silence unused variable warning
  // (void) r_app; // Silence unused variable warning
  //COMPLETED: setting up pipes, redirects and processes
  //IMPLEMENT_ME()
  if(p_out) {
    pipe(pipes[pipeWhere % 2]);
  }

  pid_t pid = fork();
  push_back_pidQueueStruct(&pidQueue, pid);

  if (pid == 0) {
    // Is a child
    if (p_in) {
      dup2(pipes[(pipeWhere - 1) % 2][0], STDIN_FILENO);
      close(pipes[(pipeWhere - 1) % 2][0]);
    }

    if (p_out) {
      dup2(pipes[pipeWhere % 2][1], STDOUT_FILENO);
      close(pipes[pipeWhere % 2][1]);
    }

    if (r_in) {
      //note: not the same as REDIRECT_IN
      FILE *myFile = fopen(holder.redirect_in, "r");
      dup2(fileno(myFile), STDIN_FILENO);
    }

    if (r_out) {
      FILE *myFile;

      if (r_app) {
        myFile = fopen(holder.redirect_out, "a");
      }
      else {
        myFile = fopen(holder.redirect_out, "w");
      }

      dup2(fileno(myFile), STDOUT_FILENO);
    }

    child_run_command(holder.cmd);
    exit(0);
  }
  else {
    //Is a parent
    if(p_out) {
      close(pipes[pipeWhere % 2][1]);
    }

    parent_run_command(holder.cmd);
  }
}

// Run a list of commands
void run_script(CommandHolder* holders) {
  //first run_script() call only
  if(!isInitialized)
  {
    jobQueue = new_jobQueueStruct(1);
    isInitialized = true;
  }

  if (holders == NULL) {
    return;
  }

  check_jobs_bg_status();

  if (get_command_holder_type(holders[0]) == EXIT && get_command_holder_type(holders[1]) == EOC) {
    end_main_loop();
    destroy_jobQueueStruct(&jobQueue);
    return;
  }

  CommandType type;

  pidQueue = new_pidQueueStruct(1);

  //run commands in holder array
  for (int i = 0; (type = get_command_holder_type(holders[i])) != EOC; ++i) {
    pipeWhere = i;
    create_process(holders[i]);
  }

  if (!(holders[0].flags & BACKGROUND)) {
    // Not a background job.
    // COMPLETED: Wait for all processes under the job to complete
    //IMPLEMENT_ME();
    while(!is_empty_pidQueueStruct(&pidQueue))
    {
      int statID;
      //wait for processes completion
      waitpid(pop_front_pidQueueStruct(&pidQueue), &statID, 0);
    }
    destroy_pidQueueStruct(&pidQueue);
  }
  else {
    // A background job.
    // COMPLETED: Push the new job to the job queue
    //create new job!
    //IMPLEMENT_ME();

    struct jobType newJob;

    newJob.id = numJob;
    numJob = numJob + 1;
    newJob.cmd = get_command_string();
    newJob.myQueue = pidQueue;

    // COMPLETED: Once jobs are implemented, uncomment and fill the following line

    push_back_jobQueueStruct(&jobQueue, newJob);
    print_job_bg_start(newJob.id, peek_front_pidQueueStruct(&newJob.myQueue), newJob.cmd);
  }
}
