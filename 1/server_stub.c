#include "ece454rpc_types.h"
#include <sys/types.h> 
#include <sys/socket.h>
#include <stdlib.h>

void launch_server() {
	int socket;
	struct sockaddr_in server;
	socklen_t len = sizeof(server);
	int port_number;
	
	socket = socket(AF_NET, SOCKET_DGRAM, 0);
	if (socket == 0) {
		perror("Socket init fails");
	}
	
	if(getsockname(socket, (struct sockaddr *)&server, &len) == 0) {
		port_number = ntohs(server.sin_port);
		printf("running on port: %d", port_number);
	}
}

bool register_procedure(const char *procedure_name,
							const int nparams,
							fp_type fnpointer) {
	
}