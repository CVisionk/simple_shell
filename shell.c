#include "main.h"


/**
 * is_builtin_command - Checks if the command is a built-in command.
 *
 * @command: The command to check.
 *
 * Return: 1 if the command is built-in, 0 otherwise.
 */
int is_builtin_command(char *command)
{
	return (strcmp(command, "cd") == 0);
}

/**
 * handle_builtin_commands - Handles built-in commands.
 *
 * @args: The arguments for the command.
 */
void handle_builtin_commands(char *args[])
{
	if (strcmp(args[0], "cd") == 0)
	{
		if (args[1] == NULL || chdir(args[1]) != 0)
		{
			perror("cd");
		}
	}
}

/**
 * main - Main shell loop.
 *
 * This function repeatedly prompts the user for a command, executes it,
 * and handles built-in commands. It terminates when the user enters "exit".
 *
 * Return: Always returns 0.
 */
int main(void)
{
	char *buffer = NULL;
	size_t bufsize = BUFFER_SIZE;
	ssize_t getline_status;

	buffer = (char *)malloc(bufsize * sizeof(char));
	if (buffer == NULL)
	{
		perror("Unable to allocate buffer");
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		print_prompt();

		getline_status = read_input(&buffer, &bufsize);
		if (getline_status == -1)
		{
			if (feof(stdin))
			{
				break;
			}
			else
			{
				perror("getline");
				continue;
			}
		}

		execute_commands(buffer);
	}

	free(buffer);
	return (0);
}


/**
 * read_input - Reads input from stdin.
 *
 * @buffer: Pointer to the buffer to store input.
 * @bufsize: Pointer to the size of the buffer.
 *
 * Return: The number of characters read (including newline), or -1 on failure.
 */
ssize_t read_input(char **buffer, size_t *bufsize)
{
	return (getline(buffer, bufsize, stdin));
}


/**
 * execute_commands - Executes the given command.
 *
 * @buffer: The command to execute.
 */
void execute_commands(char *buffer)
{
	char *args[MAX_ARGS];

	tokenize_input(buffer, args);

	if (args[0] == NULL)
	{
		return;
	}

	if (is_builtin_command(args[0]))
	{
		handle_builtin_commands(args);
	}
	else
	{
		pid_t pid = fork();

		if (pid == -1)
		{
			perror("fork");
			exit(EXIT_FAILURE);
		}
		else if (pid == 0)
		{
			search_and_execute(args[0], args);
			exit(EXIT_FAILURE);
		}
		else
		{
			waitpid(pid, NULL, 0);
		}
	}
}
