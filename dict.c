#include "fmacros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <limits.h>

#include "dict.h"
#include "zmalloc.h"

static void _dictPanic(const char *fmt, ...) {
    va_list ap;
    va_start(ap,fmt);
    fprintf(stderr, "\nDICT LIBRARY PANIC: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n\n");
    va_end(ap);
}

static void *_dictAlloc(size_t size)
{
    void *p = zmalloc(size);
    if (p == NULL)
        _dictPanic("Out of memory");
    return p;
}

static void _dictFree(void *ptr) {
    zfree(ptr);
}

static int _dictExpandIfNeeded(dict *ht);
static unsigned long _dictNextPower(unsigned long size);
static int _dictKeyIndex(dict *ht, const void *key);
static int _dictInit(dict *ht, dictType *type, void *privDataPtr);

unsigned int dictIntHashFunction(unsigned int key) {
    
}












