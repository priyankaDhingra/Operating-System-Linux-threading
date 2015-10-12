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
#define NUM_LOOPS 500000
#include "SocketLibrary/socklib.h"
#include "common.h"
#include "threadpool.h"
extern int errno;
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
	int socket_talk =(int) socket_talk;

#ifdef DEBUG
    fprintf(stderr, "(SERVER) request_client %d\n", socket_talk);
#endif
	request = read_request(socket_talk);  // step 2
	if (request != NULL) {
		int response_length;

		response = process_request(request, &response_length);  // step 3
		if (response != NULL) {
			send_response(socket_talk, response, response_length);  // step 4
		}
	}
#ifdef DEBUG
    fprintf(stderr, "(SERVER) %d\n", response);
#endif
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
	if (argc != 2) {
		fprintf(stderr, "(SERVER): Invoke as  './server socknum'\n");
		fprintf(stderr, "(SERVER): for example, './server 4434'\n");
		exit(-1);
	}

	/*
	 * Set up the 'listening socket'.  This establishes a network
	 * IP_address:port_number that other programs can connect with.
	 */
	pool = create_threadpool(10,NULL);
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

	while (1) {

		socket_talk = saccept(socket_listen);  // step 1
		if (socket_talk < 0) {
			fprintf(stderr, "An error occured in the server; a connection\n");
			fprintf(stderr, "failed because of ");
			perror("");
			exit(1);
		}

		dispatch(pool, request_client, (void *) socket_talk);

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

	   FILE *fp;
	   char buff[255];

	   fp = fopen("/tmp/test.txt", "rw");
	   fscanf(fp, "%s", buff);
	   printf("1 : %s\n", buff );

	   fgets(buff, 255, (FILE*)fp);
	   printf("2: %s\n", buff );

	   fgets(buff, 255, (FILE*)fp);
	   printf("3: %s\n", buff );
	   fclose(fp);

	return buff;
}


