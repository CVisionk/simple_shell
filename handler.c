#include "main.h"


/**
 * tokenize_input - Tokenizes the input command and arguments.
 *
 * @buffer: The input buffer containing the command and arguments.
 * @args: An array to store the tokenized arguments.
 *
 * Return: The number of arguments tokenized.
 */
int tokenize_input(char *buffer, char *args[])
{
	char *token;
	int arg_count = 0;

	token = strtok(buffer, " \t\n");
	while (token != NULL && arg_count < MAX_ARGS - 1)
	{
		args[arg_count++] = token;
		token = strtok(NULL, " \t\n");
	}
	args[arg_count] = NULL;

	return (arg_count);
}

/**
 * execute_command - Executes a command with arguments using execve.
 *
 * @command: The command to be executed.
 * @args: The arguments for the command.
 *
 * Return: This function does not return if successful.
 *         Otherwise, it exits with an error message.
 */
void execute_command(char *command, char *args[])
{
	if (execve(command, args, environ) == -1)
	{
		fprintf(stderr, "%s: %s\n", command, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

/**
 * search_and_execute - Searches for the executable in PATH and executes it.
 *
 * @command: The command to be executed.
 * @args: The arguments for the command.
 *
 * This function searches for the executable file in the PATH environment
 * variable and executes it if found. Otherwise, it prints an error message.
 */
void search_and_execute(char *command, char *args[])
{
	char *path = getenv("PATH");
	char *dir;
	char filepath[256];

	if (strchr(command, '/'))
	{
		execute_command(command, args);
		return;
	}



	if (path != NULL)
	{
		char *path_copy = strdup(path);

		dir = strtok(path_copy, ":");
		while (dir != NULL)
		{
			snprintf(filepath, sizeof(filepath), "%s/%s", dir, command);
			if (access(filepath, X_OK) == 0)
			{
				execute_command(filepath, args);
				free(path_copy);
				return;
			}
			dir = strtok(NULL, ":");
		}
		free(path_copy);
	}

	fprintf(stderr, "%s: command not found\n", command);
}
/**
 * print_prompt - Prints the shell prompt.
 */
void print_prompt(void)
{
	if (isatty(STDIN_FILENO))
	{
		printf("$ ");
		fflush(stdout);
	}
}
