CC=gcc
CFLAGS=-g

default: help

all: mypthread_lib library

help:
	@echo "make library - To generate mypthread library"
	@echo "make mypthread_lib - Build mypthread library to generate .o files"
	@echo "make mypthread_sample1 - Sample program to test create and exit threads"
	@echo "make mypthread_sample2 - Sample program to test create, join and exit threads"
	@echo "make mypthread_sample3 - Sample program to test create, join, lock, unlock and exit threads"
	@echo "make mypthread_sample4 - Sample program to test create, join, lock, unlock and exit threads"
	@echo "make mypthread_sample5 - Sample program to test create, join, lock, unlock, cond_signal, cond_wait and exit threads"
	@echo "make mtsort - Example program to sort list given in CS518"
	@echo "make ypthread_performance - Program used for testing performance"
	

	
mypthread_lib: mypthread.c mypthread.h queue_util.c queue_util.h utilities.c utilities.h
	$(CC) -Wall -c $(CFLAGS) mypthread.c queue_util.c utilities.c
	
mypthread_sample1: mypthread_sample1.c mypthread.c mypthread.h queue_util.c queue_util.h utilities.c utilities.h
	$(CC) $(CFLAGS) mypthread_sample1.c mypthread.c queue_util.c utilities.c -lrt -o mypthread_sample1

mypthread_sample2: mypthread_sample2.c mypthread.c mypthread.h queue_util.c queue_util.h utilities.c utilities.h
	$(CC) $(CFLAGS) mypthread_sample2.c mypthread.c queue_util.c utilities.c -lrt -o mypthread_sample2

mypthread_sample3: mypthread_sample3.c mypthread.c mypthread.h queue_util.c queue_util.h utilities.c utilities.h
	$(CC) $(CFLAGS) mypthread_sample3.c mypthread.c queue_util.c utilities.c -lrt -o mypthread_sample3

mypthread_sample4: mypthread_sample4.c mypthread.c mypthread.h queue_util.c queue_util.h utilities.c utilities.h
	$(CC) $(CFLAGS) mypthread_sample4.c mypthread.c queue_util.c utilities.c -lrt -o mypthread_sample4

mypthread_sample5: mypthread_sample5.c mypthread.c mypthread.h queue_util.c queue_util.h utilities.c utilities.h
	$(CC) $(CFLAGS) mypthread_sample5.c mypthread.c queue_util.c utilities.c -lrt -o mypthread_sample5

mtsort: mtsort.c mypthread.c mypthread.h queue_util.c queue_util.h utilities.c utilities.h
	$(CC) $(CFLAGS) mtsort.c mypthread.c queue_util.c utilities.c -lrt -o mtsort
	
mypthread_performance: mypthread_performance.c mypthread.c mypthread.h queue_util.c queue_util.h utilities.c utilities.h
	$(CC) $(CFLAGS) mypthread_performance.c mypthread.c queue_util.c utilities.c -lrt -o mypthread_performance

library: mypthread.c mypthread.h  queue_util.c queue_util.h utilities.c utilities.h
	$(CC) -c mypthread.c mypthread.h queue_util.c utilities.c -I./
	ar rcs libmypthread.a mypthread.o utilities.o queue_util.o