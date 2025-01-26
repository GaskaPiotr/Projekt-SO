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

// SHM file to share number of bees in hive
#define NUM_OF_BEES_IN_HIVE_FILE "/tmp/bees_in_hive"

// Time to lay eggs
int TK;
// Maximum number of workers
int N;
// Maximum number of workers in Hive
int P;
// Number of all workers
int num_of_bees = 0;


void clean_workers(int sig);
void lay_egg(const char *worker_program, const char *worker_arg1, const char *worker_arg2);
void calculate_P(int n, int *p);
key_t generate_key(const char *file_name);
void exit_handler(int sig)
{
	printf("\nQueen: Received SIGINT\n");
	printf("Queen: Waiting for all workers to die naturally\n");
	printf("Queen: num_of_bees: %d\n", num_of_bees);
	while(num_of_bees > 0)
	{
		printf("Queen: Waiting, num of workers: %d\n", num_of_bees);
		sleep(1);
	}
	printf("Queen: ALL WORKERS DIED\n");
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
	if (signal(SIGUSR1, exit_handler) == SIG_ERR)
	{
		perror("Error: Unable to catch SIGUSR1");
		exit(EXIT_FAILURE);
	}

	printf("\n\nQueen: EXIT WITH: kill -USR1 %d\n\n", getpid());

	// Setting up signal to clean zombie Workers
	if (signal(SIGCHLD, clean_workers) == SIG_ERR)
	{
		perror("Error: Unable to catch SIGCHLD");
		exit(EXIT_FAILURE);
	}
	const char *worker_program = "./robotnica";

	// Creating key for SHM N
	key_t key_n = generate_key(NUM_OF_BEES_FILE);
	if (key_n == -1)
	{
		perror("Error: Ftok failed");
		exit(EXIT_FAILURE);
	}
	// Creating SHM N with size of one int
	int shm_id_n = shmget(key_n, sizeof(int), 0666 | IPC_CREAT);
	if (shm_id_n == -1)
	{
		perror("Error: Shmget failed");
		exit(EXIT_FAILURE);
	}
	// Shared N
	int *shared_n = (int *)shmat(shm_id_n, NULL, 0);
	if (shared_n == (int *)-1)
	{
		perror("Error: Shmat failed");
		exit(EXIT_FAILURE);
	}

        // Creating key for SHM num of bees in hive
        key_t key_num_of_bees = generate_key(NUM_OF_BEES_IN_HIVE_FILE);
        if (key_num_of_bees == -1)
        {
                perror("Error: Ftok failed");
                exit(EXIT_FAILURE);
        }
        // Getting SHM num of bees with size of one int
        int shm_id_num_of_bees = shmget(key_num_of_bees, sizeof(int), 0666 | IPC_CREAT);
        if (shm_id_num_of_bees == -1)
        {
                perror("Error: Shmget failed");
                exit(EXIT_FAILURE);
        }
        // Shared num of bees
        int *shared_num_of_bees = (int *)shmat(shm_id_num_of_bees, NULL, 0);
        if (shared_num_of_bees == (int *)-1)
        {
                perror("Error: Shmat failed");
                exit(EXIT_FAILURE);
        }


	check_if_positive_int(argv[1], &TK);

	printf("Queen: Started producing workers\\nn");

	sleep(TK);
	while (1)
	{
		// Getting N from SHM
		N = *shared_n;
		printf("Queen: space in Hive = %d\n", N);
		// Calculating space in Hive
		calculate_P(N, &P);
		printf("P: %d, num_of_bees_in_hive: %d\n", P, *shared_num_of_bees);
		printf("N: %d, num_of_bees: %d\n", N, num_of_bees);
		if (P > *shared_num_of_bees && N > num_of_bees)
		{
			printf("laying egg\n");
        	        // worker_arg1: Ti - Time to oveheat or chill
	                // worker_arg2: Xi - Number of visits the Hive to die
			lay_egg(worker_program, "5", "2");
			// Increase number of bees in hive
			*shared_num_of_bees++;
			// Increase number of bees
			num_of_bees++;
		}
		// Waits Tk time to lay eggs
		sleep(TK);
	}
	return 0;
}

void clean_workers(int sig)
{
        while(waitpid(-1, NULL, WNOHANG) > 0)
	{
		num_of_bees--;
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
