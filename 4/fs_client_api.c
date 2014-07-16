/* 
 * Andrew Svoboda & Amir Benham
 * University of Waterloo
 * Client api implementation of Assignment 4 procedures 
 */
#include "ece454_fs.h"
#include "simplified_rpc/ece454rpc_types.h"

#include <errno.h>
#include <string.h>

struct fsDirent dent;

char *srvIp = NULL;
int port = 0;

int fsMount(const char *srvIpOrDomName, const unsigned int srvPort, const char *localFolderName) {

    return_type ret;
    ret = make_remote_call(srvIpOrDomName, srvPort, 
				"fsMount", 1,
				strlen(localFolderName)+1, (void*)(localFolderName)); 

    if (ret.return_val != 0) {
        errno = *(int*)ret.return_val;
        return -1;
    }

    srvIp = srvIpOrDomName;
    port = srvPort;

    return 0;
}

int fsUnmount(const char *localFolderName) {
    return_type ret;
    ret = make_remote_call(srvIp, port, 
				"fsUnmount", 1,
				strlen(localFolderName)+1, (void*)(localFolderName)); 

    if (ret.return_val != 0) {
        errno = *(int*)ret.return_val;
        return -1;
    }
    return 0;
}

FSDIR* fsOpenDir(const char *folderName) {
    return_type ret;
    ret = make_remote_call(srvIp, port, 
				"fsOpenDir", 1,
				strlen(folderName)+1, (void*)(folderName)); 

    if (ret.return_val == 0) {
        //errno = *(int*)ret.return_val;
        return NULL;
    }
    //TODO: is this correct?
    return (FSDIR*)ret.return_val;
}

int fsCloseDir(FSDIR *folder) {
    return_type ret;
    ret = make_remote_call(srvIp, port, 
				"fsCloseDir", 1,
				sizeof(folder), (void*)(folder)); 

    if (ret.return_val != 0) {
        errno = *(int*)ret.return_val;
        return -1;
    }
    return 0;
}

struct fsDirent *fsReadDir(FSDIR *folder) {
    /*
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
    */

    return_type ret;
    ret = make_remote_call(srvIp, port, 
				"fsReadDir", 1,
				sizeof(folder), (void*)(folder)); 

    if (ret.return_val == 0) {
        errno = -1; //TODO: fix 
        return NULL;
    }
    struct dirent *d;

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
    return_type ret;
    ret = make_remote_call(srvIp, port, 
				"fsOpen", 2,
				strlen(fname)+1, (void*)fname,
                sizeof(int), (void*)(&mode)); 
            
    int fd = *(int*)(ret.return_val); 
    if (fd == -1) {
        errno = 1; //TODO
        return -1;
    }
    return fd;
}

int fsClose(int fd) {
    return_type ret;
    ret = make_remote_call(srvIp, port, 
				"fsClose", 1,
				sizeof(int), (void*)(&fd)); 

    if (ret.return_val != 0) {
        errno = *(int*)ret.return_val;
        return -1;
    }
    return 0;
}

int fsRead(int fd, void *buf, const unsigned int count) {
    return(read(fd, buf, (size_t)count));
}

int fsWrite(int fd, const void *buf, const unsigned int count) {
    return(write(fd, buf, (size_t)count)); 
}

int fsRemove(const char *name) {
    return_type ret;
    ret = make_remote_call(srvIp, port, 
				"fsRemove", 1,
				strlen(name)+1, (void*)(name)); 

    if (ret.return_val != 0) {
        errno = *(int*)ret.return_val;
        return -1;
    }
    return 0;
}
