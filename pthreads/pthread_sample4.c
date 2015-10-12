/*
 * This program tests mymypthread_create(), mymypthread_exit() and mymypthread_join(), mypthread_mutex_lock() and mypthread_mutex_unlock()
 */
#include <stdio.h>
#include <pthread.h>
#include <errno.h>

#define NTHREADS 10
void *thread_function(void *);
pthread_mutex_t mutex1;
int counter = 0;

main() {
	pthread_mutex_init(&mutex1, NULL);
	pthread_t thread_id[NTHREADS];
	int i, j;
	printf("EBUSY:%d", EBUSY);

	for (i = 0; i < NTHREADS; i++) {
		pthread_create(&thread_id[i], NULL, thread_function, NULL);
	}

	for (j = 0; j < NTHREADS; j++) {
		pthread_join(thread_id[j], NULL);
	}

	/* Now that all threads are complete I can print the final result.     */
	/* Without the join I could be printing a value before all the threads */
	/* have been completed.                                                */

	printf("Final counter value: %d\n", counter);
	pthread_exit(NULL);
}

void *thread_function(void *dummyPtr) {
	printf("Thread number %ld\n", pthread_self());
	pthread_mutex_lock(&mutex1);
	counter++;
	pthread_mutex_unlock(&mutex1);
	pthread_exit(NULL);
}
