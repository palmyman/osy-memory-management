#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <cmath>
using namespace std;
#endif /* __PROGTEST__ */

struct BlockOfMemory
{
	uint8_t * StartAllocAddr;	// adresa, kde zacina struct
	size_t    memSizeAlloc;	    // velikost, kolik struct obhospodaruje mista
	bool	  isVolny;			// pokud true, tak muzu ukladat.. false je zabrano daty
	BlockOfMemory *next;
	BlockOfMemory *prev;
};

int 	      pocetAlokovanychBloku;	// pocet struktur
int 		  velikostVolneho;			// velikost dostupneho mista
int 		  velikostKrabice;
BlockOfMemory *begin;

void   HeapInit    ( void * memPool, int memSize ) {
	/* volana pri inicializaci, uk startAddr, udava pametovou adresu bloku,
	kde budu hospodarit, size velikost bloku v bajtech */
	// struct se nachazi na adrese memPool az sizeof(BlockOfMemory)
	begin = (BlockOfMemory *) (memPool);
	velikostVolneho = ( memSize - sizeof(BlockOfMemory) );
	velikostKrabice = sizeof(BlockOfMemory);
	begin->memSizeAlloc = velikostVolneho;
	begin->prev = begin->next = NULL;
	begin->StartAllocAddr = (uint8_t*) memPool;
	pocetAlokovanychBloku = 1;
	begin->isVolny = true;
}

void * HeapAlloc   ( int    size ) {
	/* vstupni blok pro alokaci v halde, param elikost pozadovaneho bloku a vraci ptr na prostor v pameti.
		jinak return NULL. Pokud dojde vycerpani */
	BlockOfMemory * tmp = begin;
	BlockOfMemory * novy;

	while (tmp != NULL) {
		if ( (tmp->isVolny == true) && (tmp->memSizeAlloc >= (size_t)(size + velikostKrabice)) && (tmp->next != NULL))  {
			novy = (BlockOfMemory*)(((char*)tmp->StartAllocAddr ) + size + sizeof(BlockOfMemory));
			novy->StartAllocAddr = (uint8_t*)novy;
			novy -> prev = tmp;//
			novy -> next = tmp -> next;//
			tmp -> next = novy; //
			novy -> next ->  prev = novy; //

			novy -> memSizeAlloc = tmp-> memSizeAlloc - size - velikostKrabice;
			novy -> isVolny = true;

			tmp -> memSizeAlloc = size;
			tmp -> isVolny = false;

			return 	(void*)(BlockOfMemory*)(((char*)tmp->StartAllocAddr) + velikostKrabice);
		}
		else if ( (tmp->isVolny == true) && ( tmp->memSizeAlloc >= (size_t)(size + velikostKrabice)) && (tmp->next == NULL)) {
			novy = (BlockOfMemory*)(((char*)tmp->StartAllocAddr ) + size + sizeof(BlockOfMemory));
			novy->StartAllocAddr = (uint8_t*)novy;
			novy -> next = NULL;
			novy -> prev = tmp;
			novy->memSizeAlloc = tmp->memSizeAlloc - size - velikostKrabice;
			novy->isVolny = true;
			tmp->next = novy;
			tmp->memSizeAlloc = size;
			tmp->isVolny = false;
			return (void*)(BlockOfMemory*)(((char*)tmp->StartAllocAddr ) + velikostKrabice);
		}
	/*	else if ( (tmp->isVolny == true) && ( tmp->memSizeAlloc >= (size) )) { // vrazim to na doraz
			novy = (BlockOfMemory*)(((char*)tmp->StartAllocAddr ) + size + sizeof(BlockOfMemory));
			novy->StartAllocAddr = (uint8_t*)novy;
			tmp->next = NULL;
			tmp->memSizeAlloc = tmp->memSizeAlloc;
			tmp->isVolny = false;
			return (void*)(BlockOfMemory*)(((char*)tmp->StartAllocAddr) + velikostKrabice);
		}*/
		tmp = tmp ->next;
	}
	return NULL;
}

bool   HeapFree    ( void * blk ) {
	BlockOfMemory *tmp = begin;

	while (tmp != NULL) {
		if ( ( (tmp->StartAllocAddr + velikostKrabice) == blk) && (tmp->isVolny == false) ) {
			tmp->isVolny = true;

			if ( (tmp->next != NULL) && (tmp->next->isVolny == true)) {
				tmp->memSizeAlloc = tmp -> next -> memSizeAlloc + tmp->memSizeAlloc + velikostKrabice; // ok
				tmp-> next = tmp -> next -> next;
				if (tmp -> next)
					tmp -> next -> prev = tmp;
				/*if (tmp->next->next != NULL) {
					tmp = tmp->next->next->prev;
					tmp->next = tmp->next->next;
				}
				else
					tmp->next = NULL;*/
			}

			if ( (tmp->prev != NULL) && (tmp->prev->isVolny == true)) {
				tmp->prev->memSizeAlloc = tmp ->prev ->memSizeAlloc + tmp->memSizeAlloc + velikostKrabice;
				tmp -> prev -> next = tmp -> next;
				if ( tmp -> next )
					tmp -> next ->prev = tmp -> prev; 
				/*	if (tmp->prev->prev != NULL) {
						tmp->prev->next = tmp -> next; // ok
						if (tmp->next != NULL)
							tmp->prev = tmp->next->prev; // ok
						else
							tmp->prev = NULL;
					}
					else {
						tmp->prev->prev = NULL;
					}*/

			}
			return true;
		}
		tmp = tmp -> next;
	}
	return false;
}

