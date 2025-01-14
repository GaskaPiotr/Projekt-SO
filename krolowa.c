#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "my_header.h"

int TK;
int N = 2;
int P = 10;
int number_of_bees_in_hive = 2;

void clean_workers(int sig)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

void spawn_worker(const char *worker_program, const char *worker_arg1, const char *worker_arg2)
{
    	pid_t pid = fork();

    	if (pid == -1)
	{
        	perror("fork");
       	 	exit(EXIT_FAILURE);
    	}
	else if (pid == 0)
	{
        	// Worker process
        	execlp(worker_program, worker_program, worker_arg1, worker_arg2, NULL);
       	 	perror("execlp");
        	exit(EXIT_FAILURE);
    	}
    	printf("Spawned worker with PID %d.\n", pid);
}


int main(int argc, char *argv[])
{
	const char *worker_program = "./robotnica";

	signal(SIGCHLD, clean_workers);

	if (argc != 2)
	{
		printf("./program Tk(number_of_sec_to_lay_eggs)\n");
		exit(EXIT_FAILURE);
	}

	check_if_positive_int(argv[1], &TK);

	printf("I am Queen\n");

	time_t seconds = time(NULL);
	int counter = 100;
	printf("Start producing workers\n");


	while (counter > 0)
	{
		if (P > number_of_bees_in_hive)
		{
			while (time(NULL) <= seconds + TK)
			{
				sleep(0.5);
			}
			printf("laying eggs\n");
			spawn_worker(worker_program, "10", "2");
			//execlp("./robotnica", "./robotnica", "10", "2", NULL);
			//perror("execlp error");
			//if (system("./robotnica 10 2") != 0)
			//	printf("ERROR executing system()\n");
			seconds = time(NULL);
			counter--;
			number_of_bees_in_hive++;
		}
	}
	return 0;
}
