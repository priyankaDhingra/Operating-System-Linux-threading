#include <stddef.h>
#include<stdio.h>
#include <stdlib.h>
#include "mypthread.h"
#include "queue_util.h"
#include "utilities.h"

//TODO handle Synchronization
/*
 * MYPTHREAD QUEUE: Doubly queue for active,waiting and blocked threads
 */
struct mypthread_queue_node* mypthread_q_head = NULL;
struct mypthread_queue_node* mypthread_q_tail = NULL;
/*
 * KTHREAD LL: Doubly linkedlist for currently running threads
 */
struct mykthread_ll_node* mykthread_ll_head = NULL;
struct mykthread_ll_node* mykthread_ll_tail = NULL;
/*
 * Helper function to create mypthread_queue_node
 */
struct mypthread_queue_node* create_mypthread_queue_node(mypthread_t* data) {
	struct mypthread_queue_node* temp = (struct mypthread_queue_node *) malloc(
			1 * sizeof(struct mypthread_queue_node));
	temp->next = NULL;
	temp->prev = NULL;
	temp->th = data;
	return temp;
}
/*
 *Enque operation on MYPTHREAD QUEUE
 */
void mypthread_enqueue(mypthread_t* data) {
	ll_add(data, &mypthread_q_head, &mypthread_q_tail);
}
/*
 *Dequeues first active thread from MYPTHREAD QUEUE
 */
mypthread_t* mypthread_dequeue_first_active() {
	struct mypthread_queue_node* del = mypthread_q_head;
	while (del != NULL) {
		if (del->th->state == PS_ACTIVE) {
			break; //Found the delete node
		} else {
			del = del->next;
		}
	}
	if (del == NULL) {
		return NULL; // means not found
	}
	//Check if the delete node is the head node
	if (mypthread_q_head == del) {
		mypthread_q_head = del->next;
	}
	/* Change next only if node to be deleted is NOT the last node */
	if (del->next != NULL)
		del->next->prev = del->prev;
	/* Change prev only if node to be deleted is NOT the first node */
	if (del->prev != NULL)
		del->prev->next = del->next;
	mypthread_t* mythread_node = del->th;
	/* Finally, free the memory occupied by del*/
	free(del);
	return mythread_node;
}
/*
 * Search/Get operation on MYPTHREAD QUEUE
 */
mypthread_t* mypthread_get(int thread_id) {
	struct mypthread_queue_node* temp = mypthread_q_head;
	while (temp != NULL) {
		if (temp->th->th_id == thread_id) {
			return temp->th;
		} else {
			temp = temp->next;
		}
	}
	return NULL; // Means not found
}
/*
 * Updates status for waiting threads for MYPTHREAD QUEUE
 */
void mypthread_update_waiting_threads(int thread_id) {
	struct mypthread_queue_node* temp = mypthread_q_head;
	while (temp != NULL) {
		if (temp->th->dependent_th == thread_id) {
			temp->th->state = PS_ACTIVE;
		}
		temp = temp->next;
	}
}
/*
 * Helper function to create mypthread_queue_node
 */
struct mykthread_ll_node* create_mykthread_ll_node(mykthread_t* data) {
	struct mykthread_ll_node* temp = (struct mykthread_ll_node *) malloc(
			1 * sizeof(struct mykthread_ll_node));
	temp->next = NULL;
	temp->prev = NULL;
	temp->kth = data;
	return temp;
}
/*
 * ADD operation on KTHREAD LL
 */
void mykthread_add(mykthread_t* data) {
	struct mykthread_ll_node* temp = create_mykthread_ll_node(data);
	if (mykthread_ll_head == NULL) {
		//This means it is the first node
		mykthread_ll_head = mykthread_ll_tail = temp;
	} else {
		mykthread_ll_tail->next = temp;
		temp->prev = mykthread_ll_tail;
		mykthread_ll_tail = temp;
	}
}
/*
 * Checks if particular Uthread is currently running or not on KTHREAD LL
 */
bool mykthread_is_running(int thread_id) {
	struct mykthread_ll_node* temp = mykthread_ll_head;
	while (temp != NULL) {
		if (temp->kth->th->th_id == thread_id) {
			return true;
		}
		temp = temp->next;
	}
	return false;
}
/*
 * get operation for KTHREAD LL
 */
mykthread_t* mykthread_get(int kthread_id) {
	struct mykthread_ll_node* temp = mykthread_ll_head;
	while (temp != NULL) {
		if (temp->kth->kth_id == kthread_id) {
			return temp->kth;
		}
		temp = temp->next;
	}
	return NULL;
}
/*
 * Update/add new operation for KTHREAD LL
 */
void mykthread_update(int kthread_id, mypthread_t* pthread) {
	struct mykthread_ll_node* temp = mykthread_ll_head;
	while (temp != NULL) {
		if (temp->kth->kth_id == kthread_id) {
			temp->kth->th = pthread;
			return;
		} else {
			temp = temp->next;
		}
	}
	//If control comes here it means that the mykthread_ll_node for that particular kthread was not found
	mykthread_t* k_th = (mykthread_t *) malloc(sizeof(mykthread_t));
	k_th->kth_id = gettid();
	k_th->th = pthread;
	//Add to the kthread list
	mykthread_add(k_th);
}
/*
 * Adds mypthread_queue_node in the linked list pointed by head
 */
void ll_add(mypthread_t* data, struct mypthread_queue_node** head,
		struct mypthread_queue_node** tail) {
	struct mypthread_queue_node* temp = create_mypthread_queue_node(data);
	if (*head == NULL) { //for the first node
		*head = temp;
		*tail = temp;
	} else {
		(*tail)->next = temp;
		temp->prev = *tail;
		*tail = temp;
	}
}
/*
 * Method that frees linked list pointed by head,tail and updates
 */
void ll_free(struct mypthread_queue_node** head,
		struct mypthread_queue_node** tail) {
	if (*head == NULL) {
		return; //LL already empty
	}
	struct mypthread_queue_node* temp = (*head);
	struct mypthread_queue_node* del;
	while (temp != NULL) {
		del = temp;
		temp = temp->next;
		del->th->state = PS_ACTIVE;
		free(del);
	}
	(*head) = (*tail) = NULL;
}
/*
 * Method that frees only one node of  the linked list pointed by head,tail and updates
 */
void ll_free_one(struct mypthread_queue_node** head) {
	if (*head == NULL) {
		return; //LL already empty
	}
	struct mypthread_queue_node* del = (*head);
	(*head) = (*head)->next;
	del->th->state = PS_ACTIVE;
	free(del);
}
