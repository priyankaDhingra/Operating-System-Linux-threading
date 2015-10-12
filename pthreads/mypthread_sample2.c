/*
 * Sample program which tests
 * mypthread_create()
 * mypthread_exit()
 * mypthread_join()
 */
#include <stdio.h>
#include <stdlib.h>
#include "mypthread.h"

void *print_message_function(void *ptr);

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("usage: %s mypthread_policy(0|1|2|3)\n", argv[0]);
		exit(0);
	}
	mypthread_init(atoi(argv[1]));
	mypthread_t thread1, thread2;
	const char *message1 = "Thread 1";
	const char *message2 = "Thread 2";
	int iret1, iret2;

	/* Create independent threads each of which will execute function */

	iret1 = mypthread_create(&thread1, NULL, print_message_function,
			(void*) message1);
	if (iret1) {
		fprintf(stderr, "Error - mypthread_create() return code: %d\n", iret1);
		exit(EXIT_FAILURE);
	}

	iret2 = mypthread_create(&thread2, NULL, print_message_function,
			(void*) message2);
	if (iret2) {
		fprintf(stderr, "Error - mypthread_create() return code: %d\n", iret2);
		exit(EXIT_FAILURE);
	}

	printf("mypthread_create() for thread 1 returns: %d\n", iret1);
	printf("mypthread_create() for thread 2 returns: %d\n", iret2);

	/* Wait till threads are complete before main continues. Unless we  */
	/* wait we run the risk of executing an exit which will terminate   */
	/* the process and all threads before the threads have completed.   */

	mypthread_join(thread1, NULL);
	mypthread_join(thread2, NULL);
	printf("All thread exits\n");

	mypthread_exit(NULL);
	exit(0);
}

void *print_message_function(void *ptr) {
	char *message;
	message = (char *) ptr;
	printf("%s \n", message);
	mypthread_exit(NULL);
}
