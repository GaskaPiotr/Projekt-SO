#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// Project ID used in keys
#define PROJ_ID 80

// File for SHM with N - Max num of bees
#define NUM_OF_BEES_FILE "/tmp/bees"


int *shared_n;
int N;
int MAX_N;
int MIN_N;
int space;

key_t create_key(const char *file_name);

void sig_handler(int sig);
void exit_handler(int sig)
{
	printf("\nBeekeper: Received SIGINT\n");
	printf("Beekeper: Exiting\n");
	exit(0);
}
int main()
{
	// Setting up exit handler
	if (signal(SIGINT, exit_handler) == SIG_ERR)
	{
		perror("Error: Unable to catch SIGINT");
		exit(EXIT_FAILURE);
	}

	printf("\n\nBeekeper: EXIT WITH SIGINT\n\n");

	// Creating SHM
	key_t shm_key = create_key(NUM_OF_BEES_FILE);
	int shm_id = shmget(shm_key, sizeof(int), 0666 | IPC_CREAT);
        if (shm_id == -1)
        {
                perror("Error: Shmget failed");
                exit(EXIT_FAILURE);
        }

	// Setting up shared int
	shared_n = (int *)shmat(shm_id, NULL, 0);
        if (shared_n == (int *)-1)
        {
                perror("Error: Shmat failed");
                exit(EXIT_FAILURE);
        }

	// Getting the N
	N =  *shared_n;
	// Setting space to N
	space = *shared_n;
	// Setting MAX_N and MIN_N
	MAX_N = 2 * N;
	MIN_N = N / 2;
	printf("N: %d, MAX_N: %d, MIN_N: %d\n", N, MAX_N, MIN_N);

        // Setting up signals for SIGUSR1
        if (signal(SIGUSR1, sig_handler) == SIG_ERR)
        {
                perror("Error: Unable to catch SIGUSR1");
                exit(EXIT_FAILURE);
        }
        // Setting up signals for SIGUSR2
        if (signal(SIGUSR2, sig_handler) == SIG_ERR)
        {
                perror("Error: Unable to catch SIGUSR2");
                exit(EXIT_FAILURE);
        }

	printf("Beekeper: Running with PID : %d\n", getpid());
	printf("Beekeper: Send signals using the kill command: \n\n");
	printf("kill -USR1 %d (for SIGUSR1)\n", getpid());
	printf("kill -USR2 %d (for SIGUSR2)\n\n", getpid());

	while(1)
	{
		pause();
	}
	return 0;
}


key_t create_key(const char *file_name)
{
        key_t k = ftok(file_name, PROJ_ID);
        if (k == -1)
        {
                perror("Error: Ftok failed");
                exit(EXIT_FAILURE);
        }
        return k;
}

void sig_handler(int sig)
{
        if (sig == SIGUSR1)
        {
                printf("Beekeper: Received SIGUSR1\n");

                if (space < MAX_N)
                {
                        space = MAX_N;
                        printf("Beekeeper: Added hive frame new space: %d\n", space);
                }
        }
        else if (sig == SIGUSR2)
        {
                printf("Beekeper: Received SIGUSR2\n");

                if (space > MIN_N)
                {
                        space = MIN_N;
                        printf("Beekeeper: Removed hive frame new space: %d\n", space);
                }
        }
        *shared_n = space;
}
