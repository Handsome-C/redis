#include "fmacros.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "anet.h"
#include "sds.h"
#include "adlist.h"
#include "zmalloc.h"

#define REDIS_CMD_INLINE 1
#define RREDIS_CMD_BULK 2

#define REDIS_NOTUSED(V) ((void) V)

static struct config{
    char *hostip;
    int hostport;
}config;

struct redisCommand{
    char *name;
    int arity;
    int flags;
};

static struct redisCommand cmdTable[] = {
        {"get",2,REDIS_CMD_INLINE},
        {"set",3,REDIS_CMD_BULK},
        {"setnx",3,REDIS_CMD_BULK},
        {"del",-2,REDIS_CMD_INLINE},
        {"exists",2,REDIS_CMD_INLINE},
        {"incr",2,REDIS_CMD_INLINE},
        {"decr",2,REDIS_CMD_INLINE},
        {"rpush",3,REDIS_CMD_BULK},
        {"lpush",3,REDIS_CMD_BULK},
        {"rpop",2,REDIS_CMD_INLINE},
        {"lpop",2,REDIS_CMD_INLINE},
        {"llen",2,REDIS_CMD_INLINE},
        {"lindex",3,REDIS_CMD_INLINE},
        {"lset",4,REDIS_CMD_BULK},
        {"lrange",4,REDIS_CMD_INLINE},
        {"ltrim",4,REDIS_CMD_INLINE},
        {"lrem",4,REDIS_CMD_BULK},
        {"sadd",3,REDIS_CMD_BULK},
        {"srem",3,REDIS_CMD_BULK},
        {"smove",4,REDIS_CMD_BULK},
        {"sismember",3,REDIS_CMD_BULK},
        {"scard",2,REDIS_CMD_INLINE},
        {"spop",2,REDIS_CMD_INLINE},
        {"sinter",-2,REDIS_CMD_INLINE},
        {"sinterstore",-3,REDIS_CMD_INLINE},
        {"sunion",-2,REDIS_CMD_INLINE},
        {"sunionstore",-3,REDIS_CMD_INLINE},
        {"sdiff",-2,REDIS_CMD_INLINE},
        {"sdiffstore",-3,REDIS_CMD_INLINE},
        {"smembers",2,REDIS_CMD_INLINE},
        {"incrby",3,REDIS_CMD_INLINE},
        {"decrby",3,REDIS_CMD_INLINE},
        {"getset",3,REDIS_CMD_BULK},
        {"randomkey",1,REDIS_CMD_INLINE},
        {"select",2,REDIS_CMD_INLINE},
        {"move",3,REDIS_CMD_INLINE},
        {"rename",3,REDIS_CMD_INLINE},
        {"renamenx",3,REDIS_CMD_INLINE},
        {"keys",2,REDIS_CMD_INLINE},
        {"dbsize",1,REDIS_CMD_INLINE},
        {"ping",1,REDIS_CMD_INLINE},
        {"echo",2,REDIS_CMD_BULK},
        {"save",1,REDIS_CMD_INLINE},
        {"bgsave",1,REDIS_CMD_INLINE},
        {"shutdown",1,REDIS_CMD_INLINE},
        {"lastsave",1,REDIS_CMD_INLINE},
        {"type",2,REDIS_CMD_INLINE},
        {"flushdb",1,REDIS_CMD_INLINE},
        {"flushall",1,REDIS_CMD_INLINE},
        {"sort",-2,REDIS_CMD_INLINE},
        {"info",1,REDIS_CMD_INLINE},
        {"mget",-2,REDIS_CMD_INLINE},
        {"expire",3,REDIS_CMD_INLINE},
        {"ttl",2,REDIS_CMD_INLINE},
        {"slaveof",3,REDIS_CMD_INLINE},
        {"debug",-2,REDIS_CMD_INLINE},
        {NULL,0,0}
};

static int cliReadReply(int fd);

static struct redisCommand *lookupCommand(char *name){
    int j = 0;
    while(cmdTable[j].name != NULL){
        if(!strcasecmp(name,cmdTable[j])) return &cmdTable[j];
        j++;
    }
    return NULL;
}

static int cliConnect(void){
    char err[ANET_ERR_LEN];
    int fd;

    fd = anetTcpConnect(err,config.hostip,config.hostport);
    if (fd == ANET_ERR) {
        fprintf(stderr,"Connect: %s\n",err);
        return -1;
    }
    anetTcpNoDelay(NULL,fd);
    return fd;
}

static sds cliReadLine(int fd){
    sds line = sdsempty();
    while(1) {
        char c;
        ssize_t ret;
        ret = read(fd,&c,1);
        if (ret == -1) {
            sdsfree(line);
            return NULL;
        }else if ((ret == 0) || (c == '\n')) {
            break;
        }else {
            line = sdscatlen(line)
        }
    }
}


















