/**
 * threadpool.c
 *
 * This file will contain your implementation of a threadpool.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "pthread.h"

#include "threadpool.h"
/* Job */
struct task {
	//task* prev;
	void* (*func)(void* arg); /* func pointer          */
	void* arg; /* func's argument       */
	struct task *next;
} task;

// _threadpool is the internal threadpool structure that is
// cast to type "threadpool" before it given out to callers
typedef struct _threadpool_st {
	// you should fill in this structure with whatever you need
	int no_of_threads;
	int no_of_tasks_in_Q;
	int no_of_running_threads;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	pthread_t *threads;
	struct task *taskQ_front;
	struct task *taskQ_rear;

} _threadpool;

/*
 * QUEUE IMPL
 *
 */

//end //remove when done
void push_task(_threadpool *pool, struct task *mytask) {
	if ((pool->no_of_tasks_in_Q) == 0) {
		pool->taskQ_front = mytask;
		pool->taskQ_rear = mytask;
		(pool->taskQ_rear->next) = NULL;
	} else {
		pool->taskQ_rear->next = mytask;
		mytask->next = NULL;
		pool->taskQ_rear = mytask;
	}
	pool->no_of_tasks_in_Q++;
}

void *threadpool_scheduler(threadpool p) {
	_threadpool * pool = (_threadpool *) p;
	struct task* cur;	//The q element

	while (1) {
		pthread_mutex_lock(&(pool->mutex));

		while (pool->no_of_tasks_in_Q == 0) {
			pthread_mutex_unlock(&(pool->mutex));
			pthread_cond_wait(&(pool->cond), &(pool->mutex));

		}

		cur = pool->taskQ_front;

		pool->no_of_tasks_in_Q--;

		if (pool->no_of_tasks_in_Q == 0) {
			pool->taskQ_front = NULL;
			pool->taskQ_rear = NULL;
		} else {
			pool->taskQ_front = cur->next;
		}

		if (pool->no_of_tasks_in_Q == 0) {
			pthread_cond_signal(&(pool->cond));
		}
		pthread_mutex_unlock(&(pool->mutex));
		(cur->func)(cur->arg);
		free(cur);
	}
}

threadpool create_threadpool(int num_threads_in_pool,int mode) {
	_threadpool *pool;
	// sanity check the argument
	if ((num_threads_in_pool <= 0) || (num_threads_in_pool > MAXT_IN_POOL))
		return NULL;
	pool = (_threadpool *) malloc(sizeof(_threadpool));
	if (pool == NULL) {
		fprintf(stderr, "Out of memory creating a new threadpool!\n");
		return NULL;
	}
	pool->threads = (pthread_t*) malloc(
			sizeof(pthread_t) * num_threads_in_pool);
	if (!pool->threads) {
		fprintf(stderr, "Out of memory creating a new threadpool!\n");
		return NULL;
	}

	pool->no_of_threads = num_threads_in_pool;
	pool->no_of_tasks_in_Q = 0;

	/*
	 * Initialize Q
	 *
	 */
	pool->taskQ_front = pool->taskQ_rear = NULL;
	//check if locks are aquired
	if ((pthread_mutex_init(&(pool->mutex), NULL))
			|| (pthread_cond_init(&(pool->cond), NULL))){
		return NULL;
	}
	//create thread queue
	int i = 0;
	while (i < num_threads_in_pool) {
		if (pthread_create(&(pool->threads[i]), NULL, threadpool_scheduler,
				(void*) pool) != 0) {
			return NULL;
		}
		pool->no_of_threads++;
		pool->no_of_running_threads++;
		i++;
	}
	return (threadpool) pool;
}

void dispatch(threadpool from_me, dispatch_fn dispatch_to_here, void *arg) {
	_threadpool *pool = (_threadpool *) from_me;
	if (pool == NULL || dispatch_to_here == NULL) {
		printf("Threadpool invalid");

	}
	struct task *mytask;
	mytask = (struct task*) malloc(sizeof(struct task));
	if (mytask == NULL) {
		printf("No memory allocation");
	}
	mytask->func = dispatch_to_here;
	mytask->arg = arg;
	pthread_mutex_lock(&(pool->mutex));

		if (&(pool->no_of_threads) == &(pool->no_of_tasks_in_Q)) {
			printf("threadpool_queue_full");
		} else {
			push_task(pool, mytask);
			if (pthread_cond_signal(&(pool->cond))) {
				printf("threadpool_lock_failure");
			}
			pthread_cond_signal(&(pool->cond));
		}
	if (pthread_mutex_unlock(&(pool->mutex) )) {
		printf("not unlock");
	}
}

void destroy_threadpool(threadpool destroyme) {
	_threadpool *pool = (_threadpool *) destroyme;
	void* nothing;
	int i = 0;
	free(pool->threads);

	pthread_mutex_destroy(&(pool->mutex));
	pthread_cond_destroy(&(pool->cond));
	return;
}

