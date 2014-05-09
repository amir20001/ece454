
CC=gcc
CFLAGS=-Wall -g
LDFLAGS=-lm

objects := $(patsubst %.c,%.o,$(wildcard *.c))

a.out: libstubs.a client.o server.o
	$(CC) $(CFLAGS) client.o server.o -L. -lstubs -o a.out

server: libstubs.a server.o
	$(CC) $(CFLAGS) server.o -L. -lstubs -o server

client: libstubs.a client.o
	$(CC) $(CFLAGS) client.o -L. -lstubs -o client

$(objects): %.o: %.c ece454rpc_types.h
	$(CC) $(CFLAGS) -c $< -o $@

libstubs.a: server_stub.o client_stub.o
	ar r libstubs.a server_stub.o client_stub.o

.PHONY: clean
clean:
	rm -rf *.out *.o core *.a
