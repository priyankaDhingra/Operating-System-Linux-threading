#ifndef QUEUE_UTILITIES
#define QUEUE_UTILITIES
#include "mypthread.h"
/*
 *
 */
struct mypthread_queue_node {
	struct mypthread_queue_node *prev;
	mypthread_t *th;
	struct mypthread_queue_node *next;
};

struct mykthread_ll_node {
	struct mykthread_ll_node *prev;
	mykthread_t *kth;
	struct mykthread_ll_node *next;
};
/*
 * mypthread queue operations
 */
void mypthread_enqueue(mypthread_t* data);
mypthread_t* mypthread_dequeue_first_active(void);
mypthread_t* mypthread_get(int thread_id);
void mypthread_update(mypthread_t* thread);
void mypthread_update_waiting_threads(int thread_id);
/*
 * mykthread queue operations
 */
void mykthread_add(mykthread_t* data);
//void mykthread_update(mykthread_t* k_th);
void mykthread_update(int kthread_id, mypthread_t* pthread);
mykthread_t* mykthread_get(int kthread_id);
bool mykthread_delete(int kthread_id);
bool mykthread_is_running(int thread_id);
/*
 * Generic LL operations
 */
void ll_add(mypthread_t* data, struct mypthread_queue_node** head,
		struct mypthread_queue_node** tail);
void ll_free(struct mypthread_queue_node** head,
		struct mypthread_queue_node** tail);
void ll_free_one(struct mypthread_queue_node** head);
#endif
