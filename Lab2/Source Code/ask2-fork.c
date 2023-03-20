#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "proc-common.h"

#define SLEEP_PROC_SEC  10
#define SLEEP_TREE_SEC  3

/*
 * Create this process tree:
 * A-+-B---D
 *   `-C
 */
void fork_procs(void)
{
	int status;
	pid_t pid;
	/*
	 * initial process is A.
	 */

	change_pname("A");
	//printf("A: Sleeping...\n");
	//sleep(SLEEP_PROC_SEC);

	pid = fork();
	if (pid < 0) 
	{
		perror("fork");
		exit(1);
	}
	if (pid == 0) 
	{
		printf("A: PID = %ld: Creating child C...\n", (long)getppid());
		change_pname("C");
		printf("C: PID = %ld: \n", (long)getpid());
		printf("A: Waiting for child C to terminate...\n");
		printf("C: Sleeping...\n");
		sleep(SLEEP_PROC_SEC);
		printf("C: Exiting...\n");
		exit(17);
	}
	else
	{
		pid = fork();
		if (pid < 0) 
		{	
			perror("fork");
			exit(1);
		}
		if (pid == 0)
		{
			printf("A: PID = %ld: Creating child B...\n", (long)getppid());
			change_pname("B");
			printf("B: PID = %ld: \n", (long)getpid());
			printf("A: Waiting for child B to terminate...\n");
			pid = fork();

			if (pid < 0) 
			{
				perror("fork");
				exit(1);
			}
			if (pid == 0)
			{
				printf("B: PID = %ld: Creating child D...\n", (long)getppid());
				change_pname("D");
				printf("D: PID = %ld: \n", (long)getpid());
				printf("B: Waiting for child D to terminate...\n");
				printf("D: Sleeping...\n");
				sleep(SLEEP_PROC_SEC);
				printf("D: Exiting...\n");
				exit(13);
			}
			else 
			{
				pid = wait(&status);
				explain_wait_status(pid,status);
				printf("B: Exiting...\n");
				exit(19);
			}
		}
		else 
		{	
			sleep(SLEEP_PROC_SEC);
			pid = wait(&status);
			explain_wait_status(pid,status);
			pid = wait(&status);
			explain_wait_status(pid,status);
		}
	}
	/* ... */

	printf("A: Exiting...\n");
	exit(16);
}

int main(void)
{
	pid_t pid;
	int status;

	/* Fork root of process tree */
	pid = fork();
	if (pid < 0) {
		/*fork failed*/
		perror("main: fork");
		exit(1);
	}
	if (pid == 0) {
		/* Child */
		fork_procs();
		exit(1);
	}

	/*
	 * Father
	 */
	sleep(SLEEP_TREE_SEC);

	/* Print the process tree root at pid */
	show_pstree(pid);

	/* Wait for the root of the process tree to terminate */
	pid = wait(&status);
	explain_wait_status(pid, status);

	return 0;
}
