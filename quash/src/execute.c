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



int numJob = 0;
bool isInitialized = false;
static int pipes[2][2];

IMPLEMENT_DEQUE_STRUCT(pidQueueStruct, pid_t);
IMPLEMENT_DEQUE(pidQueueStruct, pid_t);

pidQueueStruct pidQueue;

typedef struct jobType {
  int id;
  char* cmd;
  pidQueueStruct myQueue;
  pid_t pid;
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
  char* cwd[1024];
  // Change this to true if necessary
  *should_free = false;
  return getcwd(cwd, sizeof(cwd));
  IMPLEMENT_ME();
}

// Returns the value of an environment variable env_var *****
const char* lookup_env(const char* env_var) {
  return getenv(env_var);
  IMPLEMENT_ME();
}

// Check the status of background jobs
void check_jobs_bg_status() {
  // TODO: Check on the statuses of all processes belonging to all background
  // jobs. This function should remove jobs from the jobs queue once all
  // processes belonging to a job have completed.
  //IMPLEMENT_ME();
  int i =0;
  int j =0;
  int status;


  while(i < length_jobQueueStruct(&jobQueue))
  {
    jobType current;
    current = peek_front_jobQueueStruct(&jobQueue);

    //pidLength = length_pidQueueStruct(&pid);

  //  if (current.myQueue
      while(j < length_pidQueueStruct(&current.myQueue))
      {
        // current.waitpid
        pid_t current2 = peek_front_pidQueueStruct(&current.myQueue);
        pid_t check = waitpid(current2, &status, 0);
        if(check == -1)
        {
          //error
        }
        else if( current2 == check )
        {
          //done
          pop_front_pidQueueStruct(&current.myQueue);
        }
        else
        {
          //leave alone
        }
        j++;
      }

    i++;
  }


  // while(i < length_jobQueueStruct(&jobQueue))
  // {
  //   struct jobType current = peek_front_jobQueueStruct(&jobQueue);
  //   print_job(current.id, current.pid, current.cmd);
  // }
  //printf("CHECK JOBS STATUS FLAG\n");



  IMPLEMENT_ME();

  // TODO: Once jobs are implemented, uncomment and fill the following line
  // print_job_bg_complete(job_id, pid, cmd);
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
  char* exec = cmd.args[0];
  char** args = cmd.args;
  execvp(exec, args);
  //fprintf(stderr, "ERROR: Failed to execute the program: %s", exec);
  exit(EXIT_FAILURE);
  //perror("ERROR: Failed to execute program");
  IMPLEMENT_ME();
}

// Print strings *****
void run_echo(EchoCommand cmd) {
  char** str = cmd.args;

  //printf("ECHO CHECK\n");
  while(*str != '\0') {
    printf("%s ", *str);
    ++str;
  }
  printf("\n");
  IMPLEMENT_ME();
  fflush(stdout);
}

// Sets an environment variable *****
void run_export(ExportCommand cmd) {
  // Write an environment variable
  const char* env_var = cmd.env_var;
  const char* val = cmd.val;

  if(setenv(env_var, val, 1) == -1) {
    //fprintf(stderr, "ERROR: Failed to update environment variable, %s, to value, %s\n", env_var, val);
    exit(EXIT_FAILURE);
  }
  IMPLEMENT_ME();
}

// Changes the current working directory *****
void run_cd(CDCommand cmd) {
  const char* dir = cmd.dir;

  if(dir == NULL) {
    perror("ERROR: Failed to resolve path");
    exit(EXIT_FAILURE);
  }

  char* cwd = get_current_directory(NULL);
  setenv("PREV_PWD", cwd, 1);
  if(chdir(dir) == -1) {
  //  fprintf("ERROR: Failed to go to directory, %s\n", dir);
    exit(EXIT_FAILURE);
  }
  free(cwd);
  cwd = get_current_directory(NULL);
  if(setenv("PWD", cwd, 1) == -1) {
    //fprintf("ERROR: Failed to update PWD variable, %s\n", cwd);
    exit(EXIT_FAILURE);
  }
  IMPLEMENT_ME();
  free(cwd);
}

