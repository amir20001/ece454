objects := $(patsubst %.c,%.o,$(wildcard *.c))

a.out: libstubs.a client.o server.o
	gcc client.o server.o -L. -lstubs -o a.out

$(objects): %.o: %.c ece454rpc_types.h
	gcc -c $< -o $@

libstubs.a: server_stub.o client_stub.o
	ar r libstubs.a server_stub.o client_stub.o

.PHONY: clean
clean:
	rm -rf *.out *.o core *.a
