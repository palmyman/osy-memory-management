/* 
 * File:   main.cpp
 * Author: palmyman
 *
 * Created on April 30, 2015, 3:48 PM
 */

#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <cmath>
using namespace std;
#endif /* __PROGTEST__ */

#define KB 1024;
#define MB 1048576;

class Block {
    static int blockCount, heapMaxSize;
    static uint8_t * heap;
    
    static uint8_t * first, * last;
    static int firstIndex, lastIndex;
    
    int size, prevIndex, nextIndex;
    uint8_t * data;
public:
    Block(int size, uint8_t * data, int prevIndex, int nextIndex);
    ~Block();
    static void InitStatic(void * memPool, int memSize);
    
    int GetBlockCount() const;
    int GetHeapSize() const;
    uint8_t * GetHeap() const;
    uint8_t * GetHeapEnd() const;
    
    uint8_t * GetData() const;
    int GetNext() const;
    int GetPrev() const;
    int GetSize() const;    
};

Block::Block(int size, uint8_t * data, int prev, int next) {
    this->size = size;
    this->data = data;
    this->prevIndex = prevIndex;
    this->nextIndex = nextIndex;
    blockCount++;
}

Block::~Block() {
    blockCount--;
}

void Block::InitStatic(void * memPool, int memSize) {
    blockCount = 0;
    heapMaxSize = memSize;
    heap = static_cast<uint8_t *>(memPool);
    
    first = last = NULL;    
    firstIndex = lastIndex = 0;
}

int Block::GetBlockCount() const {
    return blockCount;
}

uint8_t * Block::GetHeap() const {
    return heap;
}

uint8_t * Block::GetHeapEnd() const {
    return heap + heapMaxSize;
}

uint8_t * Block::GetData() const {
    return data;
}

int Block::GetNext() const {
    return nextIndex;
}

int Block::GetPrev() const {
    return prevIndex;
}

int Block::GetSize() const {
    return size;
}

void HeapInit(void * memPool, int memSize) {
    Block::InitStatic(memPool, memSize);
}

void * HeapAlloc(int size) {
    void *p = NULL;
    return p;
}

bool HeapFree(void * blk) {
    /* todo */
    return true;
}

void HeapDone(int * pendingBlk) {
    /* todo */
}

#ifndef __PROGTEST__

int main(void) {
    uint8_t * p0, *p1, *p2, *p3, *p4;
    int pendingBlk;
    static uint8_t memPool[3 * 1048576];

    HeapInit(memPool, 2097152);
    assert((p0 = (uint8_t*) HeapAlloc(512000)) != NULL);
    memset(p0, 0, 512000);
    assert((p1 = (uint8_t*) HeapAlloc(511000)) != NULL);
    memset(p1, 0, 511000);
    assert((p2 = (uint8_t*) HeapAlloc(26000)) != NULL);
    memset(p2, 0, 26000);
    HeapDone(&pendingBlk);
    assert(pendingBlk == 3);


    HeapInit(memPool, 2097152);
    assert((p0 = (uint8_t*) HeapAlloc(1000000)) != NULL);
    memset(p0, 0, 1000000);
    assert((p1 = (uint8_t*) HeapAlloc(250000)) != NULL);
    memset(p1, 0, 250000);
    assert((p2 = (uint8_t*) HeapAlloc(250000)) != NULL);
    memset(p2, 0, 250000);
    assert((p3 = (uint8_t*) HeapAlloc(250000)) != NULL);
    memset(p3, 0, 250000);
    assert((p4 = (uint8_t*) HeapAlloc(50000)) != NULL);
    memset(p4, 0, 50000);
    assert(HeapFree(p2));
    assert(HeapFree(p4));
    assert(HeapFree(p3));
    assert(HeapFree(p1));
    assert((p1 = (uint8_t*) HeapAlloc(500000)) != NULL);
    memset(p1, 0, 500000);
    assert(HeapFree(p0));
    assert(HeapFree(p1));
    HeapDone(&pendingBlk);
    assert(pendingBlk == 0);


    HeapInit(memPool, 2359296);
    assert((p0 = (uint8_t*) HeapAlloc(1000000)) != NULL);
    memset(p0, 0, 1000000);
    assert((p1 = (uint8_t*) HeapAlloc(500000)) != NULL);
    memset(p1, 0, 500000);
    assert((p2 = (uint8_t*) HeapAlloc(500000)) != NULL);
    memset(p2, 0, 500000);
    assert((p3 = (uint8_t*) HeapAlloc(500000)) == NULL);
    assert(HeapFree(p2));
    assert((p2 = (uint8_t*) HeapAlloc(300000)) != NULL);
    memset(p2, 0, 300000);
    assert(HeapFree(p0));
    assert(HeapFree(p1));
    HeapDone(&pendingBlk);
    assert(pendingBlk == 1);


    HeapInit(memPool, 2359296);
    assert((p0 = (uint8_t*) HeapAlloc(1000000)) != NULL);
    memset(p0, 0, 1000000);
    assert(!HeapFree(p0 + 1000));
    HeapDone(&pendingBlk);
    assert(pendingBlk == 1);


    return 0;
}
#endif /* __PROGTEST__ */