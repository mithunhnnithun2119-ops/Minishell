#include<stdio.h>
#include"shell.h"
int status;
int last_exit_status = 0; 
char *external_commands[153]; //external commands

//List o/*builtin commands/
	char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
						"set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
						"exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", "jobs","fg","bg",NULL};



                        
void extract_external_commands(char **external_commands)
{
    int fd,i = 0, j = 0;;
    char ch;
    char buffer[50];

    fd = open("extern_cmd.txt", O_RDONLY);

    if (fd == -1)
    {
        perror("open");
        return;
    }

    while (read(fd, &ch, 1) > 0)
    {
        if (ch != '\n')
        {
            buffer[j++] = ch;
        }
        else
        {
            buffer[j] = '\0';

            external_commands[i] = malloc(strlen(buffer) + 1);
            strcpy(external_commands[i], buffer);

            i++;
            j = 0;
        }
    }

 
    if (j > 0)
    {
        buffer[j] = '\0';
        external_commands[i] = malloc(strlen(buffer) + 1);
        strcpy(external_commands[i], buffer);
        i++;
    }

    external_commands[i] = NULL;

    close(fd);
     
}
char first_word[10];
char *get_command(char *input_str)
{
    int i;
    
    for( i=0;input_str[i] != ' ' && input_str[i] != '\0';i++)
    {
        first_word[i] = input_str[i];
    }
    first_word[i] = '\0';

    return first_word;
    
}

int check_command_type(char *command)
{
    int i=0;
    while(builtins[i])
    {
        if(strcmp(builtins[i],command) == 0)
        {
            return BUILTIN;
        }
        i++;
    }

    i = 0;
    while (external_commands[i] != NULL)
    {
        if (strcmp(external_commands[i], command) == 0)
        {
            return EXTERNAL;
        }
        i++;
    }

    return NO_COMMAND;
}
    


void execute_internal_commands(char *input_string)
{   


    
    if(strncmp(input_string,"cd ",3) == 0)
    {
        if (chdir(input_string + 3) == 0)
        {
            last_exit_status = 0;
        }
        else
        {
            perror("cd");
            last_exit_status = 1;
        }
    }
    
     else if(strcmp(input_string,"pwd") == 0)
    {
        char buffer[50];
        getcwd(buffer, 50);
        printf("%s\n", buffer);

        last_exit_status = 0;
    }
   

     else if(strcmp(input_string,"exit") == 0)
    {
       exit(0);
    }
    

    else if(strcmp(input_string,"echo $$") == 0)
    {
        printf("%d\n",getpid());
        last_exit_status = 0;
    }

    
    
    else if(strcmp(input_string,"echo $?") == 0)
    {
       //
      printf("%d\n", last_exit_status);
    }
 
     else if(strcmp(input_string,"echo $SHELL") == 0)
    {
       //
       printf("%s\n",getenv("SHELL"));
       last_exit_status = 0;
       
    }
    
    
    else if (strcmp(input_string, "jobs") == 0)
    {
        jobs *temp = head;

        if (temp == NULL)
        {
            printf("No jobs\n");
        }
        else
        {
            while (temp != NULL)
            {
                printf("[%d] Stopped\t%s\n", temp->pid, temp->command);
                temp = temp->link;
            }
        }
    }
    

    else if(strcmp(input_string,"fg") == 0)
    {
        if(head == NULL)
        {
            printf("No jobs\n");
            return;
        }
        kill(head->pid,SIGCONT);
        waitpid(head->pid,&status,WUNTRACED);

        if (WIFEXITED(status) || WIFSIGNALED(status))
        {
            delete_first();
        }
    }
     

    else if(strcmp(input_string,"bg") == 0)
    {

        if(head == NULL)
        {
            printf("No jobs\n");
            return;
        }
        kill(head->pid,SIGCONT);
       delete_first();
       signal(SIGCHLD,signal_handler);
    }
   

    
}


void execute_external_commands(char *input_string)
{
    char *argv[20];
    int argc = 0;

    /* Convert input string to argv[] */
    argv[argc] = strtok(input_string, " ");

    while (argv[argc] != NULL)
    {
        argc++;
        argv[argc] = strtok(NULL, " ");
    }

    /* Check whether pipe is present */
    int pipe_present = 0;
    int save_stdin = dup(STDIN_FILENO);
    int save_stdout = dup(STDOUT_FILENO);
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "|") == 0)
        {
            pipe_present = 1;
            break;
        }
    }

    /* No pipe */
    if (pipe_present == 0)
    {
        execvp(argv[0], argv);
        perror("execvp");
        exit(1);
    }

    /* Pipe present - N pipe logic */
    int cmd_pos[20];
    cmd_pos[0] = 0;

    int k = 1;
    int count = 1;

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "|") == 0)
        {
            argv[i] = NULL;

            if (i + 1 < argc)
            {
                cmd_pos[k++] = i + 1;
                count++;
            }
            else
            {
                printf("Invalid pipe command\n");
                exit(1);
            }
        }
    }

    for (int i = 1; i <= count; i++)
    {
        int fd[2];

        if (i < count)
            pipe(fd);

        pid_t ret = fork();

        if (ret > 0)
        {

            if (i < count)
            {
                close(fd[1]);
                dup2(fd[0], STDIN_FILENO);
                close(fd[0]);
            }
        }
        else if (ret == 0)
        {
            if (i < count)
            {
                close(fd[0]);
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
            }

            execvp(argv[cmd_pos[i - 1]], &argv[cmd_pos[i - 1]]);
            perror("execvp");
            exit(1);
        }
        else
        {
            perror("fork");
            exit(1);
        }
    }
    for(int i = 0; i < count; i++)
    wait(NULL);

    dup2(save_stdin, STDIN_FILENO);
    dup2(save_stdout, STDOUT_FILENO);

    close(save_stdin);
    close(save_stdout);
}



