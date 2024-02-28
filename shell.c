#include "main.h"

#define MAX_INPUT_LENGTH 1024
#define MAX_ARGS 64

/**
 * splitter - split input into array of strings.
 * @input: input to split
 *
 * Return: array of strings
 */
char **splitter(char *input)
{
	char *token;
	int arg_index;
	char **args = malloc(MAX_ARGS * sizeof(char *));

	if (args == NULL)
	{
		fprintf(stderr, "Memory allocation failed\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(input, " ");
	arg_index = 0;
	while (token != NULL && arg_index < MAX_ARGS - 1)
	{
		args[arg_index] = strdup(token);
		if (args[arg_index] == NULL)
		{
			fprintf(stderr, "Memory allocation failed\n");
			exit(EXIT_FAILURE);
		}
		arg_index++;
		token = strtok(NULL, " ");
	}
	args[arg_index] = NULL;
	return (args);
}


/**
 * free_args - free memory
 * @args: memory to free
 *
 * Returns: void
 */
void free_args(char **args)
{
	int i;

	for (i = 0; args[i] != NULL; i++)
	{
		free(args[i]);
	}
	free(args);
}


/**
 * execute_command - execute args
 * @args: arguments to execute
 *
 * Returns: void
 */
void execute_command(char **args)
{
	pid_t pid = fork();

	if (pid < 0)
	{
		fprintf(stderr, "Fork failed\n");
		exit(EXIT_FAILURE);
	}
	else if (pid == 0)
	{
		/* Child process */
		if (execve(args[0], args, NULL) == -1)
		{
			fprintf(stderr, "./shell: %s: No such file or directory\n", args[0]);
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		/* Parent process */
		wait(NULL); /* Wait for child to finish */
	}
}



/**
 * main - Entry point of the program
 * @ac: The number of command-line arguments
 * @av: An array of pointers to the arguments
 *
 * Return: Always returns 0
 */
int main(int ac, char **av)
{
	if (ac == 2)
	{
		printf("%s", *av);
	}
	else
	{
		size_t len = 0;
		ssize_t read;
		char *line = NULL;
		char **args;

		printf("$ ");
		while ((read = getline(&line, &len, stdin)) != -1)
		{
			if (read == -1 && len == 0)
			{
				fprintf(stderr, "Memory allocation failed\n");
				exit(EXIT_FAILURE);
			}
			line[strcspn(line, "\n")] = 0;

			args = splitter(line);

			if (args[1] == NULL)
			{
				execute_command(args);
			}
			else
			{
				fprintf(stderr, "./shell: No such file or directory\n");
			}

			printf("$ ");
			free_args(args);
		}
		free(line);
	}
	return (0);
}
