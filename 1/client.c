#include <stdio.h>
#include <string.h>
#include "ece454rpc_types.h"

int main() {
	int a = -10, b = 20;
        
	return_type ans = make_remote_call("localhost",
                                            10000,
                                            "addtwo", 2,
                                            sizeof(int), (void *)(&a),
                                            sizeof(int), (void *)(&b));
	int i = *(int *)(ans.return_val);
	printf("client, got result: %d\n", i);
        
        /*
        char * c = "asd";
        char * d = "fgh";
        return_type dd = make_remote_call("localhost",
                                            10000,
                                            "concat", 2,
                                            strlen(c),(c),
                                            strlen(d),(d));
	char * e = (char *)(dd.return_val);
	printf("client, got result: %s\n", e);
         */
	return 0;
}
