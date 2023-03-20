#include "zing.h"
#include <stdio.h>
#include <unistd.h>

void zing(void)
{
	printf("You are the best, %s \n",  getlogin());
}


