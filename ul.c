#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

// Project ID used in keys
#define PROJ_ID 80

// File for SMH with N - Max num of bees
#define NUM_OF_BEES_FILE "/tmp/bees"

// A = HIVE
// B = OUTSIDE
#define FIFO_A1 "/tmp/fifo_a1"
#define FIFO_B1 "/tmp/fifo_b1"
#define FIFO_A2 "/tmp/fifo_a2"
#define FIFO_B2 "/tmp/fifo_b2"
#define SEM_FILE "/tmp/sem"

// 1 = First tunnel
// 2 = Second tunnel
#define SEM_A1_QUEUE 0
#define SEM_B1_QUEUE 1
#define SEM_A2_QUEUE 2
#define SEM_B2_QUEUE 3
#define SEM_1 4
#define SEM_2 5

int sem_id;
int shm_id;

void semaphore_operation(int semID, int sem_num, int op);
key_t generate_key(pid_t pid);
void create_or_open_file(const char *file_name);
void create_fifo(const char *file_name);
key_t create_key(const char *file_name);

void queue_function_hive(int semID, pid_t workerPID, char side, int tunnel);
void exit_handler(int sig);

int main()
{
	// Setting up exit handler
	if (signal(SIGINT, exit_handler) == SIG_ERR)
	{
		perror("Error: Unable to catch SIGINT");
		exit(EXIT_FAILURE);
	}

	printf("Hive: Starting process\n");
	printf("Hive: Create FIFOS\n");
	// Create FIFO
	create_fifo(FIFO_A1);
	// Create FIFO
	create_fifo(FIFO_B1);
	// Create FIFO
	create_fifo(FIFO_A2);
	// Create FIFO
	create_fifo(FIFO_B2);

	printf("Hive: Create SEM\n");

	// Creating or open SEM FILE
	create_or_open_file(SEM_FILE);

	// Creating key for SEM ID
	key_t sem_key = create_key(SEM_FILE);
	// Creating SEM ID
	sem_id = semget(sem_key, 6, 0666 | IPC_CREAT);
	if (sem_id == -1)
	{
		perror("Error: Semget failed");
		exit(EXIT_FAILURE);
	}

	printf("Hive: Create SHM\n");

	// Creating or open SHM FILE
	create_or_open_file(NUM_OF_BEES_FILE);

	// Creating key for SHM ID
	key_t shm_key = create_key(NUM_OF_BEES_FILE);
	// Creating SHM ID
	shm_id = shmget(shm_key, sizeof(int), 0666 | IPC_CREAT);

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

	// Opening FIFO
        int fifo_a1 = open(FIFO_A1, O_RDONLY|O_NONBLOCK);
        if (fifo_a1 == -1)
        {
                perror("Error: Opening FIFO failed");
                exit(EXIT_FAILURE);
        }
	// Opening FIFO
        int fifo_b1 = open(FIFO_B1, O_RDONLY|O_NONBLOCK);
        if (fifo_b1 == -1)
        {
                perror("Error: Opening FIFO failed");
                exit(EXIT_FAILURE);
        }
	// Opening FIFO
        int fifo_a2 = open(FIFO_A2, O_RDONLY|O_NONBLOCK);
        if (fifo_a2 == -1)
        {
                perror("Error: Opening FIFO failed");
                exit(EXIT_FAILURE);
        }
	// Opening FIFO
        int fifo_b2 = open(FIFO_B2, O_RDONLY|O_NONBLOCK);
        if (fifo_b2 == -1)
        {
                perror("Error: Opening FIFO failed");
                exit(EXIT_FAILURE);
        }

	printf("Hive: Initialize SEM\n");
	// Initialize semaphore
	// Starting queue empty
	if (semctl(sem_id, SEM_A1_QUEUE, SETVAL, 0) == -1)
	{
		perror("Error: Initialize semaphore failed");
		exit(EXIT_FAILURE);
	}
	// Starting queue empty
        if (semctl(sem_id, SEM_B1_QUEUE, SETVAL, 0) == -1)
        {
                perror("Error: Initialize semaphore failed");
                exit(EXIT_FAILURE);
        }
	// Starting queue empty
        if (semctl(sem_id, SEM_A2_QUEUE, SETVAL, 0) == -1)
        {
                perror("Error: Initialize semaphore failed");
                exit(EXIT_FAILURE);
        }
	// Starting queue empty
        if (semctl(sem_id, SEM_B2_QUEUE, SETVAL, 0) == -1)
        {
                perror("Error: Initialize semaphore failed");
                exit(EXIT_FAILURE);
        }
	// Starting semaphore closed
        if (semctl(sem_id, SEM_1, SETVAL, 0) == -1)
        {
                perror("Error: Initialize semaphore failed");
                exit(EXIT_FAILURE);
        }
	// Starting semaphore closed
        if (semctl(sem_id, SEM_2, SETVAL, 0) == -1)
        {
                perror("Error: Initialize semaphore failed");
                exit(EXIT_FAILURE);
        }

	printf("Hive: Started hive loop\n\n\n");
	// Hive loop
	while (1)
	{
		// Count of workers in FIFO
		int queue_a1_count = semctl(sem_id, SEM_A1_QUEUE, GETVAL);
		if (queue_a1_count == -1)
		{
			perror("Error: Checking semaphore count failed");
	                exit(EXIT_FAILURE);
		}
		// Count of workers in FIFO
		int queue_b1_count = semctl(sem_id, SEM_B1_QUEUE, GETVAL);
                if (queue_b1_count == -1)
                {
                        perror("Error: Checking semaphore count failed");
	                exit(EXIT_FAILURE);
                }
		// Count of workers in FIFO
		int queue_a2_count = semctl(sem_id, SEM_A2_QUEUE, GETVAL);
                if (queue_a2_count == -1)
                {
                        perror("Error: Checking semaphore count failed");
	                exit(EXIT_FAILURE);
                }
		// Count of workers in FIFO
		int queue_b2_count = semctl(sem_id, SEM_B2_QUEUE, GETVAL);
                if (queue_b2_count == -1)
                {
                        perror("Error: Checking semaphore count failed");
	                exit(EXIT_FAILURE);
                }
		// State of semaphore
		int queue_1 = semctl(sem_id, SEM_1, GETVAL);
                if (queue_1 == -1)
                {
                        perror("Error: Checking value of semaphore failed");
	                exit(EXIT_FAILURE);
                }
		// State of semaphore
		int queue_2 = semctl(sem_id, SEM_2, GETVAL);
                if (queue_2 == -1)
                {
                        perror("Error: Checking value of semaphore failed");
	                exit(EXIT_FAILURE);
                }

		printf("Hive: Checking count of workers in queues A1: %d;B1: %d\n", queue_a1_count, queue_b1_count);
		printf("Hive: Checking count of workers in queues A2: %d;B2: %d\n", queue_a2_count, queue_b2_count);
		printf("Hive: State of FIRST QUEUE: %d; SECOND QUEUE: %d\n\n\n", queue_1, queue_2);

		pid_t worker_pid;
		ssize_t number_of_bytes;
		// Choosing queue
		if (queue_a1_count > 0 && queue_1 == 0)
		{
			// Reading worker PID
			number_of_bytes = read(fifo_a1, &worker_pid, sizeof(worker_pid));
			if (number_of_bytes > 0)
				// Successful worker PID letting through tunnel
				queue_function_hive(sem_id, worker_pid, 'A', 1);
		    	else if (number_of_bytes == 0)
			{
				perror("Error: Queue A1 not empty, but no workers");
				exit(EXIT_FAILURE);
			}
			else if (number_of_bytes == -1)
			{
				perror("Error: Reading worker PID");
				exit(EXIT_FAILURE);
			}
        	}
		else if (queue_b1_count > 0 && queue_a1_count == 0 && queue_1 == 0)
		{
			// Reading worker PID
                        number_of_bytes = read(fifo_b1, &worker_pid, sizeof(worker_pid));
                        if (number_of_bytes > 0)
                                // Successful worker PID letting through tunnel
                                queue_function_hive(sem_id, worker_pid, 'B', 1);
                        else if (number_of_bytes == 0)
			{
				perror("Error: Queue B1 not empty, but no workers");
				exit(EXIT_FAILURE);
                        }
			else if (number_of_bytes == -1)
                        {
                                perror("Error: Reading worker PID");
                                exit(EXIT_FAILURE);
                        }

        	}

                if (queue_a2_count > 0 && queue_2 == 0)
                {
                        // Reading worker PID
                        number_of_bytes = read(fifo_a2, &worker_pid, sizeof(worker_pid));
                        if (number_of_bytes > 0)
                                // Successful worker PID letting through tunnel
                                queue_function_hive(sem_id, worker_pid, 'A', 2);
                        else if (number_of_bytes == 0)
			{
                                perror("Hive: Queue A2 not empty, but no workers");
				exit(EXIT_FAILURE);
                        }
			else if (number_of_bytes == -1)
                        {
                                perror("Error: Reading client PID");
                                exit(EXIT_FAILURE);
                        }
                }
                else if (queue_b2_count > 0 && queue_a2_count == 0 && queue_2 == 0)
                {
                        // Reading worker PID
                        number_of_bytes = read(fifo_b2, &worker_pid, sizeof(worker_pid));
                        if (number_of_bytes > 0)
                                // Successful client PID letting through tunnel
                                queue_function_hive(sem_id, worker_pid, 'B', 2);
                        else if (number_of_bytes == 0)
			{
                                perror("Error: Queue B2 not empty, but no workers");
				exit(EXIT_FAILURE);
			}
			else if (number_of_bytes == -1)
                        {
                                perror("Error: Reading worker PID");
                                exit(EXIT_FAILURE);
                        }

                }

		// Hive waits for not immediately queues
        	sleep(3);
	}

	return 0;
}



void semaphore_operation(int semID, int sem_num, int op) {
	struct sembuf sb;
	sb.sem_num = sem_num;
	sb.sem_op = op;
	sb.sem_flg = 0;
	if (semop(semID, &sb, 1) == -1)
	{
		perror("Error: Semop failed");
		exit(EXIT_FAILURE);
	}
}

// Generate key based on PID
key_t generate_key(pid_t pid) {
	char worker_tmp_file[30];
	// Create a unique filename based on PID
    	sprintf(worker_tmp_file, "/tmp/%d", pid);

	// Create or open the temporary file
	FILE *fp = fopen(worker_tmp_file, "w");
    	if (fp == NULL)
	{
		perror("Error: Failed to create temporary file");
	        exit(EXIT_FAILURE);
	}
	fclose(fp);

	// Use ftok to generate a key
	key_t key = ftok(worker_tmp_file, PROJ_ID);
    	if (key == -1)
	{
        	perror("Error: Ftok failed");
        	exit(EXIT_FAILURE);
    	}
    	return key;
}


void create_or_open_file(const char *file_name)
{
        // Creating or open FILE
        FILE *fp = fopen(file_name, "w");
        if (fp == NULL)
        {
                perror("Error: Failed to create SEM file");
                exit(EXIT_FAILURE);
        }
        if (fclose(fp) == EOF)
        {
                perror("Error: Failed closing SEM file");
                exit(EXIT_FAILURE);
        }

}

