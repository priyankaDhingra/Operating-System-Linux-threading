#define USEMYPTHREADS 0
/*
 * #include <pthread.h>
 * */

#include "mypthread.h"
#if USEMYPTHREADS

#define pthread_create  mypthread_create
#define pthread_exit            mypthread_exit
#define pthread_yield           mypthread_yield
#define pthread_join            mypthread_join

#define pthread_mutex_init      mypthread_mutex_init
#define pthread_mutex_lock      mypthread_mutex_lock
#define pthread_mutex_trylock   mypthread_mutex_trylock
#define pthread_mutex_unlock    mypthread_mutex_unlock
#define pthread_mutex_destroy   mypthread_mutex_destroy

#define pthread_t               mypthread_t
#define pthread_attr_t  mypthread_attr_t

#define pthread_mutex_t mypthread_mutex_t
#define pthread_mutex_attr_t    mypthread_mutex_attr_t
#define pthread_cond_init  mypthread_cond_init
#define pthread_cond_broadcast            mypthread_cond_broadcast
#define pthread_cond_signal           mypthread_cond_signal
#define pthread_cond_wait            mypthread_cond_wait

#define pthread_cond_timedwait      mypthread_cond_timedwait
#define pthread_cond_destroy      mypthread_cond_destroy

#else
#define pthread_create  pthread_create
#define pthread_exit            pthread_exit
#define pthread_yield           pthread_yield
#define pthread_join            pthread_join

#define pthread_mutex_init      pthread_mutex_init
#define pthread_mutex_lock      pthread_mutex_lock
#define pthread_mutex_trylock   pthread_mutex_trylock
#define pthread_mutex_unlock    pthread_mutex_unlock
#define pthread_mutex_destroy   pthread_mutex_destroy
#define mypthread_init		    if(1);
#define pthread_t               pthread_t
#define pthread_attr_t  pthread_attr_t

#define pthread_mutex_t pthread_mutex_t
#define pthread_mutex_attr_t    pthread_mutex_attr_t
#define pthread_cond_init  pthread_cond_init
#define pthread_cond_broadcast            pthread_cond_broadcast
#define pthread_cond_signal           pthread_cond_signal
#define pthread_cond_wait            pthread_cond_wait

#define pthread_cond_timedwait      pthread_cond_timedwait
#define pthread_cond_destroy      pthread_cond_destroy
#endif
