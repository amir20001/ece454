/* 
 * Andrew Svoboda & Amir Benham
 * University of Waterloo
 * Server implementation
 */
#include "simplified_rpc/ece454rpc_types.h"
#include "fsOtherIncludes.h"
#include "queue.c"
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
int open_dir_id = 1;
node *open_dir_queue;
resource *resource_list;
int client_id=0;

unsigned long client_ip;

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

bool add_open_dir(DIR* dir, int id) {
	if (dir == NULL) {
		return false;
	}

	node *cur_node = (node *) malloc(sizeof(node));
	cur_node->dir = dir;
	cur_node->id = id;
	cur_node->next = open_dir_queue;
	open_dir_queue = cur_node;
	return true;
}

resource* find_resource(resource *res, int id, int fd, char *path) {
    resource *found = NULL;
    while (res != NULL) {
		if (path != NULL) {
		printf("%s : %s %d\n", res->path, path, strcmp(res->path, path));
		}
        if (path != NULL && strcmp(res->path, path) == 0) {
		    printf("found resource by path\n");
		   found = res;
            break;
        } else if (fd != 0 && res->fd == fd) {
		    printf("found resource by fd\n");

            found = res;
            break;
        }
        res = res->next;
    }
    return found;
}

void remove_resource(resource **list, int id, int fd, char *path) {
    if(list == NULL) {
        return;
    }
    resource *cur;
    resource *prev = NULL;
    for (cur = *list; cur != NULL; prev = cur, cur = cur->next) {

        if ((path != NULL && strcmp(cur->path, path) == 0 && cur->id == id) || (fd != 0 && cur->fd == fd && cur->id == id)) {
            if (prev == NULL) {
                *list = cur->next;
            } else {
                prev->next = cur->next;
            }
            free(cur->path);
            free(cur);
            return;
        }
    }
}

void add_resource(int lock, int id, int fd, char *path) {
    if (path == NULL) {
        return;
    }

    resource *res = (resource *)malloc(sizeof(resource));
    res->lock_type = lock;
    res->id = id;
    res->fd = fd;
    res->path = (char *)malloc((size_t)(strlen(path)+1));
    memcpy(res->path, path, (size_t)(strlen(path)+1));
    res->next = resource_list;
    resource_list = res;
    return;
}

return_type fsMount(const int nparams, arg_type *a) {
    if (nparams != 1) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }
	client_id++;
	FSMOUNT *mount= malloc(sizeof(FSMOUNT));
	mount->id=client_id;
    ret.return_val = (void*)mount;
	memset (mount->path,0,256);
    memcpy(mount->path, mountedDir, strlen(mountedDir)+1);
	ret.return_size = sizeof(FSMOUNT);
	return ret; 
}

return_type fsUnmount(const int nparams, arg_type *a) {
    if (nparams != 2) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }
    ret.return_val = NULL;
	ret.return_size = 0;
	return ret; 
}

return_type fsOpenDir(const int nparams, arg_type *a) {
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
	int id = *(int*)(a->arg_val);
	
	char *path = a->next->arg_val;
    resource *res = find_resource(resource_list, id, 0, path);
    if(res != NULL && res->id != id) {
        ret.return_val = malloc(sizeof(EAGAIN));
		*(int*)ret.return_val =-EAGAIN;
		ret.return_size = sizeof(EAGAIN);			
		return ret;
    }

	DIR* dir = opendir(path);

	if (dir == NULL) {
		ret.return_val = NULL;
		ret.return_size = 0;
		return ret;
	}
	open_dir_id++;
	add_open_dir(dir, open_dir_id);
	int *pid = (int*)malloc(sizeof(int));
	memcpy(pid, &open_dir_id, sizeof(int));

    add_resource(1, id, *pid, path);
    //remove the old one,
    if (res != NULL) {
        remove_resource(&resource_list, id, 0, path);
    }

	ret.return_val = pid;
	//memcpy(ret.return_val,&open_dir_id, sizeof(int));
	//ret.return_val = &open_dir_id;
	ret.return_size = sizeof(FSDIR);
	return ret;
}

return_type fsReadDir(const int nparams, arg_type *a) {

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
	int id = *(int*)(a->arg_val);
	
	FSDIR *dir_id = malloc((size_t) a->next->arg_size);
	memcpy(dir_id, a->next->arg_val, (size_t) a->next->arg_size);
	struct dirent *ent;
	node* found=find(open_dir_queue,*dir_id);
	int er = errno;
	ent = readdir(found->dir);

	if (ent == NULL) {
		if (errno != er) {
			//the error number changed, so something bad happened
			ret.return_val = (int*) malloc(sizeof(errno));
			memcpy(ret.return_val, &errno, sizeof(errno));
			//ret.return_val = errno;
			ret.return_size = sizeof(errno);
		} else {
			ret.return_val = NULL;
			ret.return_size = 0;
		}
		return ret;
	}
	free(dir_id);

	ret.return_val = (struct dirent*) malloc(sizeof(struct dirent));
	memcpy(ret.return_val, ent, sizeof(struct dirent));
	//ret.return_val = (void*)ent;
	ret.return_size = sizeof(struct dirent);
	return ret;
}

