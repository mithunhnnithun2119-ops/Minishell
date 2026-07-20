/*
    MITHUN H N
    DATE : 19/07/2026

    Description : The Mini Shell is a Linux command-line interpreter developed using the C programming language. It executes user commands by creating child processes using the fork() system call. The shell supports built-in commands such as cd, exit, jobs, fg, and bg. It implements signal handling (SIGINT, SIGTSTP, and SIGCHLD) and job control using linked lists to manage background and stopped processes. This project demonstrates Linux system programming concepts, including process management, signals, system calls, and job control.
*/
#include<unistd.h>
#include<stdio.h>
#include "shell.h"

char input_prompt[25];
char prompt[]="minishell$";
int main()
{

	scan_input(prompt,input_prompt);
}



/*****
// Check whether input_str contains "PS1=" or not.
// If it does,
//     -> Check the input.




09/07/2026

SIGNAL Handling

-> Using signal() is enough; sigaction() is not required.

* Ctrl + C should not terminate the shell.
    signal(SIGINT, own_handler);

    -> When Ctrl + C is pressed, the shell should not terminate.
    -> It should go to my signal_handler().

    In signal_handler():
        if (signum == SIGINT)
        {
            printf("%s", prompt);
        }

* Ctrl + Z should not terminate the shell.

Note:
-> In the actual terminal, the prompt is not printed.
-> But in the mini shell, print the prompt.

    -> When Ctrl + Z is pressed, it should go to my signal_handler().

    In signal_handler():
        else if (signum == SIGTSTP)
        {
            printf("%s", prompt);
        }

* Extra cases (Ctrl + C and Ctrl + Z)

Example:
    sleep 30 + Ctrl + C
    -> Process terminates and prints the prompt.

    sleep 30 + Ctrl + Z
    -> Stops the execution and prints the prompt.

* Default behavior should be set only in the child.

    else if (child == 0)
    {
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
    }

    -> After control returns to while(1), the prompt is printed.

    // Problem:
    // The prompt is printed by both the parent and the child.
    // One prompt is printed from the signal handler,
    // another from while(1).

** How to avoid printing the prompt twice?

// Without using a flag

-> Create a global pid variable.

if (signum == SIGINT)
{
    if (pid == 0)
    {
        printf("%s", prompt);
    }
}
else if (signum == SIGTSTP)
{
    if (pid == 0)
    {
        printf("%s", prompt);
    }
}

// After that, reset the pid value.
// Think about where the pid should be reset.



09/07/2026

**** Parent is pending

(This is the last part of the project.)

// When the child changes state, it generates SIGCHLD.

When Ctrl + Z is pressed for a particular process,

-> The child process is stopped.
-> The parent does not know that the child has stopped.
-> The parent still thinks the child is running.

-> To avoid this, use waitpid().

Need to pass the WUNTRACED macro.

    waitpid(pid, &status, WUNTRACED);



**** 13/07/2026 ****

(FG, BG, JOB)

-> Last part of the project.

(Write in command.c)

Why are these commands used?

BG:
-> Continues the last stopped process in the background.
-> Other commands can still be executed.

Example:
    Ctrl + Z
    bg

JOB:
-> Displays all stopped processes.

FG:
-> Continues the last stopped process in the foreground.
-> Other commands cannot be executed until it finishes.

Example:
    sleep 30
    Ctrl + Z
    fg

-> The stopped process automatically continues and completes.



// **************** Implementation ****************

-> First, store the last stopped process.
-> Use a linked list.
-> Whenever Ctrl + Z is pressed, store the process in the linked list.



*************** JOB ***************

struct input
{
    int pid;
    char command[15];
    struct input *link;
};

Note:
-> Create the link pointer first.

Write in signal_handler():

else
{
    insert_first();   // Store all stopped processes.
}



******** Function definition (command.c) ********

// Check whether input_str contains "jobs" or not.

// Write the logic to print the linked list.



*************** FG ***************

// Check whether input_str contains "fg" or not.

kill(head->pid, SIGCONT);

waitpid(head->pid, &status, WUNTRACED);
// Wait until the foreground process terminates.

delete_first();



*************** BG ***************

// Check whether input_str contains "bg" or not.

kill(head->pid, SIGCONT);

delete_first();

// When the particular process finishes,
// clear the process entry.

// How to know when the process is finished?
// Use SIGCHLD.
// This signal is sent to the parent when the child terminates.

// Handle it in own_handler().

signal();

if (signum == SIGCHLD)
{
    // Clear the resources.
}





13/07/2026

First, complete the environment variables.

Implement:
    echo $$
    echo $?
    echo $SHELL

Write the code in command.c.

-> echo $$ : Print the current process PID.

-> echo $? :
   Check whether the previous command executed successfully.
   If successful, return 0.
   Otherwise, return a non-zero value.

-> echo $SHELL :
   Print the shell path.



*** Code Implementation ***

// echo $$

-> Use getpid() to get the current process ID.



// echo $?

-> Use the macro WEXITSTATUS().
-> It returns 0 or a non-zero exit status.



// echo $SHELL

char *getenv();

-> Pass "SHELL" to getenv().
-> Use the returned value in printf().

    printf("%s\n", getenv("SHELL"));



void signal_handler(int signum)
{
    if (signum == SIGINT)
    {
        if (pid == 0)
        {
            printf("%s\n", prompt);
        }
    }
    else if (signum == SIGTSTP)
    {
        if (pid == 0)
        {
            printf("%s\n", prompt);
        }
        else
        {
            // Store the stopped process in the linked list.
            insert_first( stopped process );
        }
    }
    else if (signum == SIGCHLD)
    {
        // Clear the resources.
    }
}
*///
