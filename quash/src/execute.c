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
static int n = 0;
static int p = -1;
static int pipes[2][2];

//bool for if jobQueue is isInitialized
bool isInitialized = false;

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
  //IMPLEMENT_ME();
}

// Returns the value of an environment variable env_var *****
const char* lookup_env(const char* env_var) {
  return getenv(env_var);
  //IMPLEMENT_ME();
}

// Check the status of background jobs
void check_jobs_bg_status() {
  // TODO: Check on the statuses of all processes belonging to all background
  // jobs. This function should remove jobs from the jobs queue once all
  // processes belonging to a job have completed.
  //IMPLEMENT_ME();
  int i =0;
  int j =0;
  //int status;

  //printf("%s\n", length_jobQueueStruct(&jobQueue));
  while(i < length_jobQueueStruct(&jobQueue)) {

    jobType current;
    current = pop_front_jobQueueStruct(&jobQueue);
    //current = peek_front_jobQueueStruct(&jobQueue);
    //pid_t current2 = peek_front_pidQueueStruct(&current.myQueue);
    pid_t tempPID = peek_front_pidQueueStruct(&current.myQueue);
    int x = length_pidQueueStruct(&current.myQueue);
    //printf("hello\n");
    //printf("%d\n", x);
      while(j < x) {

        /*pid_t pid = peek_front_pidQueueStruct(&current.myQueue);
        pid_t check = waitpid(pid, &status, 0);
        if(check == -1) {
          exit(EXIT_FAILURE);
          //error
        }
        else if( pid == check ) {
          //done
          pop_front_pidQueueStruct(&current.myQueue);
        }
        else {
          //leave alone
        }*/
        pid_t myPID = pop_front_pidQueueStruct(&current.myQueue);
        int statID;
        //pid_t check = waitpid(myPID, &statID, 0);
        if(waitpid(myPID, &statID, 0) == -1) {
          //printf("error\n");
          exit(EXIT_FAILURE);
        }
        else if(waitpid(myPID, &statID, 0) > 0) {
          //printf("done\n");
          //pop_front_pidQueueStruct(&current.myQueue);
          //done
        }
        else {
          //printf("keep waiting\n");
          push_back_pidQueueStruct(&current.myQueue, myPID);
        }

        j++;
      }
      if(is_empty_pidQueueStruct(&current.myQueue)) {
        print_job_bg_complete(current.id, tempPID, current.cmd);
        free(current.cmd);
        destroy_pidQueueStruct(&current.myQueue);
      }
      else {
        //pop_front_jobQueueStruct(&jobQueue);
        push_back_jobQueueStruct(&jobQueue, current);
      }
    i++;
  }
  //IMPLEMENT_ME();

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
  while(*str != NULL) {
    printf("%s ", *str);
    ++str;
  }
  printf("\n");
  //IMPLEMENT_ME();
  fflush(stdout);
}

// Sets an environment variable *****
void run_export(ExportCommand cmd) {
  // Write an environment variable
  const char* env_var = cmd.env_var;
  const char* val = cmd.val;

  if(setenv(env_var, val, 1)) {
    fprintf(stderr, "ERROR: Failed to update environment variable, %s, to value, %s\n", env_var, val);
    exit(EXIT_FAILURE);
  }
  IMPLEMENT_ME();
}

// Changes the current working directory *****
void run_cd(CDCommand cmd) {
  const char* dir = cmd.dir;

  if(dir == NULL) {
    return;
  }

  if(chdir(dir) != 0) {
    return;
  }

  const char* temp = getenv("PWD");

  if(setenv("PREV_PWD", temp, 1) != 0) {
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

  // TODO: Remove warning silencers
  (void) signal; // Silence unused variable warning
  (void) job_id; // Silence unused variable warning

  // TODO: Kill all processes associated with a background job
  IMPLEMENT_ME();

  // struct jobType myJob;
  // /*for(int i = 0; i < length; i++) {
  //
  // }*/

}

// Prints the current working directory to stdout
void run_pwd() {
    char currentWorkingDirectory[1024];
     if (getcwd(currentWorkingDirectory, sizeof(currentWorkingDirectory)) != NULL)
         fprintf(stdout, "%s\n", currentWorkingDirectory);
     else
         printf("pwd error");
  // IMPLEMENT_ME();
  // Flush the buffer before returning
  fflush(stdout);
}

// Prints all background jobs currently in the job list to stdout
void run_jobs() {
  // TODO: Print background jobs
  //IMPLEMENT_ME();
  int i = 0;
  int x = length_jobQueueStruct(&jobQueue);
  //printf("hello\n");
  //printf("%d\n", x);
  while(i < x) {
    jobType current = pop_front_jobQueueStruct(&jobQueue);
    //fprintf(stdout, "%s\n", print_job(current.id, current.pid, current.cmd));
    //print_job(current.id, current.pid, current.cmd);
    printf("[%d]\t%8d\t%s\n", current.id, peek_front_pidQueueStruct(&current.myQueue), current.cmd);
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
void create_process(CommandHolder holder, jobType* current) {
  //read flags from parser
  bool p_in  = holder.flags & PIPE_IN;
  bool p_out = holder.flags & PIPE_OUT;
  bool r_in  = holder.flags & REDIRECT_IN;
  bool r_out = holder.flags & REDIRECT_OUT;
  bool r_app = holder.flags & REDIRECT_APPEND;

  //setting up pipes, redirects and processes
  if(p_out) {
    pipe(pipes[n]);
  }
  pid_t pid = fork();

  if (pid == 0) {
    // Is a child
    if (p_in) {
      dup2(pipes[p][0], STDIN_FILENO);
      close(pipes[p][0]);
    }

    if (p_out) {
      dup2(pipes[n][1], STDOUT_FILENO);
      close(pipes[n][1]);
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
      close(pipes[n][1]);
    }
    if(p_in) {
      close(pipes[p][0]);
    }

    n = (n + 1) % 2;
    p = (p + 1) % 2;
    
    push_back_pidQueueStruct(&(current->myQueue), pid);
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

  //create a new job!
  jobType newJob;

  newJob.id = length_jobQueueStruct(&jobQueue) + 1;
  newJob.myQueue = new_pidQueueStruct(1);
  newJob.cmd = get_command_string();

  //run commands in holder array
  for (int i = 0; (type = get_command_holder_type(holders[i])) != EOC; ++i) {
    //testing if we need to pass in the new job
    create_process(holders[i], &newJob);
    //create_process(holders[i]);

  }

  if (!(holders[0].flags & BACKGROUND)) {
    // Not a background job.
    while(!is_empty_pidQueueStruct(&newJob.myQueue))
    {
      int statID;
      //wait for processes completion
      waitpid(pop_front_pidQueueStruct(&newJob.myQueue), &statID, 0);
    }
    free(newJob.cmd);
    destroy_pidQueueStruct(&newJob.myQueue);
  }
  else {
    // A background job.
    // Push the new job to the job queue
    push_back_jobQueueStruct(&jobQueue, newJob);
    print_job_bg_start(newJob.id, peek_front_pidQueueStruct(&newJob.myQueue), newJob.cmd);
  }
}
