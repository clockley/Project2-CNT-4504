CC=gcc
CFLAGS=-std=gnu11 -lm -lpthread -flto -O2
DEPS = common.h

all: client server

client: client.o
	$(CC) -o client client.c $(DEPS) $(CFLAGS)

server: server.o
	$(CC) -o server server.c $(DEPS) $(CFLAGS)

clean:
	rm *.o
	rm *~
	rm client
	rm server
