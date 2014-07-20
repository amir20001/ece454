#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ece454_fs.h"

extern void printBuf(char *buf, int size);

int main(int argc, char *argv[]) {
    char *dirname = NULL;
    char *ipaddr = NULL;
    unsigned int port = 0;

    if(argc > 3) {
        dirname = argv[1];
        ipaddr = argv[2];
        port = atoi(argv[3]);
    } else {
        perror("Please give correct args\ndirname ip port\n");
        exit(1);
    }

    int res = fsMount(ipaddr, port, dirname);
    printf("fsMount(): %d\n", res);

/*
    FSDIR *fd = fsOpenDir(dirname);
    if(fd == NULL) {
        printf("fsOpenDir failed\n");
        perror("fsOpenDir"); exit(1);
    }
    printf("successfully opened aliased dir: %s\n", dirname);

    struct fsDirent *fdent = NULL;
    for(fdent = fsReadDir(fd); fdent != NULL; fdent = fsReadDir(fd)) {
        printf("[%s], %d\n", fdent->entName, (int)(fdent->entType));
    }

    if(errno != 0) {
        perror("fsReadDir"); exit(1);
    }

    printf("fsCloseDir(): %d\n", fsCloseDir(fd));

*/


    FSDIR *fd = fsOpenDir(dirname);
    if(fd == NULL) {
        printf("fsOpenDir failed\n");
        perror("fsOpenDir"); exit(1);
    }
    printf("successfully opened aliased dir: %s\n", dirname);
    
    /*
    if(fsCloseDir(fd != 0)) {
        perror("fsCloseDir"); exit(1);
    }
    */

    int ff = open("/dev/urandom", 0);
    if(ff < 0) {
        perror("open(/dev/urandom)"); exit(1);
    } else {
        printf("open(/dev/urandom): %d\n", ff);
    }

    char fname[256];
    sprintf(fname, "%s/", dirname);
    if(read(ff, (void *)(fname+strlen(dirname)+1), 10) < 0) {
        perror("read(/dev/urandom)"); exit(1);
    }

    int i;
    for(i = 0; i < 10; i++) {
        //printf("%d\n", ((unsigned char)(fname[i]))%26);
        fname[i+strlen(dirname)+1] = ((unsigned char)(fname[i+strlen(dirname)+1]))%26 + 'a';
    }
    fname[10+strlen(dirname)+1] = (char)0;
    printf("Filename to write: %s\n", (char *)fname);

    char buf[256];
    if(read(ff, (void *)buf, 256) < 0) {
        perror("read(2)"); exit(1);
    }

    //printBuf(buf, 256);

    printf("close(/dev/urandom): %d\n", close(ff));

    ff = fsOpen(fname, 1);
    if(ff < 0) {
        perror("fsOpen(write)"); exit(1);
    }

    if(fsWrite(ff, buf, 256) < 256) {
        fprintf(stderr, "fsWrite() wrote fewer than 256\n");
    }

    if(fsClose(ff) < 0) {
        perror("fsClose"); exit(1);
    }

    char readbuf[256];
    if((ff = fsOpen(fname, 0)) < 0) {
        perror("fsOpen(read)"); exit(1);
    }

    int readcount = -1;

    if((readcount = fsRead(ff, readbuf, 256)) < 256) {
        fprintf(stderr, "fsRead() read fewer than 256\n");
    }

    if(memcmp(readbuf, buf, readcount)) {
        fprintf(stderr, "return buf from fsRead() differs from data written!\n");
    } else {
        printf("fsread(): return buf identical to data written upto %d bytes.\n", readcount);
    }

    if(fsClose(ff) < 0) {
        perror("fsClose"); exit(1);
    }

    printf("fsRemove(%s): %d\n", fname, fsRemove(fname));

    if(fsUnmount(dirname) < 0) {
        perror("fsUnmount"); exit(1);
    }

    return 0;
}