void   HeapDone ( int  * pendingBlk ) {
	/*   Funkce HeapDone, bude zavolána na konci používání haldy. Funkce zjistí, kolik bloků paměti na haldě ještě zůstalo alokovaných, tuto informaci předá volajícímu prostřednictvím výstupního parametru pendingBlk.*/
	BlockOfMemory * tmp = begin;
	int velikost = 0;
	while (tmp != NULL) {
		if ((tmp->isVolny == false)) {
			velikost++;
		}
		tmp = tmp->next;
	}
	*pendingBlk = velikost;
}


#ifndef __PROGTEST__
int main ( void )
{
	uint8_t       * p0, *p1, *p2, *p3, *p4;
	int             pendingBlk;
	static uint8_t  memPool[3 * 1048576];

	HeapInit ( memPool, 2097152 );
	assert ( ( p0 = (uint8_t*) HeapAlloc ( 512000 ) ) != NULL );
	memset ( p0, 0, 512000 );
	assert ( ( p1 = (uint8_t*) HeapAlloc ( 511000 ) ) != NULL );
	memset ( p1, 0, 511000 );
	assert ( ( p2 = (uint8_t*) HeapAlloc ( 26000 ) ) != NULL );
	memset ( p2, 0, 26000 );
	HeapDone ( &pendingBlk );
	assert ( pendingBlk == 3 );


	HeapInit ( memPool, 2097152 );
	assert ( ( p0 = (uint8_t*) HeapAlloc ( 1000000 ) ) != NULL );
	memset ( p0, 0, 1000000 );
	assert ( ( p1 = (uint8_t*) HeapAlloc ( 250000 ) ) != NULL );
	memset ( p1, 0, 250000 );
	assert ( ( p2 = (uint8_t*) HeapAlloc ( 250000 ) ) != NULL );
	memset ( p2, 0, 250000 );
	assert ( ( p3 = (uint8_t*) HeapAlloc ( 250000 ) ) != NULL );
	memset ( p3, 0, 250000 );
	assert ( ( p4 = (uint8_t*) HeapAlloc ( 50000 ) ) != NULL );
	memset ( p4, 0, 50000 );
	assert ( HeapFree ( p2 ) );
	assert ( HeapFree ( p4 ) );
	assert ( HeapFree ( p3 ) );
	assert ( HeapFree ( p1 ) );
	assert ( ( p1 = (uint8_t*) HeapAlloc ( 500000 ) ) != NULL );
	memset ( p1, 0, 500000 );
	assert ( HeapFree ( p0 ) );
	assert ( HeapFree ( p1 ) );
	HeapDone ( &pendingBlk );
	assert ( pendingBlk == 0 );


	HeapInit ( memPool, 2359296 );
	assert ( ( p0 = (uint8_t*) HeapAlloc ( 1000000 ) ) != NULL );
	memset ( p0, 0, 1000000 );
	assert ( ( p1 = (uint8_t*) HeapAlloc ( 500000 ) ) != NULL );
	memset ( p1, 0, 500000 );
	assert ( ( p2 = (uint8_t*) HeapAlloc ( 500000 ) ) != NULL );
	memset ( p2, 0, 500000 );
	assert ( ( p3 = (uint8_t*) HeapAlloc ( 500000 ) ) == NULL );
	assert ( HeapFree ( p2 ) );
	assert ( ( p2 = (uint8_t*) HeapAlloc ( 300000 ) ) != NULL );
	memset ( p2, 0, 300000 );
	assert ( HeapFree ( p0 ) );
	assert ( HeapFree ( p1 ) );
	HeapDone ( &pendingBlk );
	assert ( pendingBlk == 1 );


	HeapInit ( memPool, 2359296 );
	assert ( ( p0 = (uint8_t*) HeapAlloc ( 1000000 ) ) != NULL );
	memset ( p0, 0, 1000000 );
	assert ( ! HeapFree ( p0 + 1000 ) );
	HeapDone ( &pendingBlk );
	assert ( pendingBlk == 1 );


	return 0;
}
#endif /* __PROGTEST__ */