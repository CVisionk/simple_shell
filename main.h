#ifndef main_H
#define main_H

#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_ARGS 64
#define BUFFER_SIZE 1024

extern char **environ;

void execute_command(char *command, char *args[]);
void search_and_execute(char *command, char *args[]);
void handle_builtin_commands(char *args[]);
int is_builtin_command(char *command);

#endif
