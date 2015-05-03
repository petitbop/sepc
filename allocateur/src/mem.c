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
#include <math.h>
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

void* mem_alloc(unsigned long size)
{
    void** curr;
    void* next;
    /*  ecrire votre code ici */
    if (size > (1 << BUDDY_MAX_INDEX)){
        return NULL;
    }
    if (size < sizeof(void *)){
        return NULL;
    }
    int exp = (int) log2((double) size);
    if(tzl[exp] != NULL){
        curr = (void**)tzl[exp];
        next = *curr;
        tzl[exp] = next;
        return (void*) curr;
    }
    return 0;  
}

int mem_free(void *ptr, unsigned long size)
{
    /* ecrire votre code ici */
    return 0;
}


int mem_destroy()
{
    /* ecrire votre code ici */

    free(zone_memoire);
    zone_memoire = 0;
    return 0;
}

#endif
