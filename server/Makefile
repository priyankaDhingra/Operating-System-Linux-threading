CC=gcc -g 
AR = ar
ARFLAGS = ru
RANLIB = ranlib
CFLAGS= -g
SRCS= client.c server.c
LIBS = -L./SocketLibrary/
LIBS += -L./../pthreads
INC = -I./../pthreads
all:: socketlib client server example_thread threadpool_test threadpool_test_pthread server_io
	#strip client
	#strip server
	strip example_thread
	#strip threadpool_test


socketlib:
	cd SocketLibrary && make

example_thread: example_thread.o
	$(CC) -o example_thread example_thread.o $(LIBS) -lmypthread -lpthread 

client: client.o common.o
	$(CC) -o client client.o common.o $(LIBS) -lsock -lmypthread -lpthread 

server: server.o common.o threadpool.o
	$(CC) -o server server.o common.o threadpool.o $(LIBS) -lsock -lmypthread -lpthread 

server_io: server_io.o common.o threadpool.o
	$(CC) -o server_io server_io.o common.o threadpool.o $(LIBS) -lsock -lmypthread -lpthread 

threadpool_test: threadpool_test.o threadpool.o
	$(CC) -o threadpool_test threadpool_test.o threadpool.o $(LIBS) -lmypthread -lpthread 

threadpool_test_pthread: threadpool_test_pthread.o threadpool_pthread.o
	$(CC) -o threadpool_test_pthread threadpool_test_pthread.o threadpool_pthread.o $(LIBS) -lmypthread -lpthread 
	
client.o: client.c common.h
	$(CC) -o client.o -c client.c $(INC)

server.o: server.c common.h
	$(CC) -o server.o -c server.c $(INC)

common.o: common.c
	$(CC) -o common.o -c common.c $(INC)

example_thread.o: example_thread.c
	$(CC) -o example_thread.o -c example_thread.c $(INC)

threadpool.o: threadpool.c
	$(CC) -o threadpool.o -c threadpool.c

threadpool_pthread.o: threadpool_pthread.c
	$(CC) -o threadpool_pthread.o -c threadpool_pthread.c

threadpool_test.o: threadpool_test.c threadpool.h
	$(CC) -o threadpool_test.o -c threadpool_test.c

threadpool_test_pthread.o: threadpool_test_pthread.c threadpool.h
	$(CC) -o threadpool_test_pthread.o -c threadpool_test_pthread.c
	
clean:
	/bin/rm -f client server example_thread threadpool_test *.o core *~ #*
	cd SocketLibrary && make clean