// Sends a signal to all processes contained in a job *****
void run_kill(KillCommand cmd) {
  int signal = cmd.sig;
  int job_id = cmd.job;

  struct jobType myJob;
  /*for(int i = 0; i < length; i++) {

  }*/
  IMPLEMENT_ME();

}


// Prints the current working directory to stdout
void run_pwd() {
  // TODO: Print the current working directory
  IMPLEMENT_ME();


  // Flush the buffer before returning
  fflush(stdout);
}

// Prints all background jobs currently in the job list to stdout
void run_jobs() {
  // TODO: Print background jobs
  IMPLEMENT_ME();

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
void create_process(CommandHolder holder) {
  // Read the flags field from the parser *****
  //printf("CHECK CREATE PROCESS 1\n");
  bool p_in  = holder.flags & PIPE_IN;
  bool p_out = holder.flags & PIPE_OUT;
  bool r_in  = holder.flags & REDIRECT_IN;
  bool r_out = holder.flags & REDIRECT_OUT;
  bool r_app = holder.flags & REDIRECT_APPEND;

  IMPLEMENT_ME();
  //printf("CHECK CREATE PROCESS 2\n");

  if(p_out) {
    pipe(pipes[1]);
  }
  //printf("CHECK CREATE PROCESS 3\n");

  pid_t pid = fork();

  //printf("CHECK CREATE PROCESS 4\n");

  push_back_pidQueueStruct(&pidQueue, pid);

  //printf("CHECK CREATE PROCESS 5\n");

  if(pid != 0) {
    if(p_out) {
      close(pipes[1][1]);
    }
  }
  else {
    if(p_in) {
      dup2 (pipes[0][0], STDIN_FILENO);
      close (pipes[0][0]);
    }
    if(p_out) {
      dup2 (pipes[1][1], STDOUT_FILENO);
      close (pipes[1][1]);
    }
    if(r_in) {
      FILE* myFile = fopen(holder.redirect_in, "r");
      dup2(fileno(myFile), STDIN_FILENO);
    }
    if(r_out) {
      FILE* myFile;
      if(r_app) {
        myFile = fopen(holder.redirect_out, "a");
      }
      else {
        myFile = fopen(holder.redirect_out, "w");
      }
      dup2(fileno(myFile), STDOUT_FILENO);
    }
    child_run_command(holder.cmd);
  }
  //printf("CHECK CREATE PROCESS 6\n");
  parent_run_command(holder.cmd);
}

// Run a list of commands *****jobQueue
void run_script(CommandHolder* holders) {
  pidQueue = new_pidQueueStruct(1);

  if(!isInitialized) {
    jobQueue = new_jobQueueStruct(1);
  }

  if (holders == NULL) {
    return;
  }

  check_jobs_bg_status();

  if (get_command_holder_type(holders[0]) == EXIT && get_command_holder_type(holders[1]) == EOC) {
    end_main_loop();
    return;
  }

  //printf("CHECK 1\n");

  CommandType type;

  //printf("CHECK 2\n");

  // Run all commands in the `holder` array
  for (int i = 0; (type = get_command_holder_type(holders[i])) != EOC; ++i) {
    create_process(holders[i]);
  }

  //printf("CHECK 3\n");

  if (!(holders[0].flags & BACKGROUND)) {
    //printf("CHECK 4\n");
    while(!is_empty_pidQueueStruct(&pidQueue)) {
      //printf("CHECK 5\n");
      int status;
      pid_t pidCurrent = pop_front_pidQueueStruct(&pidQueue);
      waitpid(pidCurrent, &status, 0);
      //printf("CHECK 6\n");
    }
    //printf("CHECK 7\n");
    destroy_pidQueueStruct(&pidQueue);
    IMPLEMENT_ME();
  }
  else {
  //  printf("CHECK 8\n");
    struct jobType newJob;
    newJob.id = numJob;
    numJob++;
    newJob.cmd = get_command_string();
    newJob.pid = peek_back_pidQueueStruct(&pidQueue);

    IMPLEMENT_ME();
    //printf("CHECK 9\n");
    push_back_jobQueueStruct(&jobQueue, newJob);
    print_job_bg_start(newJob.id, newJob.pid, newJob.cmd);
  }
}
