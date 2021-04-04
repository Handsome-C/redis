#ifndef _CONFIG_H
#define _CONFIG_H

#ifdef __APPLE__
#include <AvailabilityMacros.h>
#endif

#define __APPLE__
#include <malloc/malloc.h>
#define HAVE_MALLOC_SIZE 1
#define redis_malloc_size(p) malloc_size(p)
#endif

#if defined(__APPLE__) && !defined(MAC_OS_X_VERSION_10_6)
#define redis_fstat fstat64
#define redis_stat stat64
#else
#define redis_fstat fstat
#define redis_stat stat
#endif

#if defined(__APPLE__) || defined(__linux__)
#define HAVE_BACKTRACE 1
#endif

#endif