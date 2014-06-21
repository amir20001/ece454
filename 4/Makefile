OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))

fs_client: fs_client.o fs_dummy.o

libstubs: 
	cd simplified_rpc && make libstubs.a

server:	libstubs
	gcc -g -Wall server.c ./simplified_rpc/libstubs.a -o fsServer

client_api.a: libstubs fs_client_api.o
	ar r lib_client.a fs_client_api.o
	ar -M < client_api.mri

$(OBJECTS): %.o: %.c ece454_fs.h
	gcc -c $< -o $@

clean:
	rm -rf a.out *.o core *.a fs_client fsServer client_api.a
	cd simplified_rpc && make clean