return_type fsCloseDir(const int nparams, arg_type *a) {
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
	int id = *(int*)(a->arg_val);
	
	FSDIR *dir_id = malloc((size_t) a->next->arg_size);
    resource *res = find_resource(resource_list, id, *dir_id, NULL);
    if(res != NULL && res->id != id) {
        ret.return_val = malloc(sizeof(EAGAIN));
		*(int*)ret.return_val =-EAGAIN;
		ret.return_size = sizeof(EAGAIN);			
		return ret;
    }

	memcpy(dir_id, a->next->arg_val, (size_t) a->next->arg_size);
	node* found=find(open_dir_queue,*dir_id);

    int *r = (int*)malloc(sizeof(int));
    *r = closedir(found->dir);
    if (*r == -1) {
        *r = -errno;
    }

    remove_resource(&resource_list, id, *dir_id, NULL);
    free(dir_id);
    ret.return_val = (void*)r;
    ret.return_size = sizeof(int);
    return ret;
}

return_type fsRemove(const int nparams, arg_type *a) {
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
	int id = *(int*)(a->arg_val);
	
    char *path = a->next->arg_val;
    resource *res = find_resource(resource_list, id, 0, path);
    if(res != NULL && res->id != id) {
        ret.return_val = malloc(sizeof(EAGAIN));
		*(int*)ret.return_val = -EAGAIN;
		ret.return_size = sizeof(EAGAIN);			
		return ret;
    }

    int *r = (int*)malloc(sizeof(int));
    *r = remove(path);
	if (*r == -1) {
		*r = -errno;
	}
	ret.return_val = (void*)r;
	ret.return_size = sizeof(int);

    remove_resource(&resource_list, id, 0, path);
	return ret; 
}

return_type fsOpen(const int nparams, arg_type *a) {
    if (nparams != 3) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }
	if(a->arg_size != sizeof(int)) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }
	int id = *(int*)(a->arg_val);
	
    if(a->next->arg_size != sizeof(int)) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }
    char *fname = (char*)(a->next->next->arg_val);
	printf("fname: %s , id: %d \n",fname,id);
    resource *res = find_resource(resource_list, id, 0, fname);
    if(res != NULL && res->id != id ) {
		ret.return_val = malloc(sizeof(EAGAIN));
		*(int*)ret.return_val = -EAGAIN;
		ret.return_size = sizeof(EAGAIN);			
		return ret;
    }

    int flags = -1;
    int mode = *(int*)(a->next->arg_val);
    if (mode == 0) {
        flags = O_RDONLY;
    } else {
        flags = O_WRONLY | O_CREAT;
    }
    int *r = (int*)malloc(sizeof(int));
    *r = open(fname, flags, S_IRWXU);

    add_resource(mode, id, *r, fname);
    //remove the old one,
    if (res != NULL) {
        remove_resource(&resource_list, id, 0, fname);
    }

	if (*r == -1) {
		*r = -errno;
	}
    ret.return_val = (void*)r;
    ret.return_size = sizeof(int);
    return ret;
}

return_type fsClose(const int nparams, arg_type *a) {
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
	int id = *(int*)(a->arg_val);
	
    if(a->next->arg_size != sizeof(int)) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }
	
	printf("closing file\n");
    int fd = *(int*)(a->next->arg_val);
	printf("file discriptor:%d\n",fd);
    resource *res = find_resource(resource_list, id, fd, NULL);
    if(res != NULL && res->id != id) {
        ret.return_val = malloc(sizeof(EAGAIN));
		*(int*)ret.return_val =-EAGAIN;
		ret.return_size = sizeof(EAGAIN);			
		return ret;
    }

    int *r = (int*)malloc(sizeof(int));
    *r = close(fd);

	if (*r == -1) {
		*r = -errno;
	}
    ret.return_val = (void*)r;
    ret.return_size = sizeof(int);

    remove_resource(&resource_list, id, fd, NULL);

    return ret;
}

return_type fsRead(const int nparams, arg_type *a) {
    if (nparams != 4) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }
	
	if(a->arg_size != sizeof(int)) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }
	int id = *(int*)(a->arg_val);

    if(a->next->arg_size != sizeof(int) || a->next->next->next->arg_size != sizeof(int)) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }
    int fd = *(int*)(a->next->arg_val);
    int count = *(int*)(a->next->next->next->arg_val);
    char *buf = (char*)malloc((size_t)count);
    int r = read(fd, buf, (size_t)count);

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
    if (nparams != 4) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }
	
	if(a->arg_size != sizeof(int)) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }
	int id = *(int*)(a->arg_val);
	
    if(a->next->arg_size != sizeof(int) || a->next->next->arg_size != sizeof(int)) {
        ret.return_val = NULL;
        ret.return_size = 0;
        return ret;
    }
    int fd = *(int*)(a->next->arg_val);

    resource *res = find_resource(resource_list, id, fd, NULL);
    if(res != NULL && res->id != id) {
        ret.return_val = malloc(sizeof(EAGAIN));
		*(int*)ret.return_val =-EAGAIN;
		ret.return_size = sizeof(EAGAIN);			
		return ret;
    }


	int count = *(int*)(a->next->next->arg_val);
    char *buf = (char*)(a->next->next->next->arg_val);
    
    int *r = (int*)malloc(sizeof(int));
	printBuf(buf, 256);
    *r = write(fd, buf,(size_t) count);

	if (*r == -1) {
		*r = -errno;
	}
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
	register_procedure("fsUnmount", 2, fsUnmount);

	register_procedure("fsOpenDir", 2, fsOpenDir);
    register_procedure("fsReadDir", 2, fsReadDir);
    register_procedure("fsCloseDir", 2, fsCloseDir);

	register_procedure("fsOpen", 3, fsOpen);
	register_procedure("fsRead", 4, fsRead);
	register_procedure("fsWrite", 4, fsWrite);
	register_procedure("fsClose", 2, fsClose);
	register_procedure("fsRemove", 2, fsRemove);
	launch_server();

	return 0;
}
