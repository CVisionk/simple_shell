include "main.h"

#define MAX_INPUT_LENGTH 1024
#define MAX_ARGS 64

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
		args[arg_index++] = strdup(token);
		token = strtok(NULL, " ");
		}
	args[arg_index] = NULL;
	return (args);
}

void free_args(char **args)
{
	for (int i = 0; args[i] != NULL; i++)
	{
		free(args[i]);
 }
 free(args);
}

void execute_command(char** args)
{
 pid_t pid = fork();
 if (pid < 0)
	{
  fprintf(stderr, "Fork failed\n");
  exit(EXIT_FAILURE);
 } else if (pid == 0)
	{
 	/* Child process */
 	if (execve(args[0], args, NULL) == -1)
		{
  	fprintf(stderr, "./shell: No such file or directory\n");
  	exit(EXIT_FAILURE);
  }

 } else
	{
 	 /* Parent process */
   wait(NULL); /* Wait for child to finish */
 }
}


int main()
{
	size_t len = 0;
 ssize_t read;
	char *line = NULL;
	char ** args;

	printf("#cisfun$ ");
	while ((read = getline(&line, &len, stdin))!= -1)
	{
		if (read == -1 && len == 0)
		{
   fprintf(stderr, "Memory allocation failed\n");
   exit(EXIT_FAILURE);
  }
  line[strcspn(line, "\n")] = 0;
    
		args = splitter(line);
	/*for (int i = 0; args[i] != NULL; i++) 
	{
        printf("%s\n", args[i]);
		
    }*/
	if(args[1] == NULL)
	{
		execute_command(args);

	}else
		fprintf(stderr, "./shell: No such file or directory\n");
    printf("#cisfun$ ");
	free_args(args);
    }
	return (0);
}