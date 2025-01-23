#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "my_header.h"

// Project ID used in keys
#define PROJ_ID 80

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

// File for creating private SEM
char temp_file[30];

// Time to overheat or chill
int TI;

// Number of visits of Hive to die
int XI;
int how_many_times_in_hive = 0;


void semaphore_operation(int sem_id, int sem_num, int op);
void join_queue(const char *fifo);
key_t generate_key(pid_t pid);
void queue_function(int semID, int priv_semID, int count, int tunnel, char *side);


int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("./program Ti(number_of_sec_to_overheat) Xi(number_of_visits_to_die)\n");
		exit(EXIT_FAILURE);
	}
	check_if_positive_int(argv[1], &TI);
	printf("Time to overheat = %d\n", TI);
        check_if_positive_int(argv[2], &XI);
	printf("Number of visists Hive to die = %d\n", XI);

        // Creating SEM ID
        key_t key = ftok(SEM_FILE, PROJ_ID);
        int sem_id = semget(key, 6, IPC_CREAT | 0666);
        if (sem_id == -1)
        {
                perror("Error: Semget failed\n");
                exit(EXIT_FAILURE);
        }

        // Creating private SEM
        key_t key_sem = generate_key(getpid());
        int priv_sem_id = semget(key_sem, 1, IPC_CREAT | 0666);
        if (priv_sem_id == -1)
        {
                perror("Error: Semget failed\n");
                exit(EXIT_FAILURE);
        }
        // Setting up private SEM
        semctl(priv_sem_id, 0, SETVAL, 0);

        // Worker starting in Hive
        char side = 'A';

        // TODO: early stage of worker


	// While before dying
	while (XI > how_many_times_in_hive)
	{
		// Wait until overheat or chill
		sleep(TI);

		// Worker in Hive
                if (side == 'A')
                {
                        printf("Worker: Trying to join queue on Hive side\n");
                        // Number of workers in A1 queue
                        int queue_a1_count = semctl(sem_id, SEM_A1_QUEUE, GETVAL);
                        // Number of workers in A2 queue
                        int queue_a2_count = semctl(sem_id, SEM_A2_QUEUE, GETVAL);

			// Choosing queue
                        if (queue_a1_count <= queue_a2_count)
                                queue_function(sem_id, priv_sem_id, queue_a1_count, 1, &side);
                        else
                                queue_function(sem_id, priv_sem_id, queue_a2_count, 2, &side);

                }
                // Worker Outside
                else if (side == 'B')
                {
                        printf("Worker: Trying to join queue on Outside side\n");
                        // Number of workers in B1 queue
                        int queue_b1_count = semctl(sem_id, SEM_B1_QUEUE, GETVAL);
                        // Number of workers in B2 queue
                        int queue_b2_count = semctl(sem_id, SEM_B2_QUEUE, GETVAL);

			// Choosing queue
                        if (queue_b1_count <= queue_b2_count)
                                queue_function(sem_id, priv_sem_id, queue_b1_count, 1, &side);
                        else
                                queue_function(sem_id, priv_sem_id, queue_b2_count, 2, &side);

			// Number of visits increase
			how_many_times_in_hive++;
                }
	}

	printf("Worker died\n");
	return 0;
}

void semaphore_operation(int sem_id, int sem_num, int op)
{
        struct sembuf sb;
        sb.sem_num = sem_num;
        sb.sem_op = op;
        sb.sem_flg = 0;
        if (semop(sem_id, &sb, 1) == -1)
        {
                perror("Error: Semop failed\n");
                exit(EXIT_FAILURE);
        }
}

void join_queue(const char *fifo)
{
        // Opening FIFO
        int fifo_fd = open(fifo, O_WRONLY);
        if (fifo_fd == -1)
        {
                perror("Error: Failed to open FIFO\n");
                exit(EXIT_FAILURE);
        }

        // Saving Worker PID in queue
        pid_t pid = getpid();
        if (write(fifo_fd, &pid, sizeof(pid)) == -1)
        {
                perror("Error: Failed to write to FIFO");
                exit(EXIT_FAILURE);
        }

        close(fifo_fd);
}

key_t generate_key(pid_t pid)
{
        // Create a unique filename based on PID
        sprintf(temp_file, "/tmp/%d", pid);

        // Create the temporary file
        FILE *fp = fopen(temp_file, "w");
        if (fp == NULL)
        {
                perror("Error: Failed to create temporary file\n");
                exit(EXIT_FAILURE);
        }
        fclose(fp);

        // Using ftok to generate a key
        key_t key = ftok(temp_file, PROJ_ID);
        if (key == -1)
        {
                perror("Error: Ftok failed\n");
                exit(EXIT_FAILURE);
        }
        return key;
}

void queue_function(int semID, int priv_semID, int count, int tunnel, char *side)
{
        printf("Worker: Joining %c%d queue\n", *side, tunnel);
        if (*side == 'A' && tunnel == 1)
        {
                // Joining queue
                join_queue(FIFO_A1);
                semaphore_operation(semID, SEM_A1_QUEUE, 1);
        }
        else if (*side == 'B' && tunnel == 1)
        {
                // Joining queue
                join_queue(FIFO_B1);
                semaphore_operation(semID, SEM_B1_QUEUE, 1);
        }
        else if (*side == 'A' && tunnel == 2)
        {
                // Joining queue
                join_queue(FIFO_A2);
                semaphore_operation(semID, SEM_A2_QUEUE, 1);
        }
        else
        {
                // Joining queue
                join_queue(FIFO_B2);
                semaphore_operation(semID, SEM_B2_QUEUE, 1);
        }

	// Waiting for open semaphore
        semaphore_operation(priv_semID, 0, -1);

        if (*side == 'A')
                // Changing side
                *side = 'B';
        else
                // Changing side
                *side = 'A';

        if (tunnel == 1)
                // Unlocking tunnel SEM
                semaphore_operation(semID, SEM_1, -1);
        else
                // Unlocking tunnel SEM
                semaphore_operation(semID, SEM_2, -1);

}




