/* 
 * Andrew Svoboda & Amir Benham
 * University of Waterloo
 * Client api implementation of Assignment 4 procedures 
 */
#include "ece454_fs.h"
#include "simplified_rpc/ece454rpc_types.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *str_replace(const char *str, const char *substr, const char *rep);

struct fsDirent dent;
int id;
char *srvIp = NULL;
int port = 0;
char alias[256];
char *mountedDir = NULL;

//relevant str_replace from http://coding.debuntu.org/c-implementing-str_replace-replace-all-occurrences-substring
char * str_replace ( const char *string, const char *substr, const char *replacement ) {
  char *tok = NULL;
  char *newstr = NULL;
  char *oldstr = NULL;
  char *head = NULL;
 
  /* if either substr or replacement is NULL, duplicate string a let caller handle it */
  if ( substr == NULL || replacement == NULL ) return strdup (string);
  newstr = strdup (string);
  head = newstr;
  while ( (tok = strstr ( head, substr ))){
    oldstr = newstr;
    newstr = malloc ( strlen ( oldstr ) - strlen ( substr ) + strlen ( replacement ) + 1 );
    /*failed to alloc mem, free old string and return NULL */
    if ( newstr == NULL ){
      free (oldstr);
      return NULL;
    }
    memcpy ( newstr, oldstr, tok - oldstr );
    memcpy ( newstr + (tok - oldstr), replacement, strlen ( replacement ) );
    memcpy ( newstr + (tok - oldstr) + strlen( replacement ), tok + strlen ( substr ), strlen ( oldstr ) - strlen ( substr ) - ( tok - oldstr ) );
    memset ( newstr + strlen ( oldstr ) - strlen ( substr ) + strlen ( replacement ) , 0, 1 );
    /* move back head right after the last replacement */
    head = newstr + (tok - oldstr) + strlen( replacement );
    free (oldstr);
  }
  return newstr;
}

int fsMount(const char *srvIpOrDomName, const unsigned int srvPort, const char *localFolderName) {

    return_type ret;
	printf("fsMount\n");
    ret = make_remote_call(srvIpOrDomName, srvPort, 
				"fsMount", 1,
				strlen(localFolderName)+1, (void*)(localFolderName)); 

    if (ret.return_val == 0) {
        errno = -1; //TODO: correct?
        return -1;
    }

	FSMOUNT *mount= ( FSMOUNT*)malloc((size_t)ret.return_size);
	memcpy(mount,ret.return_val,(size_t)ret.return_size);
	printf("my id: %d\n",mount->id);
	id=mount->id;
    memcpy(alias, mount->path, 256);
	printf("done setting alias \n");
    srvIp = srvIpOrDomName;
    port = srvPort;
    mountedDir = (char*)malloc(strlen(localFolderName)+1);
    memcpy(mountedDir, localFolderName, strlen(localFolderName)+1);
	printf("fsMount done\n");
    return 0;
}

int fsUnmount(const char *localFolderName) {
    return_type ret;
	printf("fsUnmount\n");
    ret = make_remote_call(srvIp, port, 
				"fsUnmount", 1,
				strlen(localFolderName)+1, (void*)(localFolderName)); 

    if (ret.return_val != 0) {
        errno = *(int*)ret.return_val;
        return -1;
    }
	printf("fsUnmount done\n");
    return 0;
}

FSDIR* fsOpenDir(const char *folderName) {
    return_type ret;
    char *path = str_replace(folderName, mountedDir, alias);
	int val;
	do{
	   printf("fsOpenDir\n");
	   ret = make_remote_call(srvIp, port, 
					"fsOpenDir", 1,
					strlen(path)+1, (void*)(path)); 
		if(ret.is_error==1){
			val = *((int *)ret.return_val);
		}		
	} while(ret.is_error==1 && val == EAGAIN);
    if (ret.return_val == 0) {
        errno = -1; //TODO: correct?
        return NULL;
    }
	val = *((int *)ret.return_val);
	printf("fsOpenDir done\n");
    return (FSDIR*)(ret.return_val);
}

