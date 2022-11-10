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
void builtin_commands(char* args[]);
void other_commands(char* args[], char *in_file, char *out_file, int bg);
//void signal_handler();
int exit_status(int status);
int *process_list(pid_t pid);

int main() {
  struct input{
    char *args[512];
    char *in_file;
    char *out_file;
    int bg;
  };

  struct input *inp = malloc(sizeof *inp);
  
  //signal_handler();

  for (;;) {
    // clear out the last input
    memset(inp, 0, sizeof(*inp));
    // send input to get tokenized into separate args
    parse_input(inp->args);

    inp->bg = 0;
    int i;
    for (i = 0; inp->args[i] != NULL; ++i) {
      // find < in the args and denote the next argument as input file
      if (strcmp(inp->args[i], "<") == 0) {
        inp->in_file = inp->args[i + 1];
        //printf("in file is: %s\n", inp->in_file);
      }
      // find > in the args and denote the next argument as output file
      else if (strcmp(inp->args[i], ">") == 0) {
        inp->out_file = inp->args[i + 1];
        //printf("out file is: %s\n", inp->out_file);
      }
      // find & and flag as background process
      else if (strcmp(inp->args[i], "&") == 0) {
        inp->bg = 1;
      }
    }
    //printf("bg value is: %d\n", inp->bg);
    // do nothing for blank input or a comment
    if (inp->args[0] == NULL || inp->args[0][0] == '#') {
      continue;
    }
    // if command is exit, cd, or status, go to built-in commands
    if (strcmp(inp->args[0], "exit") == 0 || strcmp(inp->args[0], "cd") == 0 || strcmp(inp->args[0], "status") == 0) {
      builtin_commands(inp->args);
    }
    // everything else gets forked
    else {
      other_commands(inp->args, inp->in_file, inp->out_file, inp->bg);
    }
  }
  return 0;
}


void parse_input(char *args[512]) {
  char input[2048];
  int i;

  printf(": ");
  fflush(stdout);
  fgets(input, 2048, stdin);

  int length = strlen(input);
  input[length - 1] = '\0';

  char pid[2048 - length];
  sprintf(pid, "%d", getpid());

  char *token;
  token = strtok(input, " ");
  if (token == NULL) {
    args[0] = NULL;
    return;
  }
  for (i = 0; token; ++i) {
    args[i] = strdup(token);
    if (args[i][0] == '$' && args[i][1] == '$') {
      args[i] = pid;
    }
    //printf("token is: %s\n", token);
    token = strtok(NULL, " ");
  }
}

void builtin_commands(char *args[]) {
  // exit built-in command
  if (strcmp(args[0], "exit") == 0) {
    int i;
    // get the list of all pid's
    int *processes = process_list(0);
    // kill em all
    for (i = 0; processes[i] != 0 ; ++i) {
      //printf("killing process id: %d\n", processes[i]);
      kill(processes[i], SIGTERM);
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
  else if (strcmp(args[0], "status") == 0) {
    printf("Should print exit status here.\n");
    fflush(stdout);

  }
}

void other_commands(char *args[], char *in_file, char *out_file, int bg) {
  printf("Starting other command\n");
  int i;
  for (i = 0; args[i] != NULL; ++i) {
    printf("argument %d: %s\n", i, args[i]);
  }
  printf("In-file is: %s\n", in_file);
  printf("Out-file is: %s\n", out_file);
  printf("Background flag is: %d\n", bg);
}

//void signal_handler() {
//}

int exit_status(int status) {
  int exit_value = 0;
  return exit_value;
}

int *process_list(pid_t pid) {
  static int list[2];
  static int count = 0;
  // add the pid to the list, increment pid counter, spit back the list
  list[count] = pid;
  ++count;
  return list;
}
