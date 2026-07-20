#include<stdio.h>
#include<string.h>
#include"shell.h"
pid_t ret;
jobs *head = NULL;

void delete_first(void)
{
    if (head == NULL)
        return;

    jobs *temp = head;

    head = head->link;

    free(temp);
}

void insert_first(int pid, char *command)
{
    jobs *new = malloc(sizeof(jobs));

    if (new == NULL)
        return;

    new->pid = pid;
    strcpy(new->command, command);

    new->link = head;
    head = new;
}

void signal_handler(int signum)
{
	if(signum == SIGINT)
	{
		if(ret == 0)
		{
			char buf[100];
			getcwd(buf,sizeof(buf));
			printf("\n%s%s",prompt,buf);
			fflush(stdout);
		}
		
	}

	else if(signum == SIGTSTP)
	{
		if(ret == 0)
		{
			char buf[100];
			getcwd(buf,sizeof(buf));
			printf("\n%s%s",prompt,buf);
			fflush(stdout);
		}

		else
		{
    		insert_first(ret, input_prompt);
		}

		
		
	}

	else if (signum == SIGCHLD)
	{
    	waitpid(ret, &status, WNOHANG);
	}

}



void scan_input(char *prompt, char *input_string)
{
	extract_external_commands(external_commands);

	signal(SIGINT,signal_handler); //to modify signal handler for ctrl + c
	signal(SIGTSTP,signal_handler); //to modify signal handler for ctrl + z

	while(1)
	{
		char buffer[70];
        getcwd(buffer,70);
		printf("%s %s ", buffer, prompt);

		scanf(" %[^\n]",input_string);

		if(strncmp(input_string, "PS1=" ,4) == 0)
		{
			if(input_string[4] != ' ')
			{
				strcpy(prompt,input_string+4);
			}
			continue;
		}



		char *command = get_command(input_string);
		int command_type =  check_command_type(command);



		execute_internal_commands(input_string);
		if(command_type == BUILTIN)
		{
			execute_internal_commands(input_string);
		}

		//external commands 
		else if(command_type == EXTERNAL)
		{
			 ret = fork();

			if(ret > 0)
			{
                waitpid(ret, &status, WUNTRACED);

			if (WIFEXITED(status))
			{
    			last_exit_status = WEXITSTATUS(status);
			}
			else
			{
    			last_exit_status = 1;
			}

				ret = 0;

			}

			else if(ret == 0)
			{
				signal(SIGINT,SIG_DFL);
				signal(SIGTSTP,SIG_DFL);
				execute_external_commands(input_string);
			}

			else
			{
				perror("fork");
				return;
			}
		}

		//no valid command 
		else if(command_type == NO_COMMAND)
		{
			printf("Invalid command : Enter Valid Command\n");
			last_exit_status = 1;
		}
	}
}

