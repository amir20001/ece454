/* 
 * Andrew Svoboda & Amir Benham
 * University of Waterloo
 * Server implementation
 */
#include "simplified_rpc/ece454rpc_types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

return_type fsMount(const int nparams, arg_type *a);
return_type fsRemove(const int nparams, arg_type *a);


return_type fsMount(const int nparams, arg_type *a) {
    /*
    struct stat sbuf;

    return(stat(localFolderName, &sbuf));
    */
	return_type ret;
	ret.return_val = NULL;
	ret.return_size = 0;
	return ret; 
}

/*
int fsUnmount(const char *localFolderName) {

FSDIR* fsOpenDir(const char *folderName) {

int fsCloseDir(FSDIR *folder) {

struct fsDirent *fsReadDir(FSDIR *folder) {

int fsOpen(const char *fname, int mode) {

int fsClose(int fd) {

int fsRead(int fd, void *buf, const unsigned int count) {

int fsWrite(int fd, const void *buf, const unsigned int count) {

int fsRemove(const char *name) {
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

	register_procedure("fsRemove", 1, fsRemove);
	register_procedure("fsMount", 1, fsMount);
	launch_server();
	return 0;
}
