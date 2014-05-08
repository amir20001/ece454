#include "ece454rpc_types.h"
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>

void launch_server() {
	int sock;
	struct sockaddr_in sin;
	socklen_t len = sizeof(sin);

	int port_number;
	
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == 0) {
		perror("Socket init fails");
	}
	
	if(getsockname(socket, (struct sockaddr *)&sin, &len) == 0) {
		port_number = ntohs(sin.sin_port);
		printf("running on port: %d", port_number);
	}
}

bool register_procedure(const char *procedure_name,
							const int nparams,
							fp_type fnpointer) {
	
}