#include "my_header.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>

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