struct fsDirent *fsReadDir(FSDIR *folder) {
    return_type ret;
    //read the comment at line 80 in server.c
    //int s = (sizeof(int) + sizeof(off_t) + (3*sizeof(size_t)) + sizeof(struct dirent));
	printf("fsDirent\n");
    ret = make_remote_call(srvIp, port, 
				"fsReadDir", 1,
				sizeof(FSDIR), (void*)(folder)); 
    if (ret.return_val == 0) {
        return NULL;
    } else if (ret.return_size == sizeof(errno)) {
        errno = *(int*)ret.return_val;
        return NULL;
    }
	
    struct dirent *d= (struct dirent*)malloc((size_t)ret.return_size);
	memcpy(d,ret.return_val,(size_t)ret.return_size);
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
	free(d);
	printf("fsDirent done\n");
    return &dent;
}

int fsCloseDir(FSDIR *folder) {
    return_type ret;
	int r;
	do{
		printf("fsCloseDIr\n");
		ret = make_remote_call(srvIp, port, 
					"fsCloseDir", 1,
					sizeof(FSDIR), (void*)(folder)); 

		r = *(int*)ret.return_val;
	} while(ret.is_error==1 && r == EAGAIN);
    printf("fsCloseDIr done\n");
	if (r != 0) {
        errno = r;
        return -1;
    }
    return 0;
}

int fsOpen(const char *fname, int mode) {
    return_type ret;
    char *mountedPath = str_replace(fname, mountedDir, alias);
    char *path = str_replace(mountedPath, "/../", "/");
	int fd;
	do{
		printf("fsOpen\n");
		ret = make_remote_call(srvIp, port, 
					"fsOpen", 2,
					sizeof(int), (void*)(&mode),
					strlen(path)+1, (void*)path); 
				
		fd = *(int*)(ret.return_val); 
	}while(ret.is_error==1 && fd == EAGAIN);
	printf("fsOpen done\n");
    if (fd == -1) {
        errno = 1; //TODO
        return -1;
    }
    return fd;
}

int fsClose(int fd) {
    return_type ret;
	int r;
	do{
		printf("fsClose\n");
		ret = make_remote_call(srvIp, port, 
					"fsClose", 1,
					sizeof(int), (void*)(&fd)); 

		r= *(int*)ret.return_val;
	}while(ret.is_error==1 && r == EAGAIN);
	printf("fsClose done\n");
    if (r != 0) {
        errno = r;
        return -1;
    }
    return 0;
}

int fsRead(int fd, void *buf, const unsigned int count) {
    return_type ret;
	printf("fsRead\n");
    ret = make_remote_call(srvIp, port, 
				"fsRead", 3,
                sizeof(int), (void*)(&fd),
				strlen(buf)+1, buf,
                sizeof(int), (void*)(&count)); 
            
    //int fd = *(int*)(ret.return_val); 
    int res;
    //we have the size of the buffer to read, and the size of everything together
    memcpy(&res, (ret.return_val), sizeof(int));
    memcpy(buf, ((char*)ret.return_val + sizeof(int)),(size_t)(ret.return_size - sizeof(int)));

    if (res == -1) {
        errno = res; //TODO
        return -1;
    }
    return res;
}

int fsWrite(int fd, const void *buf, const unsigned int count) {
    return_type ret;
	int res;
	do{
		printf("fsWrite\n");
		ret = make_remote_call(srvIp, port, 
					"fsWrite", 3,
					sizeof(int), (void*)(&fd),
					sizeof(int), (void*)(&count),
					count, buf); 
				
		res = *(int*)(ret.return_val); 
	} while(ret.is_error==1 && res == EAGAIN);
    if (res == -1) {
        errno = 1; //TODO
        return -1;
    }
    return res;
}

int fsRemove(const char *name) {
    return_type ret;
    char *mountedPath = str_replace(name, mountedDir, alias);
    char *path = str_replace(mountedPath, "/../", "/");
	int val;
	do{
		printf("fsRemove\n");
		ret = make_remote_call(srvIp, port, 
					"fsRemove", 1,
					strlen(path)+1, (void*)(path)); 
		val =*(int*)ret.return_val;	
	} while(ret.is_error==1 && val == EAGAIN);
    if (*(int*)ret.return_val != 0) {
        errno = *(int*)ret.return_val;
        return -1;
    }
    return 0;
}
