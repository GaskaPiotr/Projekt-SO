#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#include "my_header.h"

// Project ID used in keys
#define PROJ_ID 80

// TODO UL deciding about N and P
#define NUM_OF_BEES_FILE "/tmp/bees"
// Time to lay eggs
int TK;

// Maximum number of workers
int N = 100;

// Maximum number of workers in Hive
int P;
int number_of_bees = 0;
int number_of_bees_in_hive = 0;


void clean_workers(int sig);
void lay_egg(const char *worker_program, const char *worker_arg1, const char *worker_arg2);
void calculate_P(int n, int *p);
key_t generate_key(const char *file_name);
void exit_handler(int sig)
{
	printf("\nQueen: Received SIGINT\n");
	printf("Queen: Cleaning\n");
	// TODO
	//printf("Queen: Removing 
	exit(0);
}


int main(int argc, char *argv[])
{
        if (argc != 2)
        {
                printf("./program Tk(number_of_sec_to_lay_eggs)\n");
                exit(EXIT_FAILURE);
        }

	// Setting up exit handler
	if (signal(SIGINT, exit_handler) == SIG_ERR)
	{
		perror("Error: Unable to catch SIGINT");
		exit(EXIT_FAILURE);
	}

	printf("\n\nQueen: EXIT WITH SIGINT\n\n");

	// Setting up signal to clean zombie Workers
	if (signal(SIGCHLD, clean_workers) == SIG_ERR)
	{
		perror("Error: Unable to catch SIGCHLD");
		exit(EXIT_FAILURE);
	}
	const char *worker_program = "./robotnica";

	// Creating key for SHM
	key_t key = generate_key(NUM_OF_BEES_FILE);
	if (key == -1)
	{
		perror("Error: Ftok failed");
		exit(EXIT_FAILURE);
	}
	// Creating SHM with size of one int
	int shm_id = shmget(key, sizeof(int), 0666 | IPC_CREAT);
	if (shm_id == -1)
	{
		perror("Error: Shmget failed");
		exit(EXIT_FAILURE);
	}

	// Shared N
	int *shared_int = (int *)shmat(shm_id, NULL, 0);
	if (shared_int == (int *)-1)
	{
		perror("Error: Shmat failed");
		exit(EXIT_FAILURE);
	}

	check_if_positive_int(argv[1], &TK);

	printf("Queen: Started producing workers\\nn");

	while (1)
	{
		// Getting N from SHM
		if (*shared_int != 0)
			N = *shared_int;
		printf("Queen: space in beehive = %d\n", N);
		// Calculating space in Hive
		calculate_P(N, &P);
		if (P > number_of_bees_in_hive && N > number_of_bees)
		{
			// Waits Tk time to lay eggs
			sleep(TK);
			printf("laying egg\n");
			lay_egg(worker_program, "10", "2");


			//execlp("./robotnica", "./robotnica", "10", "2", NULL);
			//perror("execlp error");
			//if (system("./robotnica 10 2") != 0)
			//	printf("ERROR executing system()\n");
			number_of_bees_in_hive++;
			number_of_bees++;
		}
	}
	shmdt(shared_int);
	return 0;
}

void clean_workers(int sig)
{
        while(waitpid(-1, NULL, WNOHANG) > 0)
	{
		number_of_bees--;
	}
}

void lay_egg(const char *worker_program, const char *worker_arg1, const char *worker_arg2)
{
        pid_t pid = fork();

        // Fork failed
        if (pid == -1)
        {
                perror("Error: Fork failure");
                exit(EXIT_FAILURE);
        }

        // Worker process
        else if (pid == 0)
        {
                // Child become worker process
                // worker_arg1: Ti - Time to oveheat or chill
                // worker_arg2: Xi - Number of visits the Hive to die
                execlp(worker_program, worker_program, worker_arg1, worker_arg2, NULL);
                // If execlp fails
                perror("Error: Execlp failure");
                exit(EXIT_FAILURE);
        }

        // Queen process
        printf("Queen: Spawned worker with PID %d.\n", pid);
}


void calculate_P(int n, int *p)
{
	// if N is odd p < n/2
	*p = n/2;
	// if N is even p = n/2
	if (n % 2 == 0)
		*p -= 1;
}

key_t generate_key(const char *file_name)
{
        // Create or open the temporary file
        FILE *fp = fopen(file_name, "w");
        if (fp == NULL)
        {
                perror("Error: Failed to create temporary file\n");
                exit(EXIT_FAILURE);
        }
        fclose(fp);

        // Using ftok to generate a key
        key_t key = ftok(file_name, PROJ_ID);
        if (key == -1)
        {
                perror("Error: Ftok failed\n");
                exit(EXIT_FAILURE);
        }
        return key;
}
