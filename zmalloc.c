#include <stdlib.h>
#include <string.h>
#include "config.h"

//Copyright (c) 2006-2009
// 就是对malloc realloc做了简单的封装，可以直接得出分配的内存大小


static size_t used_memory = 0;

void *zmalloc(size_t size){
    void *ptr = malloc(size+sizeof(size_t));
    if(!ptr) return NULL;
#ifdef HAVE_MALLOC_SIZE
    used_memory += redis_malloc_size(ptr);
    return ptr;
#else
    *((size_t*)ptr) = size;
    used_memory += size+ sizeof(size_t);
    return (char*)ptr+ sizeof(size_t);
#endif
}

void *zrealloc(void *ptr, size_t size){
#ifdef HAVE_MALLOC_SIZE
    void *realptr;
#endif
    size_t oldsize;
    void *newptr;

    if(ptr == NULL)return zmalloc(size);
#ifdef HAV_MALLOC_SIZE
    oldsize = redis_malloc_size(ptr);
    newptr = realloc(ptr, size);
    if(!newptr)return NULL;
    used_memory -= oldsize;
    used_memory += redis_malloc_size(newptr);
    return newptr;
#else
    realptr = (char*)ptr-sizeof(size_t);
    oldsize = *((size_t*)realptr);
    newptr = realloc(realptr,size+ sizeof(size_t));
    if(!newptr)return NULL;
    *((size_t*)newptr)=size;
    return (char*)newptr+ sizeof(size_t);
#endif
}

char *zstrdup(const char *s){
    size_t l=strlen(s)+1;
    char *p=zmalloc(l);
    memcpy(p,s,l);
    return p;
}

size_t zmalloc_used_memory(void){
    return used_memory;
}

void zfree(void *ptr) {
#ifndef HAVE_MALLOC_SIZE
    void *realptr;
    size_t oldsize;
#endif

    if (ptr == NULL) return;
#ifdef HAVE_MALLOC_SIZE
    used_memory -= redis_malloc_size(ptr);
    free(ptr);
#else
    realptr = (char*)ptr-sizeof(size_t);
    oldsize = *((size_t*)realptr);
    used_memory -= oldsize+sizeof(size_t);
    free(realptr);
#endif
}

