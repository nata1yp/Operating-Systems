#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "tree.h"
#include "proc-common.h"

#define SLEEP_PROC_SEC 10
#define SLEEP_TREE_SEC 3

void create_proc(struct tree_node *root){
	int status;
	int i,j;
	pid_t pid;
	change_pname(root->name);
	printf("I am %s, with PID = %ld and Parent with PID = %ld\n",root->name,(long)getpid(),(long)getppid());

	for(i=0; i<root->nr_children; i++){
		pid = fork();
		if(pid == 0){
			if((root->children+i)->nr_children==0){
				change_pname((root->children+i)->name);
				printf("I am %s, with PID = %ld and Parent with PID = %ld\n",(root->children+i)->name,(long)getpid(),(long)getppid());
				sleep(SLEEP_PROC_SEC);
			exit(3);
			}
			else{
			create_proc(root->children+i);
			}
		}
	}

	for(j=0; j<root->nr_children; j++){
		pid=wait(&status);
		explain_wait_status(pid,status);
	}
	exit(2);
}

int main(int argc,char **argv)
{
	struct tree_node *root;
	int status;
	pid_t pid;

	if(argc!=2){
		fprintf(stderr,"Usage: %s <input_tree_file>\n\n",argv[0]);
		exit(1);
	}

	root = get_tree_from_file(argv[1]);
	print_tree(root);
	/*fork root of process tree*/
	fprintf(stderr, "Parent, PID = %ld: Creating child...\n",(long)getpid());

	pid = fork();
	if (pid < 0){
		/*fork failed*/
		perror("main: fork");
		exit(1);
	}
	if (pid == 0){
		/*child*/
		create_proc(root);
		exit(1);
	}

	sleep(SLEEP_TREE_SEC);

	/*print the process tree root at pid*/
	show_pstree(pid);

	/*wait for the root of the process tree to terminate*/
	wait(&status);
	explain_wait_status(pid,status);
	return 0;
}


