/* Author: Kevin Kraatz 
 * CS344 Fall 2022
 * Assignment 3 - smallsh */


#define _POSIX_C_SOURCE 200809L


#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>


void parse_input(char *args[]);
void builtin_commands(char* args[], int last_status);
void other_commands(char* args[], char *in_file, char *out_file, int bg, struct sigaction, struct sigaction);
void check_bg_processes();
void handle_SIGTSTP();
void fgmode_on();
void fgmode_off();
int *process_list(pid_t pid);


static int fg_mode = 0;
static int last_status = 0;


int main() {
  // initialize a struct to store input values
  struct input{
    char *args[512];
    char *in_file;
    char *out_file;
    int bg;
  };

  // allocate memory for the input struct
  struct input *inp = malloc(sizeof *inp);
  
  // ctrl-C handler, ignores ctrl-C for parent/bg children
  // taken from signals module exploration
  // using SIG_IGN per ed discussions
  struct sigaction SIGINT_action = {0};
  SIGINT_action.sa_handler = SIG_IGN;
  sigfillset(&SIGINT_action.sa_mask);
  SIGINT_action.sa_flags = 0;
  sigaction(SIGINT, &SIGINT_action, NULL);

  // ctrl-Z handler, controls foreground-only mode
  // using custom handler and sigprocmask per discord/ed discussions
  struct sigaction SIGTSTP_action = {0};
  SIGTSTP_action.sa_handler = handle_SIGTSTP;
  sigfillset(&SIGTSTP_action.sa_mask);
  SIGTSTP_action.sa_flags = 0;
  sigaction(SIGTSTP, &SIGTSTP_action, NULL);

  for (;;) {
    // clear out the last input struct
    memset(inp, 0, sizeof(*inp));

    // input gets tokenized into separate args
    parse_input(inp->args);

    // background value initialized to 0
    // bg is 1 if & was the last argument
    inp->bg = 0;

    // loop through the parsed arguments
    int i;
    for (i = 0; inp->args[i] != NULL; ++i) {
      // find < in the args and denote the next argument as input file
      if (strcmp(inp->args[i], "<") == 0) {
        inp->in_file = strdup(inp->args[i + 1]);
      }

      // find > in the args and denote the next argument as output file
      else if (strcmp(inp->args[i], ">") == 0) {
        inp->out_file = strdup(inp->args[i + 1]);
      }

      // if & is found, flag as background process/set bg to 1
      // this only occurs if & is located at the end of the command
      else if (strcmp(inp->args[i], "&") == 0 && (inp->args[i + 1] == NULL)) {
        inp->bg = 1;
        // null the & index position after flagging as bg process
        inp->args[i] = NULL;
      }
    }
    // loop through and remove the <, >, in_file, and out_file from args
    for (i = 0; inp->args[i] != NULL; ++i) {
      if (strcmp(inp->args[i], "<") == 0) {
        inp->args[i] = NULL;
        inp->args[i + 1] = NULL;
      }

      else if (strcmp(inp->args[i], ">") == 0) {
        inp->args[i] = NULL;
        inp->args[i + 1] = NULL;
      }
    }

    // do nothing for blank input or a comment
    if (inp->args[0] == NULL || inp->args[0][0] == '#') {
      continue;
    }
    // if command is exit, cd, or status, go to built-in commands
    if (strcmp(inp->args[0], "exit") == 0 || strcmp(inp->args[0], "cd") == 0 || strcmp(inp->args[0], "status") == 0) {
      builtin_commands(inp->args, last_status);
    }
    // everything else gets forked
    else {
      other_commands(inp->args, inp->in_file, inp->out_file, inp->bg, SIGINT_action, SIGTSTP_action);
    }
  }
  return 0;
}


void parse_input(char *args[512]) {
  // initialize array to store input from user
  char input[2048];
  int i;
  
  // check for any finished background processes before taking next input
  check_bg_processes();

  // print the input indicator and flush
  printf(": ");
  fflush(stdout);

  // using fgets per discord discussion
  // collecting stdin into input array
  fgets(input, 2048, stdin);

  // get the input string length and set the last index character to \0 from \n
  int length = strlen(input);
  input[length - 1] = '\0';

  // convert the pid into a str using sprintf
  char pid[2048 - length];
  sprintf(pid, "%d", getpid());

  // using strstr and memcpy for expansion per class discord discussions
  // Note to grader: this is not working exactly to spec as I didn't want to
  // copy the video that was posted on discord. If there are more than 2 $'s in
  // an argument, extraneous $'s are removed from the argument
  char *exp = strstr(input, "$$");
  if (exp != NULL) {
    memcpy(exp, pid, strlen(pid) + 1);
  }

  // tokenize the input into individual arguments
  char *token;
  token = strtok(input, " ");

  // if input is blank, just return
  if (token == NULL) {
    args[0] = NULL;
    return;
  }

  // loop through and put each argument token into the respective args index position
  for (i = 0; token != NULL; ++i) {
    args[i] = strdup(token);
    token = strtok(NULL, " ");
  }
}

