#include <stdio.h>
#include <string.h>
#include "ece454rpc_types.h"

int main() {
	int a = 10, b = 20;
        
	return_type ans = make_remote_call("localhost",
                                            10007,
                                            "addtwo", 2,
                                            sizeof(int), (void *)(&a),
                                            sizeof(int), (void *)(&b));
	int i = *(int *)(ans.return_val);
	printf("client, got result: %d\n", i);

	return_type aaa = make_remote_call("localhost",
                                            10007,
                                            "nop", 0);
	i = *(int *)(aaa.return_val);
	printf("client, got result: %d\n", i);
        
        char *c = "Hello work!";
        char *d = "This sentence has 6 words";
        return_type dd = make_remote_call("localhost",
                                            10007,
                                            "concat", 2,
                                            strlen(c)+1,((void*)c),
                                            strlen(d)+1,((void*)d));
	char * e = (char *)(dd.return_val);
	printf("client, got result: %s\n", e);
	return 0;
}
