#include "ece454rpc_types.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 9930


void launch_server() {
	int sock;
	struct sockaddr_in server, remaddr; //our address, remote address
	socklen_t addrlen = sizeof(remaddr);
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
		//rc = recv (sock, buf, sizeof(buf), 0);
		rc = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr *)&remaddr, &addrlen);
		if (rc > 0) {
			buf[rc]= 0;
			printf("Received: %s\n", buf);
			
			sendto(sock, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, addrlen);
			
			//sendto(sock, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, addrlen);
		}
	}
}

bool register_procedure(const char *procedure_name,
							const int nparams,
							fp_type fnpointer) {
	
	if (procedure_queue == NULL) {
		printf("init que\n");
		procedure_queue = (queue*)malloc(sizeof(queue));
		init(procedure_queue);
	}
	printf("registering procedure %s\n", procedure_name);
	enqueue(procedure_queue, procedure_name, nparams, fnpointer);
	
	return true;
}