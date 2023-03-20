#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "tree.h"
#include "proc-common.h"

void create_proc(struct tree_node *root){
	int status;
	int i,j;
	pid_t pid[root->nr_children];
	change_pname(root->name);
	printf("I am %s, with PID = %ld and Parent with PID = %ld\n",root->name,(long)getpid(),(long)getppid());

	for(i=0; i< root->nr_children; i++){
		pid[i] = fork();
		if(pid[i] == 0){
			pid[i] = getpid();
			if((root->children + i)->nr_children==0){
				change_pname((root->children+i)->name);
				printf("I am %s, with PID = %ld and Parent with PID = %ld\n",(root->children+i)->name,(long)getpid(),(long)getppid());
				printf("%s: Sleeping...\n", (root->children+i)->name);
				raise(SIGSTOP);
				printf("PID = %ld, name = %s is awake\n", (long)getpid(), (root->children+i)->name);
				exit(3);
			}
			else{
				create_proc(root->children+i);
			}
		}
	}
	printf("%s is waiting for his children to sleep\n", root->name);
	wait_for_ready_children(root->nr_children);
	raise(SIGSTOP);
	printf("PID = %ld, name = %s is awake\n", (long)getpid(), root->name);

	for(j=0; j < root->nr_children; j++){
		kill(pid[j],SIGCONT);
		pid[i]=wait(&status);
		explain_wait_status(pid[i],status);
	}
	exit(2);
}

int main(int argc,char *argv[])
{
	struct tree_node *root;
	int status;
	pid_t pid;

	if(argc < 2){
		fprintf(stderr,"Usage: %s <input_tree_file>\n\n",argv[0]);
		exit(1);
	}

	/*read tree*/
	root = get_tree_from_file(argv[1]);

	/*fork root of process tree*/
	pid = fork();
	if (pid < 0){
		perror("main: fork");
		exit(1);
	}
	if (pid == 0){
		create_proc(root);
		exit(1);
	}

	wait_for_ready_children(1);

	/*print the process tree root at pid*/
	show_pstree(pid);

	kill(pid, SIGCONT);

	/*wait for the root of the process tree to terminate*/
	wait(&status);
	explain_wait_status(pid,status);
	return 0;
}

