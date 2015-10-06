#include "mypthread.h"
#include "kernelthreadcount.c"

static int policy_defined = ULONLY
;
static int no_of_kt = 0;
static int thread_count = 0;

struct thread_node {
	mypthread_t *tcb;
	struct thread_node *next;
};
struct thread_node *front;
struct thread_node *rear;

//remove when done
void traverse() {
	struct thread_node *temp;
	temp = front;
	if (front != NULL) {
		printf("element--%d\n", (int) front->tcb->thread_id);
	}
	while (temp->next != front) {
		printf("element--%d\n", (int) temp->tcb->thread_id);
		temp = temp->next;
	}
}
//end //remove when done
void push_thread(mypthread_t *tcb) {
	struct thread_node *temp = malloc(sizeof(struct thread_node));
	temp->tcb = tcb;
	temp->next = NULL;
	if (front == NULL) {
		front = temp;
		rear = front;
	} else {
		rear->next = temp;
		rear = rear->next;
	}
	rear->next = front;
}

mypthread_t* pop_thread() {
	mypthread_t *tcb = malloc(sizeof(mypthread_t));
	struct thread_node *temp;
	if (front == rear | front == NULL) {
		printf("Empty Q");
	} else {
		temp = front;
		tcb = temp->tcb;
		front = front->next;
		rear->next = front;
		free(temp);
	}
	return tcb;
}

int mypthread_create(mypthread_t *thread, void *(*start_routine)(void *),
		void *arg) {
	switch (policy_defined) {
	case 0: //ULONLY
		if (thread_count <= 0) {
			mypthread_t* main_thread = (mypthread_t *) malloc(
					sizeof(mypthread_t));
			main_thread->thread_id = ++thread_count;
			ucontext_t* context = (ucontext_t*) malloc(sizeof(ucontext_t));
			main_thread->ctx = context;
			main_thread->ctx->uc_stack.ss_sp = (char*) malloc(
					sizeof(char) * 4096);
			main_thread->ctx->uc_stack.ss_size = 4096;
			main_thread->cur_state = PS_ACTIVE;
			push_thread(main_thread);
		}
		break;
	case 1: //KLMATCHCORES
		if (no_of_kt < get_no_of_cores()) {

		}
		break;
	case 2: //KLMATCHHYPER
		break;
	case 3: //KLALWAYS
		break;
	default:
		break;
	}
	ucontext_t* context = (ucontext_t*) malloc(sizeof(ucontext_t));
	thread->ctx = context;
	getcontext(thread->ctx);
	(*thread).ctx->uc_stack.ss_sp = (char*) malloc(sizeof(char) * 4096);
	(*thread).ctx->uc_stack.ss_size = 4096;
	(*thread).cur_state = PS_ACTIVE;
	thread->thread_id = thread_count++;
	makecontext(thread->ctx, (void (*)()) start_routine, 1, arg);
	push_thread(thread);

	return 0;
}
/*
 * ULONLY: only create a new user-level thread.
 * KLMATCHCORES: create a user-level thread and a kernel-level thread if the number of
 * kernel-level threads created thus far is less than the number of cores on the machine.
 * KLMATCHHYPER: create a user-level thread and a kernel-level thread if the number of
 * kernel-level threads created thus far is less than the total number of hardware hyper- threads on the machine.
 * KLALWAYS: always create a user-level thread and a kernel-level thread.
 * */

void mypthread_init(int kt) {
	policy_defined = kt;
}
