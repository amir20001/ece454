CC=gcc
CFLAGS=-Wall -g
LDFLAGS=-lm

objects := $(patsubst %.c,%.o,$(wildcard *.c))

server: libstubs.a server.o
	$(CC) $(CFLAGS) server.o -L. -lstubs -o server

client: libstubs.a client.o
	$(CC) $(CFLAGS) client.o -L. -lstubs -o client

$(objects): %.o: %.c ece454rpc_types.h
	$(CC) $(CFLAGS) -c $< -o $@

libstubs.a: server_stub.o client_stub.o mybind.o queue.o
	ar r libstubs.a server_stub.o client_stub.o mybind.o queue.o

.PHONY: clean
clean:
	rm -rf *.out *.o core *.a client server
