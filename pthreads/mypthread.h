#include "ucontext.h"
#ifndef MYPTHREAD
#define MYPTHREAD

typedef int bool;
#define true 1
#define false 0

/**
 * Define mypthread_policy
 * ULONLY: only create a new user-level thread.
 * KLMATCHCORES: create a user-level thread and a kernel-level thread if the number of
 * kernel-level threads created thus far is less than the number of cores on the machine.
 * KLMATCHHYPER: create a user-level thread and a kernel-level thread if the number of
 * kernel-level threads created thus far is less than the total number of hardware hyper-
 * threads on the machine.
 * KLALWAYS: always create a user-level thread and a kernel-level thread.
 */
#define ULONLY 0
#define KLMATCHCORES 1
#define KLMATCHHYPER 2
#define KLALWAYS 3

/**
 * Defines the state of the user thread
 */
enum mypthread_state {
	PS_ACTIVE, PS_RUNNING, PS_WAITING, PS_BLOCKED, PS_DEAD,
};

/*
 *	Structure for mypthread_t
 */
typedef struct {
	int th_id; //Thread ID
	ucontext_t* ctx; //Context
	enum mypthread_state state; //State of thread
	int dependent_th; //Id in case of dependency(waitin)
} mypthread_t;

/*
 *	Structure for mypthread_attr_t
 *	Not supported for mypthread functionality
 */
typedef struct {
} mypthread_attr_t;

/*
 * Mykthread_t struct
 */
typedef struct {
	int kth_id; // ID of the kthread processing the user thread
	mypthread_t *th; //Pointer to the user thread that is being processed
} mykthread_t;
/*
 * mypthread_mutex_t struct
 */
typedef struct {
	bool lock;
	struct mypthread_queue_node* blocked_q_head;
	struct mypthread_queue_node* blocked_q_tail;
} mypthread_mutex_t;

/*
 *
 */
typedef int mypthread_mutexattr_t;
/*
 *	mypthread_cond_t struct
 */
typedef struct {
	struct mypthread_queue_node* cond_q_head;
	struct mypthread_queue_node* cond_q_tail;
} mypthread_cond_t;
/*
 *
 */
typedef int mypthread_condattr_t;
/*
 * Helper functions
 */
void init_kthread_info(void);
void mythread_scheduler(mykthread_t* current_thread, int mode);
void init_locks(void);
void create_kthread(void);
int schedule_kthread();
/*
 * mypthread library functions
 */
void mypthread_init(int kt);
int mypthread_create(mypthread_t *thread, const mypthread_attr_t *attr,
		void *(*start_routine)(void *), void *arg);
//mypthread_self
void mypthread_exit(void *retval);
int mypthread_yield(void);
int mypthread_join(mypthread_t thread, void **retval); //Extra credit implemented
int mypthread_mutex_init(mypthread_mutex_t *mutex,
		const mypthread_mutexattr_t *attr);
int mypthread_mutex_lock(mypthread_mutex_t *mutex);
int mypthread_mutex_trylock(mypthread_mutex_t *mutex); //Extra Credit implemented
int mypthread_mutex_unlock(mypthread_mutex_t *mutex);
int mypthread_cond_init(mypthread_cond_t *cond,
		const mypthread_condattr_t *attr);
int mypthread_cond_broadcast(mypthread_cond_t *cond);
int mypthread_cond_signal(mypthread_cond_t *cond);
int mypthread_cond_wait(mypthread_cond_t *cond, mypthread_mutex_t *mutex);
int mypthread_mutex_destroy(mypthread_mutex_t *mutex); //Extra Credit  implemented
int mypthread_cond_destroy(mypthread_cond_t *cond); //Extra Credit implemented
mypthread_t mypthread_self(void); //Extra Credit implemented
int mypthread_equal(mypthread_t t1, mypthread_t t2); //Extra Credit implemented
#endif
