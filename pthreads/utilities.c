#include<stdio.h>
#include <stdlib.h>
#include "utilities.h"
#include <linux/unistd.h>
#include <sys/syscall.h>
#include <unistd.h>

/*
 * Function that gets the number of CPU cores
 */
int get_cpu_cores() {
	FILE *fp;
	int status;
	char resultString[2]; //Assuming the result string size of max 2

	fp = popen("lscpu | grep 'Core(s) per socket:' | cut -d' ' -f7", "r");
	if (fp == NULL) {
		return 2; //Assume if error return default 2 cores
	}
	fgets(resultString, 2, fp);
	status = pclose(fp);
	return atoi(resultString);
}

/*
 * Function that Gets number of threads per core. i.e. hyper threads
 */
int get_hyperthreads() {
	FILE *fp;
	int status;
	char resultString[2]; //Assuming the result string size of max 2

	fp = popen("lscpu | grep 'Thread(s) per core' | cut -d' ' -f7", "r");
	if (fp == NULL) {
		return 1; //Assume if error return default 1 hyperthread
	}
	fgets(resultString, 2, fp);
	status = pclose(fp);
	return atoi(resultString);
}
/*
 * This function gets the kthread id
 */
pid_t gettid(void) {
	return syscall(__NR_gettid);
}
