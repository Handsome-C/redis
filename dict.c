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

//一个hash算法，打散数据
unsigned int dictIntHashFunction(unsigned int key) {
    key += ~(key << 15);
    key ^=  (key >> 10);
    key +=  (key << 3);
    key ^=  (key >> 6);
    key += ~(key << 11);
    key ^=  (key >> 16);
    return key;
}

unsigned int dictIdentityHashFunction(unsigned int key)
{
    return key;
}

unsigned int dictGenHashFunction(const unsigned char *buf, int len){
    unsigned int hash = 5381;
    while (len--)
        hash = ((hash << 5) + hash) + (*buf++);
    return hash;
}

static void _dictReset(dict *ht){
    ht->table=NULL;
    ht->size=0;
    ht->sizemask=0;
    ht->used=0;
}

dict *dictCreate(dictType *type, void *privDataPtr){
    dict *ht=_dictAlloc(sizeof(*ht));
    _dictInit()
    _dictInit(ht,type,privDataPtr);
    return ht;
}

int _dictInit(dict *ht,dictType *type,void *privDataPtr){
    _dictReset(ht);
    ht->type = type;
    ht->privdata = privDataPtr;
    return DICT_OK;
}

int dictResize(dict *ht){
    int minimal=ht->used;
    if (minimal < DICT_HT_INITIAL_SIZE)
    minimal = DICT_HT_INITIAL_SIZE;
    return dictExpand(ht, minimal);
}

int dictExpand(dict *ht,unsigned long size){
    dict n;
    unsigned long readsize=_dictNextPower(size), i;
    if(ht->used > size)return DICT_ERR;
    _dictInit(&n, ht->type, ht->privdata);
    n.size=realsize;
    n.sizemask=readsize-1;
    n.table=_dictAlloc(realsize*sizeof(dictEntry*));
    memset(n.table,0,realsize*sizeof(dictEntry*));

    n.used=ht->used;
    for(i=0;i<ht->size && ht->used > 0;i++){
        dictEntry *he, *nextHe;
        if(he->table[i]==NULL)continue;
        he=ht->table[i];

        // 我觉得这里可以不用这么搞，可以直接遍历链表，得到一个size，然后指针重新指一次就可以了，可能是考虑到原子操作吧
        while(he){
            unsigned int h;
            nextHe=he->next;
            h=dictHashKey(ht,he->key) & n.sizemask;
            he->next=n.table[h];
            n.table[h]=he;
            ht->used--;
            he=nextHe;
        }
    }
    assert(ht->used==0);
    _dictFree(ht->table);

    *ht=n;
    return DICT_OK;
}

int dictAdd(dict *ht,void *key,void *val){
    int index;
    dictEntry *entry;
    if((index=_dictKeyIndex(ht,key))==-1)return DICT_ERR;

    entry=_dictAlloc(sizeof(*entry));
    entry->next=ht->table[index];
    ht->table[index]=entry;

    dictSetHashKey(ht, entry, key);
    dictSetHashVal(ht, entry, val);
    ht->used++;
    return DICT_OK;
}

int dictReplace(dict *ht,void *key,void *val){
    dictEntry *entry;

    if(dictAdd(ht,key,val)==DICT_OK)return DICT_OK;
    entry=dictFind(ht,key);

}

static unsigned long _dictNextPower(unsigned long size){
    unsigned long i=DICT_HT_INITIAL_SIZE;
    if(size>=LONG_MAX)return LONG_MAX;
    while(1){
        if(i>=size)return i;
        i*=2;
    }
}

static int _dictKeyIndex(dict *ht,const void *key){
    unsigned int h;
    dictEntry *he;
    if(_dictExpandIfNeeded()==DICT_ERR)return -1;
    h=dictHashKey(ht,key)&ht->sizemask;
    he=ht->table[h];
    while(he){
        if(dictCompareHashKeys(ht,key,he->key))return -1;
        he=he->next;
    }
    return h;
}

static int _dictExpandIfNeeded(dict *ht){
    if(ht->size==0)return dictExpand(ht,DICT_HT_INITIAL_SIZE);
    if(ht->used==ht->size)return dictExpand(ht, ht->size*2);
    return DICT_OK;
}

dictEntry *dictFind(dict *ht,const void *key){
    dictEntry *he;
    unsigned int h;
    if(ht->size==0)return NULL;
    h=dictHashKey(ht,key)&ht->sizemask;
    he=ht->table[h];
    while(he){
        if (dictCompareHashKeys(ht, key, he->key))
            return he;
        he = he->next;
    }
    return NULL;
}

dictIterator *dictNext(dictIterator *iter){
    while(1){
        if(iter->entry==NULL){
            iter->index++;
            if(iter->index >= (signed)iter->ht->size)break;
            iter->entry = iter->ht->table[iter->index];
        }else {
            iter->entry = iter->nextEntry;
        }
        if (iter->entry) {
            iter->nextEntry = iter->entry->next;
            return iter->entry;
        }
    }
    return NULL;
}

void dictReleaseIterator(dictIterator *iter)
{
    _dictFree(iter);
}

