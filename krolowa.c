#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "my_header.h"

int TK;
int N = 2;
int P = 10;
int number_of_bees_in_hive = 2;

int main(int argc, char *argv[])
{
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
