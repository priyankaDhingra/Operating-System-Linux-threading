#include <mypthread.h>

#define ULONLY 0;
#define KLMATCHCORES 1;
#define KLMATCHHYPER 2;
#define KLALWAYS 3;

static int thread_count = 0;
struct thread_node {
	struct tcb_thread *tcb;
	struct thread_node *next;
};
struct thread_node *front;
struct thread_node *rear;
void push_thread(struct tcb_thread *tcb) {
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

struct tcb_thread* pop_thread() {
	struct tcb_thread* tcb = malloc(sizeof(struct tcb_thread));
	struct thread_node *temp;
	if (front == rear | front == NULL) {
		printf('Empty Q');
	} else {
		temp = front;
		tcb = temp->tcb;
		front = front->next;
		rear->next = front;
		free(temp);
	}
	return tcb;
}

int mypthread_create(tcb_thread *thread, void *(*start_routine)(void *),
		void *arg) {
	ucontext_t* context = (ucontext_t*) malloc(sizeof(ucontext_t));
	thread->ctx = context;
	getcontext(thread->ctx);
	(*thread).ctx->uc_stack.ss_sp = (char*) malloc(sizeof(char) * 4096);
	(*thread).ctx->uc_stack.ss_size = 4096;
	(*thread).state = PS_ACTIVE;
	thread->th_id = thread_count++;
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

int mypthread_init(int kt) {
	switch (kt) {
	case 0: //ULONLY
		if (thread_count <= 0) {
			tcb_thread* main_thread = (tcb_thread *) malloc(
					sizeof(tcb_thread));
			main_thread->th_id = thread_count++;
			ucontext_t* context = (ucontext_t*) malloc(sizeof(ucontext_t));
			main_thread->ctx = context;
			main_thread->ctx->uc_stack.ss_sp = (char*) malloc(
					sizeof(char) * 4096);
			main_thread->ctx->uc_stack.ss_size = 4096;
			main_thread->state = PS_ACTIVE;
			push_thread(main_thread);
		}
		break;
	case 1: //KLMATCHCORES

		break;
	case 2: //KLMATCHHYPER
		break;
	case 3: //KLALWAYS
		break;
	default:
		break;
	}
	return 0;
}
