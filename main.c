/*
 * Simple Prime number calculator
 *
 *  Created on: Oct 29, 2018
 *      Author: yoram
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "../LIST/listc.h"

#define MAXPRIME 0x7fffffffffffffff

static long long int count = 0;
void printPrime(long long int *prime)
{
	printf("[%I64d] %I64d\n", ++count, *prime);
}

int main (int argc, char *argv[])
{
	long long int i, j, k, max, iter = 0;
	int isPrime;
	clock_t start, end;
	LIST list;
	char print;

	// Get the upper limit for the Prime calculation.
	// In this example we do not check for errors except that max_num < MAXPRIME
	if (argc > 1)
	{
		sscanf(argv[1], "%I64d", &max);
		max = (max > MAXPRIME? MAXPRIME : max);
		if (argc > 2)
			sscanf(argv[2], "%c", &print);
	}
	else
	{
		printf("usage: prime1 max_num [p]\n");
		return -1;
	}

	list = createList(1000, sizeof(long long int));

	start = clock();

	for (i = 2; i <= max; i++)
	{
		isPrime = 1;

		// It is enough to check up to the square root of each number.
		// Beyond this value, all factors that constract the number start to repeat
		// For instamce the number 12 is constract of 1 x 12, 2 x 6, 3 x 4, 4 x 3, 6 x 2 and 12 x 1.
		// Square root of 12 is 3.46.. -> 4. Therefor it is enought to check up to 4
		// We will try to devide current number by prime numbers smaller than its value.
		// This prime numbers are stored on the list we build
		k = (long long int)sqrtl(i)+1;
		resetList(list);
		j = 2;
		while (getNextElement(list, &j) && (j < k))
		{
			iter++;
			if (i % j == 0)
			{
				isPrime = 0;
				break;
			}
		}
		if (isPrime)
			insertElement(list, &i);
	}

	end = clock();

	// print primes
	if (print == 'p')
	{
		long long int ind;
		ind = listElements(list, (void (*)(void *))&printPrime);
		printf("Total primes: %I64d", ind);
	}

	printf("clock elapsed: %ld, Iterations: %I64d\n",(end - start), iter);

	deleteList(&list);

	return 0;
}
