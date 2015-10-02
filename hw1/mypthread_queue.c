#include <stdio.h>
#include "mypthread.h"

struct thread_node {
	struct tcb_thread *tcb;
	struct thread_node *next;
};
struct thread_node *front;
struct thread_node *rear;
void push(struct tcb_thread *tcb) {
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

struct tcb_thread* pop() {
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

void traverse() {
	struct thread_node *temp;
	temp = front;
	while (temp->next != front) {
		printf("element--%d\n", (int) temp->tcb->thread_id);
		temp = temp->next;
	}
}
int main() {
	struct tcb_thread *tcb = malloc(sizeof(struct tcb_thread));
	tcb->thread_id = 1;
	tcb->cur_state = 1;
	push(tcb);

	struct tcb_thread *tcb1 = malloc(sizeof(struct tcb_thread));
	tcb1->thread_id = 2;
	tcb1->cur_state = 1;
	push(tcb1);

	struct tcb_thread *tcb3 = malloc(sizeof(struct tcb_thread));
	tcb3->thread_id = 3;
	tcb3->cur_state = 1;
	push(tcb3);

	struct tcb_thread *tcb2 = malloc(sizeof(struct tcb_thread));
	tcb2->thread_id = 4;
	tcb2->cur_state = 1;
	push(tcb2);

	struct tcb_thread *tcb4 = pop();
	printf("popped--%d\n", (int) tcb4->thread_id);

	traverse();
	return 0;

}