void builtin_commands(char *args[512], int last_status) {
  // exit built-in command
  if (strcmp(args[0], "exit") == 0) {
    int i;
    // get the list of all pid's
    int *processes = process_list(0);
    // kill them all if they haven't terminated already
    for (i = 0; i < 10 ; ++i) {
      //printf("killing process id: %d\n", processes[i]);
      if (processes[i] != 0) {
        kill(processes[i], SIGTERM);
      }
    }
    // clean exit after any/all processes are killed
    exit(0);
    }
  
  // cd built-in command
  else if (strcmp(args[0], "cd") == 0) {
    // if no argument/directory after cd, chdir to home
    if (!args[1]) {
      chdir(getenv("HOME"));
    }
    // otherwise, cd to the given directory
    else {
      chdir(args[1]);
    }
  }
  
  // status built-in commmand
  // built-in commands do not change status value
  // code sourced from monitoring child processes exploration
  else if (strcmp(args[0], "status") == 0) {
    if (WIFEXITED(last_status)) {
      printf("exit value %d\n", WEXITSTATUS(last_status));
      fflush(stdout);
    }
    else {
      printf("terminated by signal %d\n", WTERMSIG(last_status));
      fflush(stdout);
    }
  }
}

void other_commands(char *args[512], char *in_file, char *out_file, int bg, struct sigaction SIGINT_action, struct sigaction SIGTSTP_action) {
  // Code sourced from process api modules and exploration: processes and i/o
  pid_t spawnpid = -5;
  pid_t bg_pid;
  int childStatus;
  spawnpid = fork();
  switch (spawnpid) {
    case -1:  // error if fork fails
      perror("Fork() failed.");
      fflush(stderr);
      exit(1);
    case 0:  // execute child process
      // add the child pid to the counter/list
      //process_list(spawnpid);

      // reset signal handler for SIGINT in child processes to default action
      SIGINT_action.sa_handler = SIG_DFL;
      sigaction(SIGINT, &SIGINT_action, NULL);

      // reset signal handler for SIGTSTP in child processes to ignore
      SIGTSTP_action.sa_handler = SIG_IGN;
      sigaction(SIGTSTP, &SIGTSTP_action, NULL);

      // if the input file exists
      if (in_file != NULL) {
        // open the input file
        int sourceFD = open(in_file, O_RDONLY);
        if (sourceFD == -1) {
          perror("Input file open failed.");
          fflush(stderr);
          exit(1);
        }
        // redirect stdin to input file
        int result = dup2(sourceFD, 0);
        if (result == -1) {
          perror("Input file assignment failed.");
          fflush(stderr);
          exit(2);
        }
        // from processes and i/o module
        // sourcefd will close upon exec call
        fcntl(sourceFD, F_SETFD, FD_CLOEXEC);
      }

      // if output file exists
      if (out_file != NULL) {
        // open the output file
        int targetFD = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (targetFD == -1) {
          perror("Output file open failed.");
          fflush(stderr);
          exit(1);
        }
        // redirect stdout to output file
        int result = dup2(targetFD, 1);
        if (result == -1) {
          perror("Output file assignment failed.");
          fflush(stderr);
          exit(2);
        }
        fcntl(targetFD, F_SETFD, FD_CLOEXEC);
      }
      // execute the command
      fflush(stdout);
      int execute = execvp(args[0], (char*const*)args);
      if (execute == -1) {
        perror("Error executing command.");
        fflush(stderr);
        exit(1);
      }
    default: // parent process
      if (bg == 1 && fg_mode == 0) {
        pid_t bgpid_value = waitpid(spawnpid, &childStatus, WNOHANG);
        printf("background pid is %d\n", spawnpid);
        fflush(stdout);
        last_status = childStatus;
        bg_pid = spawnpid;
        process_list(bg_pid);
      }

      else {
        pid_t waitpid_value = waitpid(spawnpid, &childStatus, 0);
        last_status = childStatus;
      }
      check_bg_processes();
   }
}

void check_bg_processes() {
  // exit status code sourced from monitoring child processes exploration
  int i;
  int childStatus;
  int *bg_list = process_list(0);
  for (i = 0; i < 10; ++i) {
    if (bg_list[i] != 0) {
      pid_t bgpid_value = waitpid(bg_list[i], &childStatus, WNOHANG);
      if (bgpid_value > 0) {
        printf("background pid %d is done: ", bg_list[i]);
        fflush(stdout);
        if (WIFEXITED(childStatus)) {
          printf("exit value %d\n", WEXITSTATUS(childStatus));
          fflush(stdout);
        }
        else {
          printf("terminated by signal %d\n", WTERMSIG(childStatus));
          fflush(stdout);
        }
        last_status = childStatus;
      }
    }
  }
}

void handle_SIGTSTP() {
  // formatting this based on the signals ed post
  // using sigprocmask
  if (fg_mode == 0) {
    fgmode_on();
  }
  else if (fg_mode == 1) {
    fgmode_off();
  }
}

void fgmode_on() {
  // code from signal handling exploration and signals ed post pseudocode
  fg_mode = 1;
  char* message = "\nEntering foreground-only mode (& is now ignored)\n";
  write(STDOUT_FILENO, message, 50);
  fflush(stdout);
}

void fgmode_off() {
  // code from signal handling exploration and signals ed post pseudocode
  fg_mode = 0;
  char* message = "\nExiting foreground-only mode\n";
  write(STDOUT_FILENO, message, 30);
  fflush(stdout);
}

int *process_list(pid_t pid) {
  // storing an array of the non-zero pids of background processes
  static int list[10];
  static int count = 0;
  if (pid != 0) {
  // add the pid to the list, increment pid counter, spit back the list
    list[count] = pid;
    ++count;
  }
  return list;
}
