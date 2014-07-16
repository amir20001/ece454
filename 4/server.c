/* 
 * Andrew Svoboda & Amir Benham
 * University of Waterloo
 * Server implementation
 */
#include "simplified_rpc/ece454rpc_types.h"
#include "fsOtherIncludes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

return_type ret;

return_type fsMount(const int nparams, arg_type *a);
return_type fsRemove(const int nparams, arg_type *a);
return_type fsClose(const int nparams, arg_type *a);
return_type fsOpen(const int nparams, arg_type *a);

return_type fsOpenDir(const int nparams, arg_type *a);
return_type fsReadDir(const int nparams, arg_type *a);

return_type fsMount(const int nparams, arg_type *a) {
    /*
    struct stat sbuf;

    return(stat(localFolderName, &sbuf));
    */
	ret.return_val = NULL;
	ret.return_size = 0;
	return ret; 
}

/*
int fsUnmount(const char *localFolderName) {


int fsCloseDir(FSDIR *folder) {

struct fsDirent *fsReadDir(FSDIR *folder) {

int fsOpen(const char *fname, int mode) {

int fsRead(int fd, void *buf, const unsigned int count) {

int fsWrite(int fd, const void *buf, const unsigned int count) {

int fsRemove(const char *name) {
*/

return_type fsOpenDir(const int nparams, arg_type *a) {
    if (nparams != 1) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }
    char *path = a->arg_val;
    DIR* dir = opendir(path);
    printf("path: %s\n", path);

    if (dir == NULL) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }
    printf("thin: \n");

    //TODO: ehh..
    ret.return_val = (void*)dir;
    ret.return_size = sizeof(dir);
    return ret;
}

return_type fsReadDir(const int nparams, arg_type *a) {
    if (nparams != 1) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }

    FSDIR* dir = (FSDIR*)a->arg_val;
    struct dirent *ent;

    ent = readdir(dir);

    if (ent == NULL) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }

    //TODO: is this okay?
    ret.return_val = (void*)ent;
    ret.return_size = sizeof(&ent);
    return ret;
}

return_type fsRemove(const int nparams, arg_type *a) {
	//wat do
	ret.return_val = 0;
	ret.return_size = sizeof(int);
	return ret; 
}

return_type fsOpen(const int nparams, arg_type *a) {
    if (nparams != 2) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }

    if(a->next->arg_size != sizeof(int)) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }

    char *fname = (char*)(a->arg_val);
    int flags = -1;
    int mode = *(int*)(a->next->arg_val);
    if (mode == 0) {
        flags = O_RDONLY;
    } else {
        flags = O_WRONLY | O_CREAT;
    }
    int *r = (int*)malloc(sizeof(int));
    *r = open(fname, flags, S_IRWXU);
    ret.return_val = (void*)r;
    ret.return_size = sizeof(int);
    return ret;
}

return_type fsClose(const int nparams, arg_type *a) {
    if (nparams != 1) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }

    if(a->arg_size != sizeof(int)) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }

    int fd = *(int*)(a->arg_val);
    int *r = (int*)malloc(sizeof(int));
    *r = close(fd);

    ret.return_val = (void*)(r);
    ret.return_size = sizeof(int);

    //TODO
    //is there a lock on this file? wat do

    return ret;
}

int main(int argc, char *argv[]) {

	if (argc != 2) {
		printf("Expected local directory for fileserver as second parameter\n");
		exit(EXIT_FAILURE);
	}

	//input filesystem in argv[1]
	char *fs = argv[1];

    struct stat s;
    int err = stat(fs, &s);
    if (err == -1) {
        perror("stat"); exit(1);
    } else {
        if (S_ISDIR(s.st_mode)) {
            //is a directory... wat do?
        } else {
            perror("exists but is not dir"); exit(1);
        }
    }
	register_procedure("fsMount", 1, fsMount);

	register_procedure("fsOpenDir", 1, fsOpenDir);
    //register_procedure("fsReadDir", 1, fsReadDir);
    //register_procedure("fsCloseDir", 1, fsCloseDir);

	register_procedure("fsRemove", 1, fsRemove);
	register_procedure("fsClose", 1, fsClose);
	register_procedure("fsOpen", 2, fsOpen);
	launch_server();
	return 0;
}
