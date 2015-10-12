/**
 * server.c, copyright 2001 Steve Gribble
 *
 * The server is a single-threaded program.  First, it opens
 * up a "listening socket" so that clients can connect to
 * it.  Then, it enters a tight loop; in each iteration, it
 * accepts a new connection from the client, reads a request,
 * computes for a while, sends a response, then closes the
 * connection.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <sys/time.h>
#define DEBUG 1
#include "SocketLibrary/socklib.h"
#include "common.h"
#include "threadpool.h"

#define NUM_LOOPS 500000
#define POOL_SIZE 20

extern int errno;
int count_dispatches = 0;
int setup_listen(char *socketNumber);
char *read_request(int fd);
char *process_request(char *request, int *response_length);
void send_response(int fd, char *response, int response_length);

/**
 * This program should be invoked as "./server <socketnumber>", for
 * example, "./server 4342".
 */
void request_client(void *stalk) {
	char *request = NULL;
	char *response = NULL;
	int socket_talk = (int) socket_talk;

	request = read_request(socket_talk);  // step 2
	if (request != NULL) {
		int response_length;

		response = process_request(request, &response_length);  // step 3
		if (response != NULL) {
			send_response(socket_talk, response, response_length);  // step 4
		}
	}
	close(socket_talk);  // step 5

	// clean up allocated memory, if any
	if (request != NULL)
		free(request);
	if (response != NULL)
		free(response);
}

int main(int argc, char **argv) {
	char buf[1000];
	int socket_listen;
	int socket_talk;
	int dummy, len;
	threadpool pool;
	struct timeval then, now, diff;
	if (argc != 2) {
		fprintf(stderr, "(SERVER): Invoke as  './server socknum'\n");
		fprintf(stderr, "(SERVER): for example, './server 4434'\n");
		exit(-1);
	}

	/*
	 * Set up the 'listening socket'.  This establishes a network
	 * IP_address:port_number that other programs can connect with.
	 */
	pool = create_threadpool(POOL_SIZE,NULL);
	socket_listen = setup_listen(argv[1]);

	/*
	 * Here's the main loop of our program.  Inside the loop, the
	 * one thread in the server performs the following steps:
	 *
	 *  1) Wait on the socket for a new connection to arrive.  This
	 *     is done using the "accept" library call.  The return value
	 *     of "accept" is a file descriptor for a new data socket associated
	 *     with the new connection.  The 'listening socket' still exists,
	 *     so more connections can be made to it later.
	 *
	 *  2) Read a request off of the listening socket.  Requests
	 *     are, by definition, REQUEST_SIZE bytes long.
	 *
	 *  3) Process the request.
	 *
	 *  4) Write a response back to the client.
	 *
	 *  5) Close the data socket associated with the connection
	 */
	gettimeofday(&then, NULL);
	while (1) {

		socket_talk = saccept(socket_listen);  // step 1
		if (socket_talk < 0) {
			fprintf(stderr, "An error occured in the server; a connection\n");
			fprintf(stderr, "failed because of ");
			perror("");
			exit(1);
		}

		dispatch(pool, request_client, (void *) socket_talk);
		count_dispatches++;
		if (count_dispatches == 10) {
			double seconds;  //elapsed time in seconds between then and now
			gettimeofday(&now, NULL);
			timersub(&now, &then, &diff);
			seconds = 1000000*diff.tv_sec + diff.tv_usec;

			fprintf(stdout, "Loops: %6d  Threads: %2d Dispatches/sec: %10.3f\n",
					NUM_LOOPS, POOL_SIZE, seconds/1000);
		}

	}
}

/**
 * This function accepts a string of the form "5654", and opens up
 * a listening socket on the port associated with that string.  In
 * case of error, this function simply bonks out.
 */

int setup_listen(char *socketNumber) {
	int socket_listen;

	if ((socket_listen = slisten(socketNumber)) < 0) {
		perror("(SERVER): slisten");
		exit(1);
	}

	return socket_listen;
}

/**
 * This function reads a request off of the given socket.
 * This function is thread-safe.
 */

char *read_request(int fd) {
	char *request = (char *) malloc(REQUEST_SIZE * sizeof(char));
	int ret;

	if (request == NULL) {
		fprintf(stderr, "(SERVER): out of memory!\n");
		exit(-1);
	}

	ret = correct_read(fd, request, REQUEST_SIZE);
	if (ret != REQUEST_SIZE) {
		free(request);
		request = NULL;
	}
	return request;
}

/**
 * This function crunches on a request, returning a response.
 * This is where all of the hard work happens.
 * This function is thread-safe.
 */



char *process_request(char *request, int *response_length) {
	char *response = (char *) malloc(RESPONSE_SIZE * sizeof(char));
	int i, j;

	// just do some mindless character munging here

//	FILE *fp;
//
//	fp = fopen("/tmp/test.txt", "w+");
//	fprintf(fp, "This is testing for fprintf...\n");
//	fputs("This is testing for fputs...\n", fp);
//	fclose(fp);

	for (i = 0; i < RESPONSE_SIZE; i++)
		response[i] = request[i % REQUEST_SIZE];

	for (j = 0; j < NUM_LOOPS; j++) {
		for (i = 0; i < RESPONSE_SIZE; i++) {
			char swap;

			swap = response[((i + 1) % RESPONSE_SIZE)];
			response[((i + 1) % RESPONSE_SIZE)] = response[i];
			response[i] = swap;
		}
	}
	*response_length = RESPONSE_SIZE;

	return response;
}

