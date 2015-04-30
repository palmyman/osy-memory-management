#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <cmath>

// following headers are only for testing (not present for compilation at Progtest)
#include <iostream>
#include <set>
#include <algorithm>
#include <vector>

using namespace std;
#endif /* __PROGTEST__ */

//------------------------------------- BEGGINING OF MY CODE -------------------------------------

#define PTR_TYPE uint8_t
#define KB 1024
#define MB 1024*1024

struct BlockInfo{
   int size;
   PTR_TYPE * data;
   int prevIndex;
   int nextIndex;

   void set(int size, PTR_TYPE * data, int prevIndex, int nextIndex){
      this->size = size;
      this->data = data;
      this->prevIndex = prevIndex;
      this->nextIndex = nextIndex;
   }
};

//------------------------------------- GLOBAL VARIABLES -------------------------------------

int GLOB_BlockCount;
int GLOB_HeapMaxSize;
PTR_TYPE * GLOB_Heap;
PTR_TYPE * GLOB_HeapEnd;
BlockInfo * GLOB_BlockTable;
BlockInfo * GLOB_BlockTableEnd;

PTR_TYPE * GLOB_FirstElem;
int GLOB_FirstElemIndex;

PTR_TYPE * GLOB_LastElem;
int GLOB_LastElemIndex;

//------------------------------------- SUPPORT FUNCTIONS -------------------------------------

void setPreviousToBlock(int changedBlock, int prev){
   if (changedBlock != -1){
      BlockInfo * activeBlock = ((BlockInfo *)GLOB_BlockTable)+changedBlock;
      activeBlock->prevIndex=prev;
   }
}

void setNextToBlock(int changedBlock, int next){
   if (changedBlock != -1){
      BlockInfo * activeBlock = ((BlockInfo *)GLOB_BlockTable)+changedBlock;
      activeBlock->nextIndex=next;
   }
}

// Change a position of a record in the block table and change accordingly records referencing to this item
void moveBlock(int destination, int source){
   if (destination==source)
      return;
   const int prev = GLOB_BlockTable[source].prevIndex;
   const int next = GLOB_BlockTable[source].nextIndex;

   setPreviousToBlock(next,destination);
   setNextToBlock(prev,destination);

   GLOB_BlockTable[destination]=GLOB_BlockTable[source];

   if (source == GLOB_FirstElemIndex)
      GLOB_FirstElemIndex = destination;
   if (source == GLOB_LastElemIndex)
      GLOB_LastElemIndex = destination;
}

// Alocates a block to specified area of memory and sets the attributes to the block table
void * addBlock(PTR_TYPE * location, int size, int prevBlockIndex = -1){
   const int prev = prevBlockIndex;
   const int next = (prev == -1) ? GLOB_FirstElemIndex : GLOB_BlockTable[prev].nextIndex;
   const int index = GLOB_BlockCount;

   const bool isFirst = prev == -1;
   const bool isLast = next == -1;

   if (isFirst){
      GLOB_FirstElem = location;
      GLOB_FirstElemIndex = index;
   }else{
      GLOB_BlockTable[prev].nextIndex = index;
   }

   if (isLast){
      GLOB_LastElem = location;
      GLOB_LastElemIndex = index;
   }else{
      GLOB_BlockTable[next].prevIndex = index;
   }

   GLOB_BlockTable[index].set(size, location, prev, next);
   GLOB_BlockCount++;
   return location;
}

//------------------------------------- GIVEN FUNCTIONS -------------------------------------

// Initalize the all the requered global variables
void HeapInit(void * memPool, int memSize){
   GLOB_BlockCount = 0;
   GLOB_HeapMaxSize = memSize;
   GLOB_Heap = reinterpret_cast<PTR_TYPE *>(memPool);
   GLOB_HeapEnd = (reinterpret_cast<PTR_TYPE *>(memPool))+memSize;

   GLOB_BlockTable = reinterpret_cast<BlockInfo *>(memPool);
   GLOB_BlockTableEnd = reinterpret_cast<BlockInfo *>(memPool);
   
   GLOB_FirstElem = NULL;
   GLOB_FirstElemIndex = -1;

   GLOB_LastElem = NULL;
   GLOB_LastElemIndex = -1;
}

