#include <stdio.h>
#include <string.h>
#include "ece454rpc_types.h"

int ret_int;
return_type r;

return_type nop(const int nparams, arg_type* a) {
	if(nparams != 0) {
		/* Error! */
		r.return_val = NULL;
		r.return_size = 0;
		return r;
	}

	ret_int = 189998;
	r.return_val = (void *)(&ret_int);
	r.return_size = sizeof(int);

	return r;
}

return_type add(const int nparams, arg_type* a) {
	if(nparams != 2) {
		/* Error! */
		r.return_val = NULL;
		r.return_size = 0;
		return r;
	}

	if(a->arg_size != sizeof(int) ||
		a->next->arg_size != sizeof(int)) {
		/* Error! */
		r.return_val = NULL;
		r.return_size = 0;
		return r;
	}

	int i = *(int *)(a->arg_val);
	int j = *(int *)(a->next->arg_val);

	ret_int = i+j;
	r.return_val = (void *)(&ret_int);
	r.return_size = sizeof(int);

	return r;
}

return_type concat(const int nparams, arg_type* a) {
	if(nparams != 2) {
		/* Error! */
		r.return_val = NULL;
		r.return_size = 0;
		return r;
	}

	char* i = (char *)(a->arg_val);
	char* j = (char *)(a->next->arg_val);

	char *ret = strcat(i, j);
	r.return_val = (void *)(ret);
	r.return_size = strlen(ret);

	return r;
}

int main() {
    register_procedure("addtwo", 2, add);
    register_procedure("concat", 2, concat);
    register_procedure("nop", 0, nop);

    launch_server();

    /* should never get here, because
       launch_server(); runs forever. */

    return 0;
}
