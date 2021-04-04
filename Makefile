DEBUG ?= -g -rdynamic -ggdb
CFLAGS ?= -std=c99 -pedantic -02 -Wall -W
CCOPY = ${CFLAGS}

OBJ = adlist.o ae.o anet.o dict.o redis.o sds.o zmalloc.o lzf_c.o lzf_d.o pqsort.o
BENCHOBJ = ae.o anet.o benchmark.o sds.o adlist.o zmalloc.o
CLIOBJ = anet.o sds.o adlist.o redis-cli.o zmalloc.o

PRGNAME = redis-server
BENCHPRGNAME = redis-benchmark
CLIPRGNAME = redis-cli

all: redis-server redis-benchmark redis-cli

adlist.o: adlist.c adlist.h zmalloc.h
ae.o: ae.c ae.h zmalloc.h
anet.o: anet.c fmacros.h anet.h
benchmark.o: benchmark.c fmacros.h ae.h anet.h sds.h adlist.h zmalloc.h
dict.o: dict.c fmacros.h dict.h zmalloc.h
lzf_c.o: lzf_c.c lzfP.h
lzf_d.o: lzf_d.c lzfP.h
pqsort.o: pqsort.c
redis.o: redis.c fmacros.h ae.h sds.h anet.h dict.h adlist.h zmalloc.h lzf.h pqsort.h config.h
sds.o: sds.c sds.h zmalloc.h
zmalloc.o: zmalloc.c config.h


