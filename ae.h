#ifndef __AE_H__
#define __AE_H__

struct aeEventLoop;

typedef void aeFileProc(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask);
typedef int aeTimeProc(struct aeEventLoop *eventLoop, long long id, void *clientData);
typedef void aeEventFinalizerProc(struct aeEventLoop *eventLoop, void *clientData);

typedef struct aeFileEvent{
    int fd;
    int mask;
    aeFileProc *fileProc;
    aeEventFinalizerProc *finalizerProc;
    void *clientData;
    struct aeFileEvent *next;
};

typedef struct aeEventLoop {
    long long timeEventNextId;

};