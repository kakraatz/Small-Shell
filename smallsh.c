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


void parse_input(char* args[]);
void builtin_commands(char* args[]);
//void other_commands(char* args[]);

int main() {
  struct input{
    char *args[512];
    char *in_file;
    char *out_file;
  };

  struct input *inp = malloc(sizeof *inp);
  
  // signal_handler();

  for (;;) {
    parse_input(inp->args);
    //printf("Input args[0] = %s\n", inp->args[0]);
    
    if (inp->args[0] == NULL || inp->args[0][0] == '#') {
      continue;
    }

    if (strcmp(inp->args[0], "exit") == 0 || strcmp(inp->args[0], "cd") == 0 || strcmp(inp->args[0], "status") == 0) {
      //printf("Should go to builtin_commands, input[0] = %s\n", inp->args[0]);
      //fflush(stdout);
      builtin_commands(inp->args);
    }

    else {
      //printf("Should fork to exec() here.\n");
      //fflush(stdout);
      //other_commands(input);
    }
  }
  return 0;
}


void parse_input(char *args[512]) {
  char input[2048];

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

  char *token;
  token = strtok(input, " ");
  if (token == NULL) {
    args[0] = NULL;
    return;
  }
  //printf("token = %s\n", token);
  
  int i;
  for (i = 0; token; ++i) {
    args[i] = strdup(token);
    printf("token is: %s\n", token);
    token = strtok(NULL, " ");
  }
}

void builtin_commands(char* args[]) {
  /* if the first argument is exit, run exit built-in command */
  if (strcmp(args[0], "exit") == 0) {
    printf("Should exit here.\n");
    fflush(stdout);

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


//void other_commands(char* args[]) {
//}

// void signal_handler() {
//}
}
