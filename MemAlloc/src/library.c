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
	struct f_block *prev;// pointer to previous block
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

	if(size <=0){ // requesting no space?
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

	return (free+1); // return pointer to region after the meta information of the block
}

//return the block pointer
blocks *get_block_ptr(void *ptr){

	return ptr -1;
}

void my_free(void *ptr){
	 if (!ptr) {
	    return;
	  }
	 blocks *blk = get_block_ptr(ptr);

	 assert(blk == NULL);
	 assert(blk->address == 0x77777777 || blk->address == 0x12345678);
	 blk->inUse = false;
	 blk->address = 0x55555555;


}

// to reset the memory
void *calloc(size_t nelem, size_t elsize) {
  size_t size = nelem * elsize;
  void *ptr = malloc(size);
  memset(ptr, 0, size);
  return ptr;
}

// to reallocate memory
void *realloc(void *ptr, size_t size) {
  if (!ptr) {
    // NULL ptr. realloc should act like malloc.
    return my_malloc(size);
  }

  blocks *block_ptr = get_block_ptr(ptr);

  if (block_ptr->length >= size) {
    // We have enough space. Could free some once we implement split.
    return ptr;
  }

  // Need to really realloc. Malloc new space and free old space.
  // Then copy old data to new space.
  void *new_ptr;
  new_ptr = my_malloc(size);
  if (!new_ptr) {
    return NULL; //TODO: set err on failure.
  }
  memcpy(new_ptr, ptr, block_ptr->length);
  my_free(ptr);
  return new_ptr;
}

int main()
{
	return 0;
}
