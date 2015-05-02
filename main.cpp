#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <cmath>
using namespace std;
#endif /* __PROGTEST__ */

struct Block {
    uint8_t * adress;
    size_t size;
    bool isBusy;
    Block * prev, * next;    
    static Block * firstBlock;
};

Block * Block::firstBlock;

void HeapInit(void * memPool, int memSize) {
    Block::firstBlock = (Block *) (memPool);    

    Block::firstBlock->size = (memSize - sizeof (Block));
    Block::firstBlock->prev = Block::firstBlock->next = NULL;
    Block::firstBlock->adress = (uint8_t *) memPool;
    Block::firstBlock->isBusy = false;
}

void * HeapAlloc(int size) {
    Block * newBlock;

    for (Block * iter = Block::firstBlock; iter != NULL; iter = iter->next) {        
        if ((iter->isBusy == false) && (iter->size >= (size_t) (size + sizeof (Block)))) {
            newBlock = (Block *) (((uint8_t*) iter->adress) + size + sizeof (Block));
            newBlock->adress = (uint8_t*) newBlock;
            newBlock->next = iter->next;
            newBlock->prev = iter;   
            
            if(iter->next != NULL)
                newBlock->next->prev = newBlock;
            
            newBlock->size = iter->size - size - sizeof (Block);
            newBlock->isBusy = false;
            
            iter->next = newBlock;
            iter->size = size;
            iter->isBusy = true;
            return (void*) (Block*) (((uint8_t*) iter->adress) + sizeof (Block));
        }
    }
    return NULL;
}

bool HeapFree(void * blk) {
    for (Block * iter = Block::firstBlock; iter != NULL; iter = iter->next) {
        if(iter->adress + sizeof(Block) == blk && iter->isBusy) {
            iter->isBusy = false;
            
            if(iter->next != NULL && !iter->next->isBusy) {
                iter->size = iter->next->size + iter->size + sizeof(Block);
                iter->next = iter->next->next;
                
                if(iter->next != NULL)
                    iter->next->prev = iter;
            }
            
            if(iter->prev != NULL && !iter->prev->isBusy) {
                iter->prev->size = iter->prev->size + iter->size + sizeof(Block);
                iter->prev->next = iter->next;
                
                if(iter->next != NULL)
                    iter->next->prev = iter->prev;
            }
            return true;
        }
    }    
    return false;
}

void HeapDone(int * pendingBlk) {
    int counter = 0;
    for (Block * iter = Block::firstBlock; iter != NULL; iter = iter->next) {
        if (iter->isBusy)
            counter++;
    }    
    * pendingBlk = counter;
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