dictEntry *dictGetRandomKey(dict *ht){
    dictEntry *he;
    unsigned int h;
    int listlen,listele;
    if(ht->used==0)return NULL;
    do{
        h=random()&ht->sizemask;
        he=ht->table[h];
    }while(he==NULL);
    listlen=0;
    while(he){
        he=he->next;
        listlen++;
    }
    listele=random()%listlen;
    he=ht->table[h];
    while(listele--)he=he->next;
    return he;
}

void dictEmpty(dict *ht) {
    _dictClear(ht);
}

int _dictClear(dict *ht){
    unsigned long i;
    for(i=0;i<ht->size && ht->used>0;i++){
        dictEntry *he,*nextHe;
        if ((he = ht->table[i]) == NULL) continue;
        while(he) {
            nextHe = he->next;
            dictFreeEntryKey(ht, he);
            dictFreeEntryVal(ht, he);
            _dictFree(he);
            ht->used--;
            he = nextHe;
        }
    }
    _dictFree(ht->table);
    _dictReset(ht);
    return DICT_OK;
}

#define DICT_STATS_VECTLEN 50
void dictPrintStats(dict *ht){
    unsigned long i,slots=0, chainlen,maxchainlen=0;
    unsigned long totchainlen = 0;
    unsigned long clvector[DICT_STATS_VECTLEN];

    if (ht->used == 0) {
        printf("No stats available for empty dictionaries\n");
        return;
    }

    for (i = 0; i < DICT_STATS_VECTLEN; i++) clvector[i] = 0;
    for (i = 0; i < ht->size; i++) {
        dictEntry *he;
        if (ht->table[i] == NULL) {
            clvector[0]++;
            continue;
        }
        slots++;
        chainlen = 0;
        he = ht->table[i];
        while(he) {
            chainlen++;
            he = he->next;
        }
        clvector[(chainlen < DICT_STATS_VECTLEN) ? chainlen : (DICT_STATS_VECTLEN-1)]++;
        if (chainlen > maxchainlen) maxchainlen = chainlen;
        totchainlen += chainlen;
    }
    printf("Hash table stats:\n");
    printf(" table size: %ld\n", ht->size);
    printf(" number of elements: %ld\n", ht->used);
    printf(" different slots: %ld\n", slots);
    for (i = 0; i < DICT_STATS_VECTLEN-1; i++) {
        if (clvector[i] == 0) continue;
        printf("   %s%ld: %ld (%.02f%%)\n",(i == DICT_STATS_VECTLEN-1)?">= ":"", i, clvector[i], ((float)clvector[i]/ht->size)*100);
    }
}

static unsigned int _dictStringCopyHTHashFunction(const void *key){
    return dictGenHashFunction(key, strlen(key));
}

static void *_dictStringCopyHTKeyDup(void *privdata, const void *key){
    int len = strlen(key);
    char *copy = _dictAlloc(len+1);
    //防止编译器报错的，不加报什么错不知道
    DICT_NOTUSED(privdata);
    memcpy(copy, key, len);
    copy[len] = '\0';
    return copy;
}

static void *_dictStringKeyValCopyHTValDup(void *privdata, const void *val){
    int len = strlen(val);
    char *copy = _dictAlloc(len+1);
    DICT_NOTUSED(privdata);
    memcpy(copy, val, len);
    copy[len] = '\0';
    return copy;
}

static int _dictStringCopyHTKeyCompare(void *privdata, const void *key1,
                                       const void *key2)
{
    DICT_NOTUSED(privdata);
    return strcmp(key1, key2) == 0;
}

static void _dictStringCopyHTKeyDestructor(void *privdata, void *key)
{
    DICT_NOTUSED(privdata);
    _dictFree((void*)key); /* ATTENTION: const cast */
}

static void _dictStringKeyValCopyHTValDestructor(void *privdata, void *val)
{
    DICT_NOTUSED(privdata);
    _dictFree((void*)val); /* ATTENTION: const cast */
}

dictType dictTypeHeapStringCopyKey = {
        _dictStringCopyHTHashFunction,        /* hash function */
        _dictStringCopyHTKeyDup,              /* key dup */
        NULL,                               /* val dup */
        _dictStringCopyHTKeyCompare,          /* key compare */
        _dictStringCopyHTKeyDestructor,       /* key destructor */
        NULL                                /* val destructor */
};

dictType dictTypeHeapStrings = {
        _dictStringCopyHTHashFunction,        /* hash function */
        NULL,                               /* key dup */
        NULL,                               /* val dup */
        _dictStringCopyHTKeyCompare,          /* key compare */
        _dictStringCopyHTKeyDestructor,       /* key destructor */
        NULL                                /* val destructor */
};

dictType dictTypeHeapStringCopyKeyValue = {
        _dictStringCopyHTHashFunction,        /* hash function */
        _dictStringCopyHTKeyDup,              /* key dup */
        _dictStringKeyValCopyHTValDup,        /* val dup */
        _dictStringCopyHTKeyCompare,          /* key compare */
        _dictStringCopyHTKeyDestructor,       /* key destructor */
        _dictStringKeyValCopyHTValDestructor, /* val destructor */
};