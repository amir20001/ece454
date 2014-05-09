#include "ece454rpc_types.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

#define PORT 9930

void launch_server() {
	int sock;
	struct sockaddr_in server;
	char buf[512];
	int rc;
	
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == -1) {
		perror("failed creating socket");
	}
	
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if (bind ( sock, (struct sockaddr *)&server, sizeof(server)) == -1) {
		 perror("bind failed");
	}
	printf("running udp server on %s:%d\n", inet_ntoa(server.sin_addr), ntohs(server.sin_port));
	
	
	for (;;) {
		rc = recv (sock, buf, sizeof(buf), 0);
		if (rc > 0) {
			buf[rc]= 0;
			printf("Received: %s\n", buf);
		}
	}
}

bool register_procedure(const char *procedure_name,
							const int nparams,
							fp_type fnpointer) {
	
	return false;
}