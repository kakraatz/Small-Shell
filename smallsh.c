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
void other_commands(char* args[]);

int main() {
  char* input[512];
  
  // signal_handler();

  for (;;) {
    parse_input(input);
    
    if (strncmp(input[0], "\n", 1) == 0 || strncmp(input[0], "#", 1) == 0) {
      continue;
    }

    else if (strcmp(input[0], "exit") == 0 || strcmp(input[0], "cd") == 0 || strcmp(input[0], "status") == 0) {
      builtin_commands(input);
    }

    else {
      other_commands(input);
    }
  }
  return 0;
}


void parse_input(char* args[]) {
  char input[2048];

  printf(": ");
  fflush(stdout);
  fgets(input, 2048, stdin);
}

void builtin_commands(char* args[]) {
  /* if the first argument is exit, run exit built-in command */
  if (strcmp(args[0], "exit") == 0) {
    printf("Should exit here.");
    fflush(stdout);
  }

  else if (strcmp(args[0], "cd") == 0) {
    printf("Should cd here.");
    fflush(stdout);
  }

  else if (strcmp(args[0], "status") == 0) {
    printf("Should print exit status here.");
    fflush(stdout);
  }


//void other_commands(char* args[]) {
//}

// void signal_handler() {
//}
}
