#include "ece454rpc_types.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 1024

return_type make_remote_call(const char *servernameorip,
								const int serverportnumber,
								const char *procedure_name,
								const int nparams,
					...) {
	return_type ret;
	
	int sock;
	struct sockaddr_in server;
	char buf[BUF_SIZE];

	sock = socket (AF_INET,SOCK_DGRAM,0);
	if (sock == -1) {
		perror("failed creating socket");
	}
	
	inet_pton(AF_INET, "127.0.0.1", &(server.sin_addr));
	server.sin_family = AF_INET;
	server.sin_port = htons(serverportnumber);
	
	char * func = "fds";
	
	sendto(sock, func, strlen(func), 0, (struct sockaddr *) &server, sizeof(server));
	int nread;
	
	nread = recv (sock, buf, BUF_SIZE, 0);
	buf[nread] = 0;
	printf("Received: %s\n", buf);
	
	close(sock);
	return ret;
}