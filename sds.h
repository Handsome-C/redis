#ifndef __SDS_H
#define __SDS_H
#include <sys/types.h>

typedef char *sds;
struct sdshdr {
    long len;
    long free;
    char buf[];
};
sds sdsnewlen(const void *init, size_t initlen);
sds sdsnew(const char *init);
