/*
 * Sample program which tests
 * mypthread_create()
 * mypthread_exit()
 */
#include "mypthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
int num_threads; //	100

void *PrintHello(void *threadid) {
	long tid;
	tid = (long) threadid;
	FILE *fp;
	char buff[255];
/*
	fp = fopen("/tmp/test.txt", "rw");
	fscanf(fp, "%s", buff);
	//printf("1 : %s\n", buff);

	fgets(buff, 255, (FILE*) fp);
	//printf("2: %s\n", buff);

	fgets(buff, 255, (FILE*) fp);
	//printf("3: %s\n", buff);
	fclose(fp);
	*/
	printf("Hello World! It's me, thread #%ld!\n", tid);
	mypthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("usage: %s mypthread_policy(0|1|2|3) max_num_threads\n",
				argv[0]);
		exit(0);
	}
	num_threads = atoi(argv[2]);
	mypthread_init(atoi(argv[1]));
	mypthread_t threads[num_threads];
	int rc;
	long t;
	struct timeval then, now, diff;

	double dif;  //elapsed time in seconds between then and now
	gettimeofday(&now, NULL);

	for (t = 0; t < num_threads; t++) {
		//printf("In main: creating thread %ld\n", t);
		rc = mypthread_create(&threads[t], NULL, PrintHello, (void *) t);
		if (rc) {
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	for (t = 0; t < num_threads; t++) {
		//printf("In main: joining thread: %ld\n", t);
		mypthread_join(threads[t], NULL);
	}
	timersub(&now, &then, &diff);
	dif = 1000000 * diff.tv_sec + diff.tv_usec;
	printf("Your calculations took %.2lf milli to run.\n", dif / 1000);
	mypthread_exit(NULL);

	//exit(0);
}
