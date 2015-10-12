/*
 * Sample program which tests
 * mypthread_create()
 * mypthread_exit()
 * mypthread_join()
 * mypthread_mutex_lock()
 * mypthread_mutex_unlock()
 * mypthread_cond_signal()
 * mypthread_cond_wait()
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include "mypthread.h"

mypthread_mutex_t count_mutex;
mypthread_mutex_t condition_mutex;
mypthread_cond_t condition_cond;

void *functionCount1();
void *functionCount2();
int count = 0;
#define COUNT_DONE  10
#define COUNT_HALT1  3
#define COUNT_HALT2  6

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("usage: %s mypthread_policy(0|1|2|3)\n", argv[0]);
		exit(0);
	}
	mypthread_init(atoi(argv[1]));
	mypthread_mutex_init(&count_mutex, NULL);
	mypthread_mutex_init(&condition_mutex, NULL);
	mypthread_cond_init(&condition_cond, NULL);
	mypthread_t thread1, thread2;

	mypthread_create(&thread1, NULL, &functionCount1, NULL);
	mypthread_create(&thread2, NULL, &functionCount2, NULL);
	mypthread_join(thread1, NULL);
	mypthread_join(thread2, NULL);

	mypthread_exit(NULL);
}

void *functionCount1() {
	for (;;) {
		mypthread_mutex_lock(&condition_mutex);
		while (count >= COUNT_HALT1 && count <= COUNT_HALT2) {
			mypthread_cond_wait(&condition_cond, &condition_mutex);
		}
		mypthread_mutex_unlock(&condition_mutex);

		mypthread_mutex_lock(&count_mutex);
		count++;
		printf("Counter value functionCount1: %d\n", count);
		mypthread_mutex_unlock(&count_mutex);

		if (count >= COUNT_DONE) {
			mypthread_exit(NULL);
			//return (NULL);
		}
	}
}

void *functionCount2() {
	for (;;) {
		mypthread_mutex_lock(&condition_mutex);
		if (count < COUNT_HALT1 || count > COUNT_HALT2) {
			mypthread_cond_signal(&condition_cond);
		}
		mypthread_mutex_unlock(&condition_mutex);

		mypthread_mutex_lock(&count_mutex);
		count++;
		printf("Counter value functionCount2: %d\n", count);
		mypthread_mutex_unlock(&count_mutex);

		if (count >= COUNT_DONE) {
			mypthread_exit(NULL);
			//return (NULL);
		}

	}

}
