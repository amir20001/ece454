#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ece454_fs.h"

extern void printBuf(char *buf, int size);
extern void fsii(void);

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



/*
    FSDIR *fd = fsOpenDir(dirname);
    if(fd == NULL) {
        printf("fsOpenDir failed\n");
        perror("fsOpenDir"); exit(1);
    }
    printf("successfully opened aliased dir: %s\n", dirname);

    char *filen = "/okay/thing.txt"; 
    printf("going to operate on %s\n", filen);
    int ff = fsOpen(filen, 0);
    if(ff < 0) {
        perror("fsOpen"); exit(1);
    } else {
        printf("fsOpen(): %d\n", ff);
    }

    char fname[15];
    if(fsRead(ff, (void *)fname, 10) < 0) {
        perror("fsRead"); exit(1);
    }
    if(fsRead(ff, (void *)fname, 10) < 0) {
        perror("fsRead"); exit(1);
    }

    int i;
    for(i = 0; i < 10; i++) {
        //printf("%d\n", ((unsigned char)(fname[i]))%26);
        fname[i] = ((unsigned char)(fname[i]))%26 + 'a';
    }

    fname[10] = (char)0;
    printf("Filename to write: %s\n", (char *)fname);

    int siz = 200;
    char buf[siz];
    exit(0);
    if(fsRead(ff, (void *)buf, siz) < 0) {
	    perror("fsRead(2)"); exit(1);
    }

    printf("%s\n", buf);
    //printBuf(buf, 256);

    printf("fsClose(): %d\n", fsClose(ff));

    exit(0);

    ff = fsOpen(fname, 1);
    if(ff < 0) {
	    perror("fsOpen(write)"); exit(1);
    }

    if(fsWrite(ff, buf, siz) < siz) {
	    fprintf(stderr, "fsWrite() wrote fewer than %d\n", siz);
    }

    if(fsClose(ff) < 0) {
	    perror("fsClose"); exit(1);
    }

    printf("fsRemove(%s): %d\n", fname, fsRemove(fname));
*/
    return 0;
}
