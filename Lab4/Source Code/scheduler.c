#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <assert.h>

#include <sys/wait.h>
#include <sys/types.h>

#include "proc-common.h"
#include "request.h"

/* Compile-time parameters. */
#define SCHED_TQ_SEC 2                /* time quantum */
#define TASK_NAME_SZ 60               /* maximum size for a task's name */

struct node_t
{
	pid_t pid;
	int serial_id;
	struct node_t *next;
};
typedef struct node_t node;

struct list_t
{
	node *head;
	node *tail;
};
typedef struct list_t list;

list lista;
int status;
pid_t current_pid;

list createList(void)
{
	list result;
	result.head = NULL;
	result.tail = NULL;
	return result;
}

void add (list *l, pid_t x, int n)
{
	node *temp;
	temp=(node *)malloc(sizeof(node));
	temp->pid = x;
	temp->serial_id = n;

	if (l->head == NULL)
	{
		temp->next = temp;
		l->head = temp;
		l->tail = temp;
	}
	else
	{
		l->tail->next = temp;
		l->tail = temp;
		l->tail->next = l->head;
	}
}	

pid_t get_head (list *l)
{
	pid_t result;
	if (l->head == NULL)
		return -1;
	result = l->head->pid;
	return result;
}

void move_head (list *l)
{
	l->head = l->head->next;
	l->tail = l->tail->next;
}

void remove_head (list *l)
{
	node *temp, *prev;
	temp = l->head;
	prev = l->tail;

	if (temp == prev)
	{
		l->head = NULL;
		l->tail = NULL;
	}
	else
	{
		l->head = temp->next;
		prev->next = l->head;
	}
	free(temp);
}

/*
 * SIGALRM handler
 */
static void
sigalrm_handler(int signum)
{
	//assert(0 && "Please fill me!");
	printf("%d is stopping\n", current_pid);
	kill(current_pid, SIGSTOP);
}

/* 
 * SIGCHLD handler
 */
static void
sigchld_handler(int signum)
{
	//assert(0 && "Please fill me!");
	pid_t p;
	for (;;)
	{
		p = waitpid(-1, &status, WUNTRACED | WNOHANG);
		if (p < 0)
		{
			perror("waitpid");
			exit(1);
		}
		if (p == 0)
			break;
		explain_wait_status(p, status);
		if (WIFEXITED(status) || WIFSIGNALED(status))
		{
			/*A child has died*/
			alarm(0);
			remove_head(&lista);
			current_pid = get_head(&lista);
			if (current_pid < 0)
			{
				printf("All processes have been executed\n");
				exit(1);
			}
			printf("%d is starting\n", current_pid);
			kill(current_pid, SIGCONT);
			alarm(SCHED_TQ_SEC);
		}
		/* A child has died*/
		if (WIFSTOPPED(status))
		{
			move_head(&lista);
			current_pid = get_head(&lista);
			printf("%d is starting\n",current_pid);
			kill(current_pid, SIGCONT);
			alarm(SCHED_TQ_SEC);
		}
	}
}



/* Install two signal handlers.
 * One for SIGCHLD, one for SIGALRM.
 * Make sure both signals are masked when one of them is running.
 */
static void
install_signal_handlers(void)
{
	sigset_t sigset;
	struct sigaction sa;

	sa.sa_handler = sigchld_handler;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGCHLD);
	sigaddset(&sigset, SIGALRM);
	sa.sa_mask = sigset;
	if (sigaction(SIGCHLD, &sa, NULL) < 0) {
		perror("sigaction: sigchld");
		exit(1);
	}

	sa.sa_handler = sigalrm_handler;
	if (sigaction(SIGALRM, &sa, NULL) < 0) {
		perror("sigaction: sigalrm");
		exit(1);
	}

	/*
	 * Ignore SIGPIPE, so that write()s to pipes
	 * with no reader do not result in us being killed,
	 * and write() returns EPIPE instead.
	 */
	if (signal(SIGPIPE, SIG_IGN) < 0) {
		perror("signal: sigpipe");
		exit(1);
	}
}

int main(int argc, char *argv[])
{
	lista = createList();
	int nproc;
	int i;	
	pid_t p;
	nproc = argc - 1;
	for (i=1; i<=nproc; i++)
	{
		p = fork();
		add(&lista, p , i);
		if (p == 0)
		{
			char *newargv[] = {argv[i], NULL, NULL, NULL};
			char *newenviron[] = {NULL};
			printf("I am %s, PID = %ld\n", argv[0], (long)getpid());
			printf("About to replace myself with the executable %s...\n", argv[i]);
			sleep(2);
			raise(SIGSTOP);
			execve(argv[i], newargv, newenviron);
			/*execve() only returns an error*/
			perror("execve");
			exit(1);
		}
	}
	/*
	 * For each of argv[1] to argv[argc - 1],
	 * create a new child process, add it to the process list.
	 */

	//nproc = 0; /* number of proccesses goes here */

	/* Wait for all children to raise SIGSTOP before exec()ing. */
	wait_for_ready_children(nproc);

	/* Install SIGALRM and SIGCHLD handlers. */
	install_signal_handlers();

	if (nproc == 0) {
		fprintf(stderr, "Scheduler: No tasks. Exiting...\n");
		exit(1);
	}
	current_pid = get_head(&lista);
	printf("%d\n", current_pid);
	kill(current_pid, SIGCONT);
	alarm(SCHED_TQ_SEC);


	/* loop forever  until we exit from inside a signal handler. */
	while (pause())
		;

	/* Unreachable */
	fprintf(stderr, "Internal error: Reached unreachable point\n");
	return 1;
}

