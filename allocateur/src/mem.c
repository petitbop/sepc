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

void* mem_alloc(unsigned long size)
{
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
    void* prec;
    void* curr;
    void* next;
    exp = exponent((int)size);
    curr = tzl[exp];
    prec = (void*)(&tzl[exp]);
    while(ptr != curr && curr != NULL){
        next = *(void**)curr;
        prec = curr;
        curr = next;
    }
    if(curr == NULL)
        return 1;
    next = *(void**) curr;
    *(void**) prec = next;

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
