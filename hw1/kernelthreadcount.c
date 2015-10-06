#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int get_no_of_cores() {
	FILE *fp = fopen("/proc/cpuinfo", "r");
	int no_of_core;
	char * pch;
	char tmp[256] = { 0x0 };
	while (fp != NULL && fgets(tmp, sizeof(tmp), fp) != NULL) {
		if (strstr(tmp, "cpu cores")) {
			printf("%s\n", tmp);
			break;
		}

	}
	pch = strtok(tmp, ":");

	while (pch != NULL) {

		if (!strstr(pch, "cpu cores")) {
			no_of_core = atoi(pch);
		}
		pch = strtok(NULL, " ");
	}
	if (fp != NULL)
		fclose(fp);

	return no_of_core;

}
int get_no_of_hyperthreads() {
	int num_cpus = sysconf(_SC_NPROCESSORS_ONLN);
	return (num_cpus / get_no_of_cores());
}
//int main() {
//	FILE *fp = fopen("/proc/cpuinfo", "r");
//	int no_of_core;
//	char * pch;
//	char tmp[256] = { 0x0 };
//	while (fp != NULL && fgets(tmp, sizeof(tmp), fp) != NULL) {
//		if (strstr(tmp, "cpu cores")) {
//			printf("%s\n", tmp);
//			break;
//		}
//
//	}
//	pch = strtok(tmp, ":");
//
//	while (pch != NULL) {
//
//		if (!strstr(pch, "cpu cores")) {
//			no_of_core = atoi(pch);
//		}
//		pch = strtok(NULL, " ");
//	}
//	if (fp != NULL)
//		fclose(fp);
//
//	int num_cpus = sysconf(_SC_NPROCESSORS_ONLN);
//	printf("%d is it %d", num_cpus, no_of_core);
//
//	return 0;
//}
