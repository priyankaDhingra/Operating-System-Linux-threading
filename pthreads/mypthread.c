/*
 * Author: Abhijit Shanbhag, Priyanka Dhingra
 * This is the mypthread library file
 */
#include<stdio.h>
#include<stdlib.h>
#include "mypthread.h"
#include "queue_util.h"
#include "utilities.h"
#include <unistd.h>
#include <semaphore.h>
#include "sched.h"
#include <errno.h>
/*********************************************************************************************
 *GLOBALS
 *********************************************************************************************/
//Program parameters
#define KTHREAD_SLEEP_TIME 1000
#define STACK_SIZE (1024 * 1024)

//Globals
int mypthread_policy = 0;
int mypthread_count = 0;
int mypthread_th_gen = 0;
int cpu_cores;
int thread_per_core;
int max_kthread_count;
int current_kthread_count;

mypthread_t main_thread;
bool suicide_flag;

//Locks for synchronization
sem_t mypthread_sem; // For active,blocked and waiting queue
sem_t mutex_sem; // For mutex functionality
sem_t kthread_sem; //For running threads functionality
sem_t cond_sem; //For CV functionality
/*********************************************************************************************
 *Helper Functions
 *********************************************************************************************/
/*
 *Helper function to initialize all the locks used in the program
 */
void init_locks(void) {
	sem_init(&mypthread_sem, 0, 1);
	sem_init(&mutex_sem, 0, 1);
	sem_init(&cond_sem, 0, 1);
	sem_init(&kthread_sem, 0, 1);
}
/*
 * Helper function to init cpu_cores,thread_per_core,max_kthread_count and current_kthread.
 */
void init_kthread_info() {
	cpu_cores = get_cpu_cores();
	thread_per_core = get_hyperthreads();
	switch (mypthread_policy) {
	case ULONLY:
		printf("ULONLY policy selected\n");
		max_kthread_count = 1;
		printf("max_kthread_count: %d\n", max_kthread_count);
		break;
	case KLMATCHCORES:
		printf("KLMATCHCORES policy selected\n");
		max_kthread_count = cpu_cores;
		printf("max_kthread_count: %d\n", max_kthread_count);
		break;
	case KLMATCHHYPER:
		printf("KLMATCHHYPER policy selected\n");
		max_kthread_count = cpu_cores * thread_per_core;
		printf("max_kthread_count: %d\n", max_kthread_count);
		break;
	case KLALWAYS:
		printf("KLALWAYS policy selected\n");
		printf("No max_kthread_count set\n");
		break;
	}
}
/*
 *Helper function to create kthread by calling clone
 */
