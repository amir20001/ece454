/* 
 * Andrew Svoboda & Amir Benham
 * University of Waterloo
 * Server implementation
 */
#include "simplified_rpc/ece454rpc_types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
int fsMount(const char *srvIpOrDomName, const unsigned int srvPort, const char *localFolderName) {
    struct stat sbuf;

    return(stat(localFolderName, &sbuf));
}

int fsUnmount(const char *localFolderName) {
    return 0;
}

FSDIR* fsOpenDir(const char *folderName) {
    return(opendir(folderName));
}

int fsCloseDir(FSDIR *folder) {
    return(closedir(folder));
}

struct fsDirent *fsReadDir(FSDIR *folder) {
    const int initErrno = errno;
    struct dirent *d = readdir(folder);

    if(d == NULL) {
	if(errno == initErrno) errno = 0;
	return NULL;
    }

    if(d->d_type == DT_DIR) {
	dent.entType = 1;
    }
    else if(d->d_type == DT_REG) {
	dent.entType = 0;
    }
    else {
	dent.entType = -1;
    }

    memcpy(&(dent.entName), &(d->d_name), 256);
    return &dent;
}

int fsOpen(const char *fname, int mode) {
    int flags = -1;

    if(mode == 0) {
	flags = O_RDONLY;
    }
    else if(mode == 1) {
	flags = O_WRONLY | O_CREAT;
    }

    return(open(fname, flags, S_IRWXU));
}

int fsClose(int fd) {
    return(close(fd));
}

int fsRead(int fd, void *buf, const unsigned int count) {
    return(read(fd, buf, (size_t)count));
}

int fsWrite(int fd, const void *buf, const unsigned int count) {
    return(write(fd, buf, (size_t)count)); 
}

int fsRemove(const char *name) {
    return(remove(name));
}
*/

return_type fsRemove(const int nparams, arg_type *a) {
	//wat do
	return_type ret;
	ret.return_val = 0;
	ret.return_size = sizeof(int);
	return ret; 
}

int main(int argc, char *argv[]) {

	if (argc != 2) {
		printf("Expected local directory for fileserver as second parameter\n");
		exit(EXIT_FAILURE);
	}

	//input filesystem in argv[1]
	char *fs = argv[1];

	register_procedure("fsRemove", 1, fsRemove);
	launch_server();
	return 0;
}
