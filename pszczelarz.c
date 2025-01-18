#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>


void handler(int sig)
{
	if (sig == SIGUSR1)
	{
        	printf("Received SIGUSR1\n");
	}
	else if (sig == SIGUSR2)
	{
        	printf("Received SIGUSR2\n");
    	}
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

	printf("I am beekeeper\n");

	printf("Program running with PID: %d\n", getpid());
	printf("Send signals using the kill command: \n");
	printf("kill -USR1 %d (for SIGUSR1)\n", getpid());
	printf("kill -USR2 %d (for SIGUSR2)\n", getpid());

	while(1)
	{
		pause();
	}
	return EXIT_SUCCESS;
}