void create_kthread(void) {
	/* Allocate stack for child */
	char *stack; /* Start of stack buffer */
	char *stackTop; /* End of stack buffer */
	stack = malloc(STACK_SIZE);
	if (stack == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	stackTop = stack + STACK_SIZE;
	// Call the clone system call to create the child thread
	long pid = clone(&schedule_kthread, stackTop,
			SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, NULL);
	if (pid == -1) {
		perror("clone");
		exit(EXIT_FAILURE);
	}
	printf("clone() returned %ld\n", (long) pid);
}
/*
 * Helper function that creates mykthread_t node and executes next active user thread from runnable queue
 */
int schedule_kthread() {
	mykthread_t *k_th = (mykthread_t *) malloc(sizeof(mykthread_t));
	k_th->kth_id = gettid();
	//This value will be populated by mythread_scheduler
	k_th->th = NULL;
	//Add to the kthread list
	mykthread_add(k_th);
	mythread_scheduler(k_th, 0);
	return 0;
}
/*
 * This is an infinite loop program which switches to first active thread.
 * If it gets first active thread, it swaps the context with the first active thread.
 * If it doesn't get the first active thread, it sleeps for some time and checks again
 * mode = 0 set context and mode = 1 swaps context
 */
void mythread_scheduler(mykthread_t* current_kthread, int mode) {
	sem_wait(&mypthread_sem);
	if (mypthread_count == 0) {
		sem_post(&mypthread_sem);
		return;
	}
	sem_post(&mypthread_sem);
	mypthread_t* last_running_pthread = current_kthread->th;
	mypthread_t* next_active_th;			// = NULL;
	do {
		sem_wait(&mypthread_sem);
		next_active_th = mypthread_dequeue_first_active();
		sem_post(&mypthread_sem);
		if (next_active_th != NULL) {
			//Update running KTHREAD LL
			sem_wait(&kthread_sem);
			current_kthread->th = next_active_th;
			sem_post(&kthread_sem);
			if (mode == 0) {			//SET
				setcontext(next_active_th->ctx);
			} else {			//SWAP
				swapcontext(last_running_pthread->ctx, next_active_th->ctx);
			}
		} else {
			sleep(KTHREAD_SLEEP_TIME);
			//sched_yield();
		}
	} while (next_active_th == NULL);
}
/*********************************************************************************************
 *mypthread library functions
 *********************************************************************************************/
/**
 * Sets the policy for mypthread library on thread creation
 * the parameter kt can take one of four values that controls the behavior of pthread create()
 * (and potentially that of pthread exit()):
 */
void mypthread_init(int kt) {
	mypthread_policy = kt;
	init_kthread_info();
}
/*
 * Functionality for pthread_create()
 */
int mypthread_create(mypthread_t *thread, const mypthread_attr_t *attr,
		void *(*start_routine)(void *), void *arg) {
	if (mypthread_count == 0) {
		//mypthread_create was called for the first time
		//Init locks that is going to be used by the entire thread library
		init_locks();
		//Creating a thread for calling thread (assuming it is main function)
		main_thread.th_id = ++mypthread_th_gen;
		mypthread_count++;
		main_thread.ctx = (ucontext_t*) malloc(sizeof(ucontext_t));
		main_thread.ctx->uc_stack.ss_sp = (char*) malloc(sizeof(char) * 4096);
		main_thread.ctx->uc_stack.ss_size = 4096;
		main_thread.state = PS_RUNNING;
		//Insert into kthread
		mykthread_t* k_th = (mykthread_t *) malloc(sizeof(mykthread_t));
		k_th->kth_id = gettid();
		k_th->th = &main_thread;
		//Add to the kthread list
		sem_wait(&kthread_sem);
		//Assuming initially there will be only one kthread
		current_kthread_count = 1;
		mykthread_add(k_th);
		sem_post(&kthread_sem);
	}
	// Create node for thread
	ucontext_t* context = (ucontext_t*) malloc(sizeof(ucontext_t));
	thread->ctx = context;
	getcontext(thread->ctx);
	(*thread).ctx->uc_stack.ss_sp = (char*) malloc(sizeof(char) * 4096);
	(*thread).ctx->uc_stack.ss_size = 4096;
	(*thread).state = PS_ACTIVE;
	makecontext(thread->ctx, (void (*)()) start_routine, 1, arg);
	sem_wait(&mypthread_sem);
	thread->th_id = ++mypthread_th_gen;
	mypthread_count++;
	mypthread_enqueue(thread);
	sem_post(&mypthread_sem);
	//Create new kthread
	bool create_thread_flag;
	//Create Kthread as per Pthread library policy
	if (mypthread_policy == KLMATCHCORES || mypthread_policy == KLMATCHHYPER) {
		sem_wait(&kthread_sem);
		create_thread_flag = false;
		if (current_kthread_count < max_kthread_count) {
			current_kthread_count++;
			create_thread_flag = true;
		}
		sem_post(&kthread_sem);
		if (create_thread_flag) {
			create_kthread();
		}
	} else if (mypthread_policy == KLALWAYS) {
		sem_wait(&kthread_sem);
		current_kthread_count++;
		sem_post(&kthread_sem);
		create_kthread();
	}
	return 0;
}
/*
 *	Functionality for pthread_exit()
 */
void mypthread_exit(void *retval) {
	//First get the current executing kthread
	mykthread_t* current_kthread = mykthread_get(gettid());
	//First make all other waiting threads active
	sem_wait(&mypthread_sem);
	mypthread_count--;
	current_kthread->th->state = PS_DEAD;
	mypthread_update_waiting_threads(current_kthread->th->th_id);
	sem_post(&mypthread_sem);
	//Free pthread
	free(current_kthread->th->ctx);
	//Just destroy the mypthread node and run the next active thread
	mythread_scheduler(current_kthread, 0);
}
/*
 *	Functionality for pthread_yield()
 */
int mypthread_yield(void) {
	//get current thread and change the status to PS_ACTIVE
	mykthread_t* current_kthread = mykthread_get(gettid());
	current_kthread->th->state = PS_ACTIVE;
	sem_wait(&mypthread_sem);
	mypthread_enqueue(current_kthread->th);
	sem_post(&mypthread_sem);
	//Swap context to next runnable thread
	mythread_scheduler(current_kthread, 1);
	return 0;
}
/*
 *	Functionality for pthread_join()
 */
int mypthread_join(mypthread_t thread, void **retval) {
	//Search for target thread
	sem_wait(&mypthread_sem);
	mypthread_t* target_thread = mypthread_get(thread.th_id);
	sem_wait(&kthread_sem);
	mykthread_t* current_kthread = mykthread_get(gettid());
	sem_post(&kthread_sem);
	sem_post(&mypthread_sem);
	if (target_thread == NULL) {
		if (mykthread_is_running(thread.th_id)) {
			// Thread is currently running by some other kthread
			//Change status of the current thread and run next active thread
			current_kthread->th->state = PS_WAITING;
			current_kthread->th->dependent_th = thread.th_id;
			sem_wait(&mypthread_sem);
			mypthread_enqueue(current_kthread->th);
			sem_post(&mypthread_sem);
			//Swap context to the next active thread
			mythread_scheduler(current_kthread, 1);
			return 0;
		} else {
			//Immediately return if no thread is waiting
			return 0;
		}
	} else {
		if (target_thread->state != PS_DEAD) {
			//Current thread is currently waiting due to other thread
			//Change status of the current thread and run next active thread
			current_kthread->th->state = PS_WAITING;
			current_kthread->th->dependent_th = thread.th_id;
			sem_wait(&mypthread_sem);
			mypthread_enqueue(current_kthread->th);
			sem_post(&mypthread_sem);
			//Swap context to the next active thread
			mythread_scheduler(current_kthread, 1);
			return 0;
		} else {
			//No need to wait. Immediately return
			return 0;
		}
	}
}
/*
 *	Functionality for pthread_self()
 */
mypthread_t mypthread_self(void) {
	sem_wait(&kthread_sem);
	mykthread_t* current_kthread = mykthread_get(gettid());
	sem_post(&kthread_sem);
	return *(current_kthread->th);
}
/*
 *	Functionality for pthread_equal()
 */
int mypthread_equal(mypthread_t t1, mypthread_t t2) {
	if (t1.th_id == t2.th_id) {
		return true;
	} else {
		return false;
	}
}
/*
 *	Functionality for pthread_mutex_init()
 */
int mypthread_mutex_init(mypthread_mutex_t *mutex,
		const mypthread_mutexattr_t *attr) {
	mutex->lock = false;
	mutex->blocked_q_head = NULL;
	mutex->blocked_q_tail = NULL;
	return 0;
}
/*
 *	Functionality for pthread_mutex_destroy()
 */
int mypthread_mutex_destroy(mypthread_mutex_t *mutex) {
	sem_wait(&mutex_sem);
	mypthread_mutex_init(mutex, NULL);
	sem_post(&mutex_sem);
	return 0;
}
/*
 *	Functionality for pthread_mutex_lock()
 */
int mypthread_mutex_lock(mypthread_mutex_t *mutex) {
	sem_wait(&mutex_sem);
	if (!(mutex->lock)) {
		mutex->lock = true;
		sem_post(&mutex_sem);
	} else {
		mykthread_t* current_kthread = mykthread_get(gettid());
		//No need to acquire mypthread_lock as the mypthread_t are not in the run queue
		current_kthread->th->state = PS_BLOCKED;
		ll_add(current_kthread->th, &(mutex->blocked_q_head),
				&(mutex->blocked_q_tail));
		//This will ensure that when the blocked thread becomes runnable, it executes the mutex lock logic again
		makecontext(current_kthread->th->ctx, (void (*)()) mypthread_mutex_lock,
				1, mutex);
		sem_wait(&mypthread_sem);
		mypthread_enqueue(current_kthread->th);
		sem_post(&mypthread_sem);
		sem_post(&mutex_sem);
		//Set context to the next active thread
		mythread_scheduler(current_kthread, 0);
	}
	return 0;
}
/*
 *	Functionality for pthread_mutex_trylock()
 */
int mypthread_mutex_trylock(mypthread_mutex_t *mutex) {
	sem_wait(&mutex_sem);
	if (!(mutex->lock)) {
		//Lock not acquired
		//acquire lock and return
		mutex->lock = true;
		sem_post(&mutex_sem);
		return 0;
	} else {
		//Lock acquired
		//return immediately with error number
		//Assuming default error = -1
		return EBUSY;
	}
}
/*
 *	Functionality for pthread_mutex_unlock()
 */
int mypthread_mutex_unlock(mypthread_mutex_t *mutex) {
	sem_wait(&mutex_sem);
	mutex->lock = false;
	ll_free(&(mutex->blocked_q_head), &(mutex->blocked_q_tail));
	sem_post(&mutex_sem);
	return 0;
}
/*
 *	Functionality for pthread_cond_init()
 */
int mypthread_cond_init(mypthread_cond_t *cond,
		const mypthread_condattr_t *attr) {
	cond->cond_q_head = NULL;
	cond->cond_q_tail = NULL;
	return 0;
}
/*
 *	Functionality for pthread_cond_destroy()
 */
int mypthread_cond_destroy(mypthread_cond_t *cond) {
	sem_wait(&cond_sem);
	mypthread_cond_init(cond, NULL);
	sem_post(&cond_sem);
	return 0;
}
/*
 *	Functionality for pthread_cond_broadcast()
 */
int mypthread_cond_broadcast(mypthread_cond_t *cond) {
	sem_wait(&cond_sem);
	sem_wait(&mypthread_sem);
	//Make active all threads sleeping on cond variable
	ll_free(&(cond->cond_q_head), &(cond->cond_q_tail));
	sem_post(&mypthread_sem);
	sem_post(&cond_sem);
	return 0;
}
/*
 *	Functionality for pthread_cond_signal()
 */
int mypthread_cond_signal(mypthread_cond_t *cond) {
	sem_wait(&cond_sem);
	sem_wait(&mypthread_sem);
	ll_free_one(&(cond->cond_q_head));
	sem_post(&mypthread_sem);
	sem_post(&cond_sem);
	return 0;
}
/*
 *	Functionality for pthread_cond_wait()
 */
int mypthread_cond_wait(mypthread_cond_t *cond, mypthread_mutex_t *mutex) {
	sem_wait(&cond_sem);
	//Unblock all threads locked by mutex
	mypthread_mutex_unlock(mutex);
	//Make the current thread wait for the signal
	mykthread_t* current_kthread = mykthread_get(gettid());
	current_kthread->th->state = PS_BLOCKED;
	ll_add(current_kthread->th, &(cond->cond_q_head), &(cond->cond_q_tail));
	sem_wait(&mypthread_sem);
	mypthread_enqueue(current_kthread->th);
	sem_post(&mypthread_sem);
	sem_post(&cond_sem);
	mythread_scheduler(current_kthread, 1);
	mypthread_mutex_lock(mutex);
	return 0;
}
