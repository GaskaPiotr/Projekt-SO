#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

int TI;
int XI;
int how_many_times_in_hive = 1;

void check_if_positive_int(char *arg, int *a)
{
	char *p;
	errno = 0;
        long conv = strtol(arg, &p, 10);

        if (errno != 0 || *p != '\0' || conv > INT_MAX || conv < 1)
        {
                printf("Parameter isn't a postive int\n");
                exit(EXIT_FAILURE);
        }
        else
                *a = conv;

}

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
		printf("Wychodze z ula\n");
		printf("Wracam do ula\n");
		seconds = time(NULL);
		how_many_times_in_hive++;
	}
	printf("Pszczola umarla\n");
	return 0;
}