void create_fifo(const char *file_name)
{
	if (mkfifo(file_name, IPC_CREAT | 0666) == -1)
	{
		perror("Error: Unable to create FIFO");
		exit(EXIT_FAILURE);
	}
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



// Passing worker through tunnel
void queue_function_hive(int semID, pid_t workerPID, char side, int tunnel)
{
	printf("Hive: Worker with PID %d wants to pass a tunnel.\n", workerPID);
	key_t worker_key = generate_key(workerPID);
	int worker_sem_id = semget(worker_key, 1, IPC_CREAT|0666);
	if (worker_sem_id == -1)
	{
		perror("Error: Semget failed");
		exit(EXIT_FAILURE);
	}
	// Opening worker SEM
	semaphore_operation(worker_sem_id, 0, 1);
	printf("Hive: Worker with ID: %d passing through a tunnel\n", worker_sem_id);

	if (tunnel == 1)
	{
		if (side == 'A')
			// Removing one from queue
			semaphore_operation(semID, SEM_A1_QUEUE, -1);
		else
			// Removing one from queue
			semaphore_operation(semID, SEM_B1_QUEUE, -1);
		// Blocking SEM
		semaphore_operation(semID, SEM_1, 1);
	}
	else
	{
		if (side == 'A')
			// Removing one from queue
			semaphore_operation(semID, SEM_A2_QUEUE, -1);
		else
			// Removing one from queue
			semaphore_operation(semID, SEM_B2_QUEUE, -1);
		// Blocking SEM
		semaphore_operation(semID, SEM_2, 1);
	}
}


void exit_handler(int sig)
{
        printf("\nHive: Received SIGINT\n");
        printf("Hive: Cleaning\n");
        printf("Hive: Removing FIFOS\n");
        if (unlink(FIFO_A1) == -1)
        {
                perror("Error: Unlinking FIFO failed");
                exit(EXIT_FAILURE);
        }
        if (unlink(FIFO_B1) == -1)
        {
                perror("Error: Unlinking FIFO failed");
                exit(EXIT_FAILURE);
        }
        if (unlink(FIFO_A2) == -1)
        {
                perror("Error: Unlinking FIFO failed");
                exit(EXIT_FAILURE);
        }
        if (unlink(FIFO_B2) == -1)
        {
                perror("Error: Unlinking FIFO failed");
                exit(EXIT_FAILURE);
        }

        printf("Hive: Removing SEM\n");
        if (unlink(SEM_FILE) == -1)
        {
                perror("Error: Unlinking SEM FILE failed");
                exit(EXIT_FAILURE);
        }
        if (semctl(sem_id, 0, IPC_RMID, 0) == -1)
        {
                perror("Error: Removing SEM failed");
                exit(EXIT_FAILURE);
        }

        printf("Hive: Removing SHM\n");
        if(unlink(NUM_OF_BEES_FILE) == -1)
 	{
                perror("Error: Unlinking num of bees FILE failed");
                exit(EXIT_FAILURE);
        }
	if (shmctl(shm_id, IPC_RMID, 0))
	{
		perror("Error: Removing SHM failed");
		exit(EXIT_FAILURE);
	}

        exit(0);
}

