#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "tree.h"
#include "proc-common.h"

void create_proc(struct tree_node *root,int fd){
	int status;
	int i,j;
	int val;
	pid_t pid;
	int pfd[2];
	int read_pfd[2];
	int num1,num2,result;

	change_pname(root->name);
	printf("I am %s, with PID = %ld and Parent with PID = %ld\n",root->name,(long)getpid(),(long)getppid());

	for(i=0; i<root->nr_children; i++){
		printf("%s: Creating pipe...\n",root->name);
		if(pipe(pfd) < 0){
			perror("pipe");
			exit(1);
		}
		read_pfd[i] = pfd[0];
		pid = fork();
		if(pid == 0){
			if((root->children + i)->nr_children == 0){
				change_pname((root->children + i)->name);
				printf("I am %s, with PID = %ld and Parent with PID = %ld\n",(root->children + i)->name, (long)getpid(),(long)getppid());
				val = atoi((root->children +i)->name);
				if(write(pfd[1],&val,sizeof(val)) != sizeof(val)){
					perror("Child: write to pipe");
					exit(1);
				}
				exit(3);
			}
			else{
				create_proc(root->children + i,pfd[1]);	
			}
		}
	}
	printf("%s: My PID is %ld. Receiving int values from the children.\n",root->name, (long)getpid());
	if(read(read_pfd[0], &num1, sizeof(num1)) != sizeof(num1)){
		perror("Parent: read from pipe");
		exit(1);
	}
	printf("%s: received value %d from the pipe.\n",root->name,num1);	

	if(read(read_pfd[1], &num2, sizeof(num2)) != sizeof(num2)){
		perror("Parent: read from pipe");
		exit(1);
	}
	printf("%s: received value %d from the pipe. Will now compute!\n",root->name,num2);
	int symbol = *root->name;
	if((symbol) == '+'){
		result = num1 + num2;
	}
	else{
		result = num1 * num2;
	}
	printf("%s: Writing to pipe\n",root->name);
	if(write(fd,&result,sizeof(result)) != sizeof(result)){
		perror("child: write to pipe");
		exit(1);	
	}

	for(j=0; j<root->nr_children; j++){
		pid=wait(&status);
		explain_wait_status(pid,status);
	}
	exit(2);
}

int main(int argc,char *argv[])
{
	struct tree_node *root;
	int status;
	int val;
	int pfd[2];
	pid_t pid;

	printf("Parent: Creating pipe...\n");
	if(pipe(pfd) < 0){
		perror("pipe");
		exit(1);
	}
	if(argc != 2){	
		fprintf(stderr,"Usage: %s <input_tree_file>\n\n",argv[0]);
		exit(1);	
	}
	
	root = get_tree_from_file(argv[1]);
	print_tree(root);

	fprintf(stderr, "Parent, PID = %ld: Creating child...\n",(long)getpid());

	pid = fork();
	if (pid < 0){
		/*fork failed*/
		perror("main: fork");
		exit(1);
	}
	if (pid == 0){	
		/*child*/
		create_proc(root,pfd[1]);	
		exit(1);
	}
	
	printf("Parent: My PID is %ld, Receiving a value from the child.\n",(long)getpid());
	if(read(pfd[0],&val,sizeof(val)) != sizeof(val)){
		perror("parent: read from pipe");
		exit(1);
	}
	printf("Parent: received value %d from the pipe. Will now compute.\n",val);
	printf("The final result is:%d\n",val);
	/*wait for the root of the process tree to terminate*/
	wait(&status);
	explain_wait_status(pid,status);
	return 0;
}

