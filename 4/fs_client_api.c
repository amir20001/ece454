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
char *alias = NULL;
char *mountedDir = NULL;

char *str_replace(const char *str, const char *substr, const char *rep);

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
    ret = make_remote_call(srvIpOrDomName, srvPort, 
				"mount", 1,
				strlen(localFolderName)+1, (void*)(localFolderName)); 

    if (ret.return_val == 0) {
        errno = -1; //TODO: correct?
        return -1;
    }

    char *mm = (char*)ret.return_val;
    printf("got %s\n", mm);
    alias = (char*)malloc(ret.return_size);
    memcpy(alias, mm, ret.return_size);

    srvIp = srvIpOrDomName;
    port = srvPort;
    mountedDir = (char*)malloc(strlen(localFolderName)+1);
    memcpy(mountedDir, localFolderName, strlen(localFolderName)+1);

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
    char *path = str_replace(folderName, mountedDir, alias);
    printf("path:%s\n", path);
    ret = make_remote_call(srvIp, port, 
				"fsOpenDir", 1,
				strlen(path)+1, (void*)(path)); 

    if (ret.return_val == 0) {
        errno = -1; //TODO: correct?
        return NULL;
    }
    printf("opened dir: %d\n", (FSDIR*)ret.return_val);
    //TODO: is this correct?
    return (FSDIR*)(ret.return_val);
}

struct fsDirent *fsReadDir(FSDIR *folder) {
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
    printf("name [%s]\n", dent.entName);
    return &dent;
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
