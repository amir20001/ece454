/* 
 * Mahesh V. Tripunitara
 * University of Waterloo
 * You specify what goes in this file. I just have a "dummy"
 * specification of the FSDIR type.
 */

#ifndef _ECE_FS_OTHER_INCLUDES_
#define _ECE_FS_OTHER_INCLUDES_
#include <sys/types.h>
#include <dirent.h>

typedef int FSDIR;

//locktype indicates read/write lock
// 1 = read
// 2 = write
typedef struct _resource {
    int lock_type;
    unsigned long client;
    int fd;
    char *path;
    struct _resource *next;  
} resource;

typedef struct {
    int id;
    char path[256];
} FSMOUNT;

void remove_resource(resource **list, unsigned long client, int fd, char *path);
resource* find_resource(resource *res, unsigned long client, int fd, char *path);
void add_resource(int lock, unsigned long client, int fd, char *path);
#endif
