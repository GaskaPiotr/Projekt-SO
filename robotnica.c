#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#include "my_header.h"

int TI;
int XI;
int how_many_times_in_hive = 1;


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
	printf("Number of visists to die = %d\n", XI);

	time_t seconds = time(NULL);

	while (XI > how_many_times_in_hive)
	{
		while (time(NULL) <= seconds + TI)
		{
			sleep(0.5);
		}
		printf("I'm leaving the hive\n");
		printf("I'm coming back to the hive\n");
		seconds = time(NULL);
		how_many_times_in_hive++;
	}
	printf("Worker died\n");
	return 0;
}





