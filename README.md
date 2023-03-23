# Operating Systems - NTUA

# Lab Exercises

### [Lab 1](https://github.com/natalypeyioti/Operating-Systems/tree/master/Lab1)
* This lab was focused on:
  * Introduction to Linux env and get to know how the file system works.
  * Creating object file and [Linking object files](https://github.com/natalypeyioti/Operating-Systems/tree/master/Lab1/Source%20Code%201.1)
  * [Merging two files into a third](https://github.com/natalypeyioti/Operating-Systems/tree/master/Lab1/Source%20Code%201.2) using read and write system calls.

### [Lab 2](https://github.com/natalypeyioti/Operating-Systems/tree/master/Lab2)
* This lab was focused on:
  * Fork system call. Creating both a [given](https://github.com/natalypeyioti/Operating-Systems/blob/master/Lab2/Source%20Code/ask2-fork.c  "Solution for creating given proccess tree") and [arbitrary](https://github.com/natalypeyioti/Operating-Systems/blob/master/Lab2/Source%20Code/prog1.2.c  "Solution for creating arbitrary proccess tree") proccess tree.
  * Sending/Handling signals so as the messages of the arbitrary proccess tree are [shown in depth](https://github.com/natalypeyioti/Operating-Systems/blob/master/Lab2/Source%20Code/prog1.3.c "Depth - First printing messages of arbitrary tree").
  * [Parallel calculation of numerical expression](https://github.com/natalypeyioti/Operating-Systems/blob/master/Lab2/Source%20Code/prog1.4.c "Solution for parallel calc of numerical expr") given in a form of an arbitrary proccess tree. Every node is either + or * and every leaf is a number. Pipes was used for the implementation.

### [Lab 3](https://github.com/natalypeyioti/Operating-Systems/tree/master/Lab3)
* This lab was focused on:
  * [Sync](https://github.com/natalypeyioti/Operating-Systems/blob/master/Lab3/Source%20Code/simplesync.c "Sync on given code") - locks vs atomic operations.
  * [Parallel calculation of the Mandelbrot set](https://github.com/natalypeyioti/Operating-Systems/blob/master/Lab3/Source%20Code/mandel_threads.c "Solution using threads"). See the difference between using more threads each time.
  
### [Lab 4](https://github.com/natalypeyioti/Operating-Systems/tree/master/Lab4)
* This lab was focused on:
  * Creating a round-robin scheduler. The scheduler is a parent proccess in the user space, distributing computational time in child processes. In order to control the processes, the scheduler will use the SIGSTOP and SIGCONT signals to stop and activate each process, respectively. Every proccess is being executed for a period of time equal to tq. If the proccess is terminated before the time equal to tq, the scheduler removes the proccess from the queue and activates the next one. If the time quantum is been reached and the proccess won't finish, the proccess terminates, its been placed to the end of the queue and the next proccess is been activated. The operation of the scheduler is required to be asynchronous based on signals. The scheduler will be using the SIGALRM (when the time quantum is reached the scheduler stops the current proccess) and SIGCHLD (pause or terminate the proccess) signals. Every proccess has its own id. The scheduler shows relevant messages at activation/pause/termination of a proccess. When all the proccesses are finished, the scheduler also terminates.
  Execution of the scheduler: ```./scheduler prog prog prog prog```
  * Operation check of the scheduler through shell. Extending the previous code we need to support the control of the operations of the scheduler through the shell. The user must be able to request dynamic creation and termination of proccesses while interacting with the shell. Shell will be receiving commands from the user, issue requests of the right form and send them to the scheduler. Receive replies (success/fail) of the outcome of their executions and informs the user.
  Commands of the shell:
    * 'p' : prints a list with the running proccesses. Id,PID,Name are shown.
    * 'k' : Gets the ID of a proccess (not the PID) and asks from the scheduler to terminate that proccess.
    * 'e' : Gets the Name of a running proccess and asks from the scheduler to create a proccess with that name.
    * 'q' : Shell terminates its operation.
  * Implement priorities in the scheduler. Extending the previous code we need also to support priorities in the running proccesses queue. There must be two priorities, High and Low. If there are any High priority proccesses in the queue, only those are been executed using round-robin. If there are olny Low priority proccesses in the queue, they are been executed using round-robin. In order to change the priority of any proccess the following command must be given:
    * Make a low priority proccess high priority: ``` 'h' id_of_the_proccess ```
    * Make a high priority proccess low priority: ``` 'l' id_of_the_proccess ```

  ###### Parts of code were given from the lecturers teaching this course.
