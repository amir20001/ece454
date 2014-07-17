CC = gcc
OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))
CFLAGS = -Wall -Wextra -Wmissing-prototypes -Wmissing-declarations -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Wredundant-decls -Wnested-externs -Winline -Wno-long-long -Wconversion -Wstrict-prototypes 
RPC = ./simplified_rpc/libstubs.a

fs_client: fs_client.o fs_dummy.o

libstubs: 
	cd simplified_rpc && make libstubs.a

client-api.a: libstubs fs_client_api.o
	ar r lib_client.a fs_client_api.o
	ar -M < client_api.mri

server:	libstubs
	$(CC) $(CFLAGS) -g server.c $(RPC) -o fsServer

client: libstubs client-api.a
	$(CC) $(CFLAGS) -g fs_client.c -L. -lclient-api -o fsClient

$(OBJECTS): %.o: %.c ece454_fs.h
	$(CC) $(CFLAGS) -c -g $< -o $@

clean:
	rm -rf a.out *.o core *.a fs_client fsServer fsClient
	cd simplified_rpc && make clean
