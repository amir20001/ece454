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
char *mountedDir = NULL;

return_type fsMount(const int nparams, arg_type *a);
return_type fsUnmount(const int nparams, arg_type *a);

return_type fsOpen(const int nparams, arg_type *a);
return_type fsRead(const int nparams, arg_type *a);
return_type fsWrite(const int nparams, arg_type *a);
return_type fsClose(const int nparams, arg_type *a);
return_type fsRemove(const int nparams, arg_type *a);

return_type fsOpenDir(const int nparams, arg_type *a);
return_type fsReadDir(const int nparams, arg_type *a);
return_type fsCloseDir(const int nparams, arg_type *a);

return_type fsMount(const int nparams, arg_type *a) {
    if (nparams != 1) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }
    ret.return_val = malloc(strlen(mountedDir)+1);
    memcpy(ret.return_val, mountedDir, strlen(mountedDir)+1);
	ret.return_size = strlen(mountedDir)+1;
	return ret; 
}

return_type fsUnmount(const int nparams, arg_type *a) {
    if (nparams != 1) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }
    ret.return_val = NULL;
	ret.return_size = 0;
	return ret; 
}

return_type fsOpenDir(const int nparams, arg_type *a) {
    if (nparams != 1) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }
    char *path = a->arg_val;
    FSDIR* dir = (FSDIR*)opendir(path);

    if (dir == NULL) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }
    //the size of the DIR struct is opaque to us
    // this is likely the least offensive answer
    // IF IT WORKS. FUCK
    int s = (sizeof(int) + (3*sizeof(size_t)) + sizeof(off_t) + sizeof(struct dirent));
    printf("size of DIR (__dirstream) %d\n", s);
    ret.return_val = (FSDIR*)malloc((size_t)s);
    memcpy(ret.return_val, dir, (size_t)s);

    //ret.return_size = sizeof(DIR);
    ret.return_size = s;
    return ret;
}

return_type fsReadDir(const int nparams, arg_type *a) {
    if (nparams != 1) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }

    printf("starting readdir\n");
    FSDIR* dir = (FSDIR*)malloc((size_t)a->arg_size);
    memcpy(dir, a->arg_val, (size_t)a->arg_size);
    struct dirent *ent;
    printf("size: %d\n", a->arg_size);

    int er = errno;
    ent = readdir(dir);

    if (ent == NULL) {
        printf("ent is null \n");
        if (errno != er) {
            //the error number changed, so something bad happened
            ret.return_val = (int*)malloc(sizeof(errno));
            memcpy(ret.return_val, &errno, sizeof(errno));
            //ret.return_val = errno;
            ret.return_size = sizeof(errno);
        } else {
            ret.return_val = NULL;
            ret.return_size = 0;
        }
        return ret;
    }
    free(dir);

    //TODO: gotta actually verify if we can just do this
    ret.return_val = (struct dirent*)malloc(sizeof(struct dirent));
    memcpy(ret.return_val, ent, sizeof(struct dirent));
    //ret.return_val = (void*)ent;
    ret.return_size = sizeof(struct dirent);
    return ret;
}

return_type fsCloseDir(const int nparams, arg_type *a) {
    if (nparams != 1) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }

    FSDIR* dir = (FSDIR*)a->arg_val;

    int *r = (int*)malloc(sizeof(int));
    *r = closedir(dir);

    ret.return_val = (void*)r;
    ret.return_size = sizeof(int);
    return ret;
}

return_type fsRemove(const int nparams, arg_type *a) {
    if (nparams != 1) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }
    char *path = a->arg_val;

    int *r = (int*)malloc(sizeof(int));
    *r = remove(path);
	ret.return_val = (void*)r;
	ret.return_size = sizeof(int);
	return ret; 
}

return_type fsOpen(const int nparams, arg_type *a) {
    if (nparams != 2) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }

    if(a->arg_size != sizeof(int)) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }

    char *fname = (char*)(a->next->arg_val);
    int flags = -1;
    int mode = *(int*)(a->arg_val);
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

    ret.return_val = (void*)r;
    ret.return_size = sizeof(int);

    //TODO
    //is there a lock on this file? wat do

    return ret;
}

return_type fsRead(const int nparams, arg_type *a) {
    if (nparams != 3) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }

    if(a->arg_size != sizeof(int) || a->next->next->arg_size != sizeof(int)) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }

    int fd = *(int*)(a->arg_val);
    int count = *(int*)(a->next->next->arg_val);
    char *buf = (char*)malloc((size_t)count);
    int r = read(fd, buf, (size_t)count);
    //printf("read buf: %s\n", buf);
    printf("read buf\n");

    int buf_index = 0;
    int size = sizeof(int) + r;
    char *retbuf = (char*)malloc((size_t)size);
    memcpy(retbuf, &r, sizeof(int));
    buf_index += sizeof(int);

    //strcpy(retbuf + buf_index, buf);
    memcpy(retbuf+buf_index, buf, (size_t)r);
    ret.return_val = (void*)(retbuf);
    ret.return_size = size;

    free(buf);
    return ret;
}

return_type fsWrite(const int nparams, arg_type *a) {
    if (nparams != 3) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }

    if(a->arg_size != sizeof(int) || a->next->next->arg_size != sizeof(int)) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }


    int fd = *(int*)(a->arg_val);
    char *buf = (char*)(a->next->arg_val);
    int count = *(int*)(a->next->next->arg_val);
    int *r = (int*)malloc(sizeof(int));
    *r = write(fd, buf,(size_t) count);


    ret.return_val = (void*)r;
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

    struct stat s;
    int err = stat(fs, &s);
    if (err == -1) {
        perror("stat"); exit(1);
    } else {
        if (S_ISDIR(s.st_mode)) {
            //is a directory... wat do?
            mountedDir = (char*)malloc((strlen(fs)+1));
            memcpy(mountedDir, fs, strlen(fs)+1);
        } else {
            perror("exists but is not dir"); exit(1);
        }
    }
	register_procedure("fsMount", 1, fsMount);
	register_procedure("fsUnmount", 1, fsUnmount);

	register_procedure("fsOpenDir", 1, fsOpenDir);
    register_procedure("fsReadDir", 1, fsReadDir);
    register_procedure("fsCloseDir", 1, fsCloseDir);

	register_procedure("fsOpen", 2, fsOpen);
	register_procedure("fsRead", 3, fsRead);
	register_procedure("fsWrite", 3, fsWrite);
	register_procedure("fsClose", 1, fsClose);
	register_procedure("fsRemove", 1, fsRemove);
	launch_server();

	return 0;
}
