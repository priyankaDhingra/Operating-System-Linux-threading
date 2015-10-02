#include <ucontext.h>
#include <pthread.h>
#define USEMYPTHREADS 1
/* 0 = use standard pthreads , 1 = use my threads package */
#if USE_MY_PTHREADS
#define PTHREAD_CREATE my_pthread_create 
//#define PTHREAD_EXIT my_pthread_exit
#define MYPTHREAD_INIT mypthread_init
#else
#define PTHREAD_CREATE pthread_create 
//#define PTHREAD_EXIT pthread_exit
#define PTHREAD_INIT pthread_init
#endif

enum mypthread_state {
	PS_ACTIVE,
	PS_BLOCKED,
	PS_DEAD
};
struct tcb_thread{
	int thread_id;
	enum mypthread_state cur_state;
	ucontext_t* ctx;//has the stack space
};
// Types
 typedef struct {
 	int th_id;

 	enum mypthread_state state;
 	//char thread_stack[8192];
 	int joinfrom_th;
} mypthread_t;
//
