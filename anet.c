#include "fmacros.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>

#include "anet.h"

static void anetSetError(char *err, const char *fmt, ...){
    va_list ap;
    if(!err) return;
    va_start(ap,fmt);
    vsnprintf(err, ANET_ERR_LEN, fmt, ap);
    va_end(ap);
}

int anetNonBlock(char *err, int fd){
    int flags;
    if((flags = fcntl(fd,F_GETFL)) == -1){
        anetSetError(err, "fcntl(F_GETFL): %s\n", strerror(errno));
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        anetSetError(err, "fcntl(F_SETFL,O_NONBLOCK): %s\n", strerror(errno));
        return ANET_ERR;
    }
    return ANET_OK;
}

int anetTcpNoDelay(char *err,int fd){
    int yes = 1;
    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes)) == -1){
        anetSetError(err, "setsockopt TCP_NODELAY: %s\n", strerror(errno));
        return ANET_ERR;
    }
    return ANET_OK;
}

int anetSetSendBuffer(char *err,int fd,int buffsize){
    if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &buffsize, sizeof(buffsize)) == -1)
    {
        anetSetError(err, "setsockopt SO_SNDBUF: %s\n", strerror(errno));
        return ANET_ERR;
    }
    return ANET_OK;
}

int anetTcpKeepAlive(char *err, int fd){
    int yes = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(yes)) == -1) {
        anetSetError(err, "setsockopt SO_KEEPALIVE: %s\n", strerror(errno));
        return ANET_ERR;
    }
    return ANET_OK;
}

int anetResolve(char *err, char *host, char *ipbuf){
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    if (inet_aton(host, &sa.sin_addr) == 0) {
        struct hostent *he;

        he = gethostbyname(host);
        if (he == NULL) {
            anetSetError(err, "can't resolve: %s\n", host);
            return ANET_ERR;
        }
        memcpy(&sa.sin_addr, he->h_addr, sizeof(struct in_addr));
    }
    strcpy(ipbuf,inet_ntoa(sa.sin_addr));
    return ANET_OK;
}

#define ANET_CONNECT_NONE 0
#define ANET_CONNECT_NONBLOCK 1

static int anetTcpGenericConnect(char *err,char *addr, int port, int flags){
    int s, on = 1;
    struct sockaddr_in sa;
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        anetSetError(err, "creating socket: %s\n", strerror(errno));
        return ANET_ERR;
    }
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    if (inet_aton(addr))
}
