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


void parse_input(char* args[]);
void builtin_commands(char* args[]);
//void other_commands(char* args[]);

int main() {
  char* input[512];
  
  // signal_handler();

  for (;;) {
    parse_input(input);
    
    if (input[0] == NULL || input[0][0] == '#') {
      continue;
    }

    if (strcmp(input[0], "exit") == 0 || strcmp(input[0], "cd") == 0 || strcmp(input[0], "status") == 0) {
      printf("Should go to builtin_commands, input[0] = %s\n", input[0]);
      fflush(stdout);
      builtin_commands(input);
    }

    else {
      printf("Should fork to exec() here.\n");
      fflush(stdout);
      //other_commands(input);
    }
  }
  return 0;
}


void parse_input(char* args[]) {
  char input[2048];

  printf(": ");
  fflush(stdout);
  fgets(input, 2048, stdin);

  int length = strlen(input);
  //printf("input[length] = %c, ", input[length-2]);
  input[length - 1] = '\0';

  char *token;
  token = strtok(input, " ");
  //printf("token = %s", token);

  args[0] = token;
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