void * HeapAlloc(int size){
   // Ckeck whether there is enough space for another record
   PTR_TYPE * NewBlockTableEnd = reinterpret_cast<PTR_TYPE *>(GLOB_BlockTable+(GLOB_BlockCount+1));
   if (((GLOB_FirstElem != NULL) ? GLOB_FirstElem : reinterpret_cast<PTR_TYPE *>(GLOB_HeapEnd)) - NewBlockTableEnd < 0)
      return NULL;

   // Iterate over all block and check, whether we can fit the block between it and the following block
   PTR_TYPE * endOfPotentialBlock = GLOB_LastElem;
   int blockIndex = GLOB_LastElemIndex;
   while(blockIndex != -1){
      PTR_TYPE * previousBlockEnd = GLOB_BlockTable[blockIndex].data+size;

      if (endOfPotentialBlock-previousBlockEnd >= size)
         return addBlock(endOfPotentialBlock-size,size,blockIndex);

      endOfPotentialBlock= GLOB_BlockTable[blockIndex].data;
      blockIndex=GLOB_BlockTable[blockIndex].prevIndex;
   }

   // The propriate area might be before the first block
   PTR_TYPE * freeSpaceEnd = (GLOB_FirstElem != NULL ? GLOB_FirstElem : GLOB_HeapEnd);
   if (freeSpaceEnd - NewBlockTableEnd >= size){
      return addBlock(freeSpaceEnd-size,size);
   }

   // No space with sufficient size has been found
   return NULL;
}

bool HeapFree(void * blk){
   // Iterates over the record in order to find block at this address
   for (int i = 0; i < GLOB_BlockCount; i++){
      if (GLOB_BlockTable[i].data == blk){
         const int prev = GLOB_BlockTable[i].prevIndex;
         const int next = GLOB_BlockTable[i].nextIndex;

         setPreviousToBlock(next,prev);
         setNextToBlock(prev,next);

         if(GLOB_FirstElemIndex == i){
            GLOB_FirstElemIndex = next;
            GLOB_FirstElem = (next != -1 ? GLOB_BlockTable[next].data : NULL);
         }

         if(GLOB_LastElemIndex == i){
            GLOB_LastElemIndex = prev;
            GLOB_LastElem = (prev != -1 ? GLOB_BlockTable[prev].data : NULL);
         }

         // Fill the gap by the last record in the table
         moveBlock(i,GLOB_BlockCount-1);

         GLOB_BlockCount--;
         return true;
      }
   }
   //cout << "nothing" << endl;
   return false;
}

void HeapDone(int * pendingBlk){
   *pendingBlk = GLOB_BlockCount;
}

//------------------------------------- TESTS -------------------------------------
 
#ifndef __PROGTEST__
void subtestValidity(){
   set<int> prevSet, nextSet;
   vector<int> prevVec, nextVec;
   int prev = GLOB_FirstElemIndex;
   int next = GLOB_LastElemIndex;
   while(prev != -1){
      assert(prevSet.count(prev) == 0);
      prevSet.insert(prev);
      prevVec.push_back(prev);
      prev=GLOB_BlockTable[prev].nextIndex;
   }

   while(next != -1){
      assert(nextSet.count(next) == 0);
      nextSet.insert(next);
      nextVec.push_back(next);
      next=GLOB_BlockTable[next].prevIndex;
   }

   assert(prevSet == nextSet);
   reverse(nextVec.begin(),nextVec.end());
   assert(nextVec == nextVec);
}

void test1(){
   uint8_t *p1, *p2;
   uint8_t  memPool[MB];
   HeapInit(memPool, MB);
   bool freed;
   int pendingBlocks;

   assert(GLOB_FirstElemIndex == -1);
   assert(GLOB_LastElemIndex == -1);
   assert(GLOB_FirstElem == NULL);
   assert(GLOB_LastElem == NULL);
   HeapDone(&pendingBlocks);
   assert(pendingBlocks == 0);
   subtestValidity();

   p1 = reinterpret_cast<uint8_t*>(HeapAlloc(KB));
   assert(p1 != NULL);
   assert(GLOB_FirstElemIndex != -1);
   assert(GLOB_LastElemIndex != -1);
   assert(GLOB_FirstElem != NULL);
   assert(GLOB_LastElem != NULL);
   assert(GLOB_LastElemIndex == GLOB_FirstElemIndex);
   assert(GLOB_LastElem == GLOB_FirstElem);
   HeapDone(&pendingBlocks);
   assert(pendingBlocks == 1);
   subtestValidity();

   p2 = reinterpret_cast<uint8_t*>(HeapAlloc(KB));
   assert(p2 != NULL);
   assert(GLOB_FirstElemIndex != -1);
   assert(GLOB_LastElemIndex != -1);
   assert(GLOB_FirstElem != NULL);
   assert(GLOB_LastElem != NULL);
   assert(GLOB_LastElemIndex != GLOB_FirstElemIndex);
   assert(GLOB_LastElem != GLOB_FirstElem);
   HeapDone(&pendingBlocks);
   assert(pendingBlocks == 2);
   subtestValidity();

   freed = HeapFree(p1);
   assert(freed);
   assert(GLOB_FirstElemIndex != -1);
   assert(GLOB_LastElemIndex != -1);
   assert(GLOB_FirstElem != NULL);
   assert(GLOB_LastElem != NULL);
   assert(GLOB_LastElemIndex == GLOB_FirstElemIndex);
   assert(GLOB_LastElem == GLOB_FirstElem);
   HeapDone(&pendingBlocks);
   assert(pendingBlocks == 1);
   subtestValidity();

   freed = HeapFree(p1);
   assert(!freed);
   HeapDone(&pendingBlocks);
   assert(pendingBlocks == 1);
   subtestValidity();

   freed = HeapFree(p2);
   assert(freed);
   assert(GLOB_FirstElemIndex == -1);
   assert(GLOB_LastElemIndex == -1);
   assert(GLOB_FirstElem == NULL);
   assert(GLOB_LastElem == NULL);
   HeapDone(&pendingBlocks);
   assert(pendingBlocks == 0);
   subtestValidity();
}

