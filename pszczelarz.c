#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>



int *shared_space;
int max_space = 100;

void handler(int sig)
{
	if (sig == SIGUSR1)
	{
		max_space += 10;
        	printf("Received SIGUSR1\n");
		printf("Beekeeper added hive frame, new space = %d\n", max_space);
	}
	else if (sig == SIGUSR2)
	{
		max_space -= 10;
        	printf("Received SIGUSR2\n");
		printf("Beekeeper removed hive frame, new space = %d\n", max_space);
    	}
	*shared_space = max_space;
}


int main()
{
	if (signal(SIGUSR1, handler) == SIG_ERR)
	{
		perror("Error: Unable to catch SIGUSR1\n");
		return EXIT_FAILURE;
	}
        if (signal(SIGUSR2, handler) == SIG_ERR)
	{
                perror("Error: Unable to catch SIGUSR2\n");
                return EXIT_FAILURE;
        }


	key_t key = ftok("hive", 65);
	int shmid = shmget(key, sizeof(int), 0666 | IPC_CREAT);
	shared_space = (int *)shmat(shmid, NULL, 0);
	*shared_space = max_space;

	printf("I am beekeeper\n");

	printf("Program running with PID: %d\n", getpid());
	printf("Send signals using the kill command: \n");
	printf("kill -USR1 %d (for SIGUSR1)\n", getpid());
	printf("kill -USR2 %d (for SIGUSR2)\n", getpid());

	while(1)
	{
		//printf("Beekeeper: Changed the space of beehive\n");
		//printf("Beekeeper sent current beehive space = %d to the Quen\n", max_space);
		pause();
	}
	shmdt(shared_space);
	shmctl(shmid, IPC_RMID, NULL);
	return EXIT_SUCCESS;
}
