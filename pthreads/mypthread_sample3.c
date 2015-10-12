/*
 * Sample program which tests
 * mypthread_create()
 * mypthread_exit()
 * mypthread_join()
 * mypthread_mutex_lock()
 * mypthread_mutex_unlock()
 */
#include <stdio.h>
#include <stdlib.h>
#include "mypthread.h"

void *functionC();
mypthread_mutex_t mutex1;
int counter = 0;

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("usage: %s mypthread_policy(0|1|2|3)\n", argv[0]);
		exit(0);
	}
	mypthread_init(atoi(argv[1]));
	int rc1, rc2;
	mypthread_t thread1, thread2;
	mypthread_mutex_init(&mutex1, NULL);

	/* Create independent threads each of which will execute functionC */

	if ((rc1 = mypthread_create(&thread1, NULL, functionC, NULL))) {
		printf("Thread creation failed: %d\n", rc1);
	}

	if ((rc2 = mypthread_create(&thread2, NULL, functionC, NULL))) {
		printf("Thread creation failed: %d\n", rc2);
	}

	/* Wait till threads are complete before main continues. Unless we  */
	/* wait we run the risk of executing an exit which will terminate   */
	/* the process and all threads before the threads have completed.   */

	mypthread_join(thread1, NULL);
	mypthread_join(thread2, NULL);

	mypthread_exit(NULL);
	exit(0);
}

void *functionC() {
	mypthread_mutex_lock(&mutex1);
	counter++;
	printf("Counter value: %d\n", counter);
	mypthread_mutex_unlock(&mutex1);
	mypthread_exit(NULL);
}