int countNumOfReachableBlocks(int initSize, int blockSize){
   uint8_t * memory = new uint8_t[initSize];
   HeapInit(memory,initSize);

   int i = 0;
   while (HeapAlloc(blockSize) != NULL)
      i++;
   return i;
}

void capacityTest(int initSize, int blockSize){
   int blockCount = countNumOfReachableBlocks(initSize, blockSize);
   int expectedCount = initSize/blockSize;
   cout << "Capacity test finished, results:" << endl;
   cout << "initSize: " << initSize << endl;
   cout << "blockSize: " << blockSize << endl;
   cout << "Reached " << expectedCount << " out of " << blockCount << " expected(" << (blockCount/expected)*100 << ")" endl;
   cout << endl;
}

int main(void){
   uint8_t       * p0, *p1, *p2, *p3, *p4;
   int             pendingBlk;
   static uint8_t  memPool[3 * 1048576];

   test1();
   capacityTest(MB,KB);
   capacityTest(MB,MB);
   capacityTest(MB,2*KB);
   capacityTest(MB*64,KB);
   capacityTest(1000,2);
 
   HeapInit(memPool, 2097152); subtestValidity();
   assert((p0 = (uint8_t*) HeapAlloc(512000)) != NULL);subtestValidity();
   memset(p0, 0, 512000);
   assert((p1 = (uint8_t*) HeapAlloc ( 511000 ) ) != NULL);subtestValidity();
   memset(p1, 0, 511000);
   assert((p2 = (uint8_t*) HeapAlloc ( 26000 ) ) != NULL);subtestValidity();
   memset(p2, 0, 26000);
   HeapDone(&pendingBlk);
   assert(pendingBlk == 3);
 
   HeapInit(memPool, 2097152 );
   assert((p0 = (uint8_t*) HeapAlloc(1000000)) != NULL);subtestValidity();
   memset(p0, 0, 1000000 );
   assert((p1 = (uint8_t*) HeapAlloc(250000)) != NULL );subtestValidity();
   memset(p1, 0, 250000 );
   assert((p2 = (uint8_t*) HeapAlloc(250000)) != NULL );subtestValidity();
   memset(p2, 0, 250000 );
   assert((p3 = (uint8_t*) HeapAlloc(250000)) != NULL );subtestValidity();
   memset(p3, 0, 250000 );
   assert((p4 = (uint8_t*) HeapAlloc(50000)) != NULL );subtestValidity();
   memset(p4, 0, 50000 );
   assert(HeapFree(p2));subtestValidity();
   assert(HeapFree(p4));subtestValidity();
   assert(HeapFree(p3));subtestValidity();
   assert(HeapFree(p1));subtestValidity();
   assert((p1 = (uint8_t*) HeapAlloc(500000)) != NULL );subtestValidity();
   memset(p1, 0, 500000);
   assert(HeapFree(p0));subtestValidity();
   assert(HeapFree(p1));subtestValidity();
   HeapDone(&pendingBlk );subtestValidity();
   assert(pendingBlk == 0);
 
   HeapInit(memPool, 2359296 );subtestValidity();
   assert((p0 = (uint8_t*) HeapAlloc(1000000)) != NULL );subtestValidity();
   memset(p0, 0, 1000000);
   assert((p1 = (uint8_t*) HeapAlloc(500000)) != NULL );subtestValidity();
   memset(p1, 0, 500000);
   assert((p2 = (uint8_t*) HeapAlloc(500000)) != NULL );subtestValidity();
   memset(p2, 0, 500000);
   assert((p3 = (uint8_t*) HeapAlloc(500000)) == NULL );subtestValidity();
   assert(HeapFree(p2));
   assert((p2 = (uint8_t*) HeapAlloc(300000)) != NULL );subtestValidity();
   memset(p2, 0, 300000);
   assert(HeapFree(p0));subtestValidity();
   assert(HeapFree(p1));subtestValidity();
   HeapDone(&pendingBlk);subtestValidity();
   assert(pendingBlk == 1 );
 
   HeapInit(memPool, 2359296);subtestValidity();
   assert((p0 = (uint8_t*) HeapAlloc(1000000)) != NULL );subtestValidity();
   memset(p0, 0, 1000000 );
   assert(!HeapFree(p0 + 1000));subtestValidity();
   HeapDone(&pendingBlk);subtestValidity();
   assert(pendingBlk == 1);
 
   cout << "Test passed" << endl;
   return 0;
 }
#endif /* __PROGTEST__ */
