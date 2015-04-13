/*
 * library.c
 *
 *  Created on: Mar 30, 2015
 *      Author: cnehem
 */

#include "mem_alloc_api.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

#define BLOCK_SIZE sizeof(blocks)

typedef struct f_block{
	int length;// length of block
	bool inUse;// whether block is free or not
	struct f_block *next;// pointer to next block
	int address;//address where it located
}blocks;

extern char *my_malloc_error;

/*global variables*/
void *global_base = NULL;// pointer to the head
int frag;// value representing threshold of internal fragmentation
int newPolicy;// the memory allocation policy to be used

blocks *find_free_block(blocks **last,int size){

	blocks *current = global_base;

	while(current && !(current->inUse && current->length >= size)){
		*last = current;
		current = current->next;
	}

	return current;
}

// to search for free space
blocks *request_space(blocks *last, int size){

	blocks *blk;

	blk = sbrk(0);

	void *request = sbrk(size + BLOCK_SIZE);

	assert((void*)blk == request);

	if (request == (void*) -1) {
	    return NULL; // sbrk failed.
	  }

	  if (last) { // NULL on first request.
	    last->next = blk;
	  }
	  blk->length = size;
	  blk->next = NULL;
	  blk->inUse = false;
	  blk->address = 0x12345678;

	  return blk;
}

// allocate memory
void *my_malloc(int size){

	blocks *free;//list of free blocks

	if(size <= 0){ // requesting no space?
		return NULL; //TODO: set error
	}

	if(!global_base){

		free = request_space(NULL,size);
		if(!free){ // unable to allocate space
			return NULL;//TODO: set error
		}
		global_base = free;
	}
	else
	{
		blocks *last = global_base;
		free = find_free_block(&last,size);
		if(!free){
			free = request_space(last,size);
			if(!free){
				//TODO: set error
				return NULL; // space could not be allocated
			}
		}
		else // free block found
		{
			free->inUse = true;
			free->address = 0x77777777;
		}
	}

	return free; // return pointer to region after the meta information of the block
}

void my_free(void *ptr){
	 if (!ptr) {
	    return;
	  }

	 blocks *blk = ptr;

	 assert(blk->inUse == NULL);
	 assert(blk->address == 0x77777777 || blk->address == 0x12345678);
	 blk->inUse = false;
	 blk->address = 0x55555555;


}

// for testing
int main()
{
	int *ptr_one;

	ptr_one = (int *)my_malloc(sizeof(int));

	if (ptr_one == 0)
	{
		printf("ERROR: Out of memory\n");
		return 1;
	}

	*ptr_one = 25;
	printf("%d\n", *ptr_one);

	my_free(ptr_one);

	return 0;
}
