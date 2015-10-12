/*
 * Sample program which tests
 * mypthread_create()
 * mypthread_exit()
 * mypthread_join()
 * mypthread_mutex_lock()
 * mypthread_mutex_unlock()
 */
#include <stdio.h>
#include "mypthread.h"

#define NTHREADS 10
void *thread_function(void *);
mypthread_mutex_t mutex1;
int counter = 0;

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("usage: %s mypthread_policy(0|1|2|3)\n", argv[0]);
		exit(0);
	}
	mypthread_init(atoi(argv[1]));
	mypthread_mutex_init(&mutex1, NULL);
	mypthread_t thread_id[NTHREADS];
	int i, j;

	for (i = 0; i < NTHREADS; i++) {
		mypthread_create(&thread_id[i], NULL, thread_function, NULL);
	}

	for (j = 0; j < NTHREADS; j++) {
		mypthread_join(thread_id[j], NULL);
	}

	/* Now that all threads are complete I can print the final result.     */
	/* Without the join I could be printing a value before all the threads */
	/* have been completed.                                                */

	printf("Final counter value: %d\n", counter);
	mypthread_exit(NULL);
}

void *thread_function(void *dummyPtr) {
	//printf("Thread number %ld\n", mypthread_self());
	mypthread_mutex_lock(&mutex1);
	counter++;
	mypthread_mutex_unlock(&mutex1);
	mypthread_exit(NULL);
}
