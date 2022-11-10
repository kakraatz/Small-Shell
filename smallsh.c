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
void builtin_commands(char* args[], char *in_file, char *out_file);
//void other_commands(char* args[]);
//void signal_handler();
//char process_list();

int main() {
  struct input{
    char *args[512];
    char *in_file;
    char *out_file;
  };

  struct input *inp = malloc(sizeof *inp);
  
  //signal_handler();

  for (;;) {
    // clear out the last input
    memset(inp, 0, sizeof(*inp));
    // send input to get tokenized
    parse_input(inp->args);
    int i;
    for (i = 0; inp->args[i] != NULL; ++i) {
      printf("Input args = %s\n", inp->args[i]);
    }

    for (i = 0; inp->args[i] != NULL; ++i) {
      if (strcmp(inp->args[i], "<") == 0) {
        inp->in_file = inp->args[i + 1];
        printf("in file is: %s\n", inp->in_file);
      }
      else if (strcmp(inp->args[i], ">") == 0) {
        inp->out_file = inp->args[i + 1];
        printf("out file is: %s\n", inp->out_file);
      }
    }
    // do nothing for blank argument or a comment
    if (inp->args[0] == NULL || inp->args[0][0] == '#') {
      continue;
    }
    // if command is exit, cd, or status, send it to built-in commands
    if (strcmp(inp->args[0], "exit") == 0 || strcmp(inp->args[0], "cd") == 0 || strcmp(inp->args[0], "status") == 0) {
      //printf("Should go to builtin_commands, input[0] = %s\n", inp->args[0]);
      //fflush(stdout);
      builtin_commands(inp->args, inp->in_file, inp->out_file);
    }
    // everything else gets forked
    else {
      //printf("Should fork to exec() here.\n");
      //fflush(stdout);
      //other_commands(inp->args, inp->in_file, inp->out_file);
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
  //if (length == 1) {
    //args[0] = NULL;
    //return;
  //}
  //printf("input length = %d", length);
  //printf("input[length-1] = %c, ", input[length-1]);
  input[length - 1] = '\0';
  //printf("should be null = %c", input[length - 1]);
  char pid[2048 - length];
  sprintf(pid, "%d", getpid());

  char *token;
  token = strtok(input, " ");
  if (token == NULL) {
    args[0] = NULL;
    return;
  }
  //printf("token = %s\n", token);
  for (i = 0; token; ++i) {
    args[i] = strdup(token);
    if (args[i][0] == '$' && args[i][1] == '$') {
      args[i] = pid;
    }
    printf("token is: %s\n", token);
    token = strtok(NULL, " ");
  }
}

void builtin_commands(char *args[], char *in_file, char *out_file) {
  /* if the first argument is exit, run exit built-in command */
  if (strcmp(args[0], "exit") == 0) {
    printf("Should exit here.\n");
    fflush(stdout);
    int i;
    //char processes = process_list(0);
    //for (i = 0; processes != 0 ; ++i) {
      //kill(processes[i], SIGTERM);
    //}
  }

  else if (strcmp(args[0], "cd") == 0) {
    printf("Should cd here.\n");
    fflush(stdout);
    if (!args[1]) {
      printf("cd'ing to home\n");
      chdir(getenv("HOME"));
      printf("cd'd to home\n");
    }
    else {
      printf("cd'ing to %s\n", args[1]);
      chdir(args[1]);
      printf("cd'd to %s\n", args[1]);
    }
  }

  else if (strcmp(args[0], "status") == 0) {
    printf("Should print exit status here.\n");
    fflush(stdout);
  }
}


//void other_commands(char* args[]) {
//}

//void signal_handler() {
//}

//char process_list() {
//}
