/*
 * Sample program which tests
 * mypthread_create()
 * mypthread_exit()
 */
#include "mypthread.h"
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS	5

void *PrintHello(void *threadid) {
	long tid;
	tid = (long) threadid;
	printf("Hello World! It's me, thread #%ld!\n", tid);
	mypthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("usage: %s mypthread_policy(0|1|2|3)\n", argv[0]);
		exit(0);
	}
	mypthread_init(atoi(argv[1]));
	mypthread_t threads[NUM_THREADS];
	int rc;
	long t;
	for (t = 0; t < NUM_THREADS; t++) {
		printf("In main: creating thread %ld\n", t);
		rc = mypthread_create(&threads[t], NULL, PrintHello, (void *) t);
		if (rc) {
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	/* Last thing that main() should do */
	mypthread_exit(NULL);
	exit(0);
}
