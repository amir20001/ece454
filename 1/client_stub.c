#include "ece454rpc_types.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>


return_type make_remote_call(const char *servernameorip,
								const int serverportnumber,
								const char *procedure_name,
								const int nparams,
					...) {
	return_type ret;
	
	int sock;
	struct sockaddr_in server;

	sock = socket (AF_INET,SOCK_DGRAM,0);
	if (sock == -1) {
		perror("failed creating socket");
	}
	
	inet_pton(AF_INET, "127.0.0.1", &(server.sin_addr));
	server.sin_family = AF_INET;
	server.sin_port = htons(serverportnumber);
	
	sendto(sock, "HI", 2, 0, (struct sockaddr *) &server, sizeof(server));
	
	
	//close(sock);
	return ret;
}