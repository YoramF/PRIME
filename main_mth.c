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
#include <pthread.h>
#include "../LIST/listc.h"

#define MAXPRIME	0x7fffffffffffffff
#define MAXTHREADS	100
#define MINLIST		1000

typedef struct _thtd
{
	int					exit_s;			// thread exit status
	long long int		prime_s;		// lower range for prime calculation for thread
	long long int		prime_l;		// upper range for prime calculation for thread
	LIST				list;			// list of generated primes for this thread
	pthread_t			thrd_id;		// thread ID
} thrd;

static thrd thrd_arr [MAXTHREADS];

static long long int count = 0;
void printPrime(long long int *prime)
{
	printf("[%I64d] %I64d\n", ++count, *prime);
}

void primeCalc (thrd *param)
{
	long long int i, j, k;
	int isPrime;

	if ((param->list = createList(MINLIST, sizeof(long long int))) == NULL)
	{
		param->exit_s = -1;
		pthread_exit(param);
	}

	for (i = param->prime_s; i <= param->prime_l; i++)
	{
		isPrime = 1;

		// It is enough to check up to the square root of each number.
		// Beyond this value, all factors that constract the number start to repeat
		// For instamce the number 12 is constract of 1 x 12, 2 x 6, 3 x 4, 4 x 3, 6 x 2 and 12 x 1.
		// Square root of 12 is 3.46.. -> 4. Therefor it is enought to check up to 4
		k = (long long int)sqrtl(i)+1;
		for (j = 2; j < k; j++)
			if (i % j == 0)
			{
				isPrime = 0;
				break;
			}
		if (isPrime)
			insertElement(param->list, &i);
	}

	param->exit_s = 1;
	pthread_exit(param);
}


int main (int argc, char *argv[])
{
	int maxthrd, thrd_c = 0, i;
	long long int maxprime, primeperth, primec = 2;
	clock_t start, end;
	char prn = 'n';

	int rc;
	thrd *thrd_p;

	// Get the upper limit for the Prime calculation.
	// In this example we do not check for errors except that max_num < MAXPRIME
	if (argc > 2)
	{
		sscanf(argv[1], "%I64d", &maxprime);
		maxprime = (maxprime > MAXPRIME? MAXPRIME : maxprime);

		sscanf(argv[2], "%d", &maxthrd);
		maxthrd = (maxthrd > MAXTHREADS? MAXTHREADS : maxthrd);
		maxthrd = (maxthrd > maxprime? maxprime : maxthrd);

		if (argc > 3)
			sscanf(argv[3], "%c", &prn);
	}
	else
	{
		printf("usage: prime1 maxPrimes maxThreads p\n");
		return -1;
	}

	start = clock();

	// Lounch threads
	primeperth = (maxprime / maxthrd) + 1;
	for (i = 0; i < maxthrd; i++)
	{
		if (primec > maxprime)
			break;

		thrd_arr[i].prime_s = primec;
		primec += primeperth;
		primec = (primec > maxprime? maxprime : primec);
		thrd_arr[i].prime_l = primec;
		primec++;

		if ((rc = pthread_create(&thrd_arr[i].thrd_id, NULL, (void *(*)(void *))&primeCalc, &thrd_arr[i])))
		{
		      fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
		      return -1;
		}
		thrd_c++;
	}

	// Wait for thread completion
	for (i = 0; i < thrd_c; i++)
	{
	    pthread_join(thrd_arr[i].thrd_id, (void **)(&thrd_p));
	    printf("thread (%d), exit status: %d\n", i, thrd_p->exit_s);
	}


	end = clock();

	// print primes
	// ind = listElements(list, &printPrime);
	// printf("Total primes: %d\n", ind);

	for (i = 0; i < thrd_c; i++)
	{
		if (prn == 'p')
		{
			rc = listElements(thrd_arr[i].list, (void (*)(void *))&printPrime);
			printf("Total primes for thread [%d] %d\n", i, rc);
		}

		deleteList(&thrd_arr[i].list);
	}

	printf("clock elapsed: %ld\n",(end - start));
	return 0;
}
