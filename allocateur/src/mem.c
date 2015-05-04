/*****************************************************
 * Copyright Grégory Mounié 2008-2013                *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#ifndef MEM_C
#define MEM_C

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mem.h"

/** squelette du TP allocateur memoire */

void *zone_memoire = 0;

/* ecrire votre code ici */
void* tzl[BUDDY_MAX_INDEX + 1];

int mem_init()
{
    if (! zone_memoire)
        zone_memoire = (void *) malloc(ALLOC_MEM_SIZE);
    if (zone_memoire == 0)
    {
        perror("mem_init:");
        return -1;
    }

    /* ecrire votre code ici */
    for(int i = 0; i < BUDDY_MAX_INDEX; i++){
        tzl[i] = NULL;
    }
    tzl[BUDDY_MAX_INDEX] = zone_memoire;
    (*(void**)zone_memoire) = NULL;

    return 0;
}



static int exponent(int x){
    int exp = 0;
    assert(x > 0);
    while(x != 1){
        x /= 2;
        exp += 1;
    }
    return exp;
}

static void* comp(void* addr, int size){
    int i = exponent(size);
    return (void*) (((int)(addr - zone_memoire) ^ (1 << i)) + zone_memoire);
}

static void* min(void* ptr1, void* ptr2){
    if((unsigned int)ptr1 < (unsigned int) ptr2){
        return ptr1;
    } else {
        return ptr2;
    }
}

static void* max(void* ptr1, void* ptr2){
    if((unsigned int)ptr1 > (unsigned int) ptr2){
        return ptr1;
    } else {
        return ptr2;
    }
}

static int is_in_tzl(void* ptr, int size){
    int exp;
    void* curr;
    void* next;
    exp = exponent((int)size);
    curr = tzl[exp];
    while(ptr != curr && curr != NULL){
        next = *(void**)curr;
        curr = next;
    }
    if(curr == NULL)
        return 0;

    return 1;
}

static int supp_from_tzl(void* ptr, int size){
    int exp;
    void* prec;
    void* curr;
    void* next;
    exp = exponent((int)size);
    prec = (void*)&tzl[exp];
    curr = tzl[exp];
    while(ptr != curr && curr != NULL){
        next = *(void**)curr;
        curr = next;
    }
    if(curr == NULL)
        perror("pointer disappeared from tzl...");

    *(void**)prec = next;
    return 0;

}

static int add_to_tzl(void* ptr, int size){
    int exp;
    void* first;
    exp = exponent(size);
    first = tzl[exp];
    *(void**)ptr = first;
    tzl[exp] = ptr;

    return 0;
}



void* mem_alloc(unsigned long size)
{
    if(! zone_memoire)
        return (void*) 0;
    void** curr;
    void* next;
    void* addr;
    void* addr_comp;
    int i;
    /*  ecrire votre code ici */
    if (size > (1 << BUDDY_MAX_INDEX)){
        return NULL;
    }
    if (size < sizeof(void *)){
        return NULL;
    }
    int exp = exponent((int) size);
    if(tzl[exp] != NULL){
        curr = (void**)(tzl[exp]);
        next = *curr;
        tzl[exp] = next;
        return (void*) curr;
    } else {
        i = exp;
        while(i <= BUDDY_MAX_INDEX && tzl[i] == NULL)
            i++;
        if(i == BUDDY_MAX_INDEX + 1)
            return (void*) 0;
        addr = tzl[i];
        tzl[i] = *(void**)addr;
        while(i != exp){
            i--;
            addr_comp = (void*) (((int)(addr - zone_memoire) ^ (1 << i)) + zone_memoire);
            next = tzl[i];
            curr = (void**)addr_comp;
            *curr = next;

            tzl[i] = addr_comp;
        }
        next = tzl[i];
        curr = (void**)addr;
        *curr = next;

        tzl[i] = addr;
        return addr;

    }
    return 0;  
}


int mem_free(void *ptr, unsigned long size)
{
    int exp;
    int i;
    void* addr;
    void* addr_comp;
    void* left_addr;
    void* right_addr;

    exp = exponent((int)size);
    i = exp;
    addr = ptr;

    while(i <= BUDDY_MAX_INDEX){
        addr_comp = comp(addr, 1<<i);
        if(i < BUDDY_MAX_INDEX && is_in_tzl(addr_comp, 1<<i)){
            left_addr = min(addr, addr_comp);
            right_addr = max(addr, addr_comp);
            supp_from_tzl(right_addr,1<<i);

            addr = left_addr;
            i++;
        } else{ 
            add_to_tzl(addr, 1<<i);
            break;
        }
    }


    return 0;
}



int mem_destroy()
{
    /* ecrire votre code ici */

    free(zone_memoire);
    zone_memoire = 0;
    return 0;
}

/*
int main(int* argv, int argc){
    int multi;
    mem_init();
    mem_alloc(1 << BUDDY_MAX_INDEX);
    printf("ok\n");
  multi = 1;
  
  //ASSERT_EQ(multi, 1);
  //ASSERT_EQ( mem_init(), 0 );
  mem_init();

  void *mref = mem_alloc(ALLOC_MEM_SIZE);
  //ASSERT_NE( mref, (void*) 0);
  //ASSERT_EQ( mem_free(mref, ALLOC_MEM_SIZE), 0 );
  mem_free(mref, ALLOC_MEM_SIZE);

  void *m1 = mem_alloc(64);
  //ASSERT_NE( m1, (void *)0 );

  void *m2 = mem_alloc(64);
  //ASSERT_NE( m2, (void *)0 );

  unsigned long vref = (unsigned long) mref;
  unsigned long v1 = (unsigned long)m1;
  unsigned long v2 = (unsigned long)m2;
  //ASSERT_EQ( (v1-vref)^(v2-vref), 64 );

  ( mem_free( m1, 64 ), 0 );
  ( mem_free( m2, 64 ), 0 );

  mref = mem_alloc(ALLOC_MEM_SIZE);
  //ASSERT_NE( mref, (void *)0 );
  ( mem_free( mref, ALLOC_MEM_SIZE ) );
  ( mem_destroy());
    return 0;
}
*/

#endif
