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

#define BLOCK__META_SIZE sizeof(blocks)

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
int policy = 1;// the memory allocation policy to be used

/*stats*/
//total free space and largest contigous free space
// total number of bytes allocated
int totalBytes = 0,totalFree = 0, largestCont = 0;

//split blocks, returns 1 on successful split and -1 on unsuccessful split
blocks *splitBlocks(blocks *split, int size)
{

	blocks *newBlock = NULL;
	split = global_base;

	if(split->next != NULL){
		if(split->inUse == false){
			if(split->length  > sizeof(size + BLOCK__META_SIZE )){
				split->length -= sizeof(size + BLOCK__META_SIZE);
				newBlock->next = split->next->next;
				split->next = newBlock;
			}
		}
	}

	return split;

}

blocks *get_block_ptr(void *ptr) {
  return (blocks*)ptr - 1;
}

blocks *best_fit(blocks *current,int size){
	int min = size;
	blocks *minimum = NULL;

	while(current->next != NULL){
		if(current->length < min && !current->inUse){
			min = size;
			minimum = current;
		}
		current = current->next;
	}


	return minimum;
}

blocks *first_fit(blocks *current, int size){

	while(current && !(current->inUse && current->length >= size)){
			current = current->next;
		}

	return current;
}

blocks *find_free_block(blocks **last,int size){

	blocks *current = global_base;

	if(policy == 1){
		current = first_fit(current,size);
	}
	else{
		current = best_fit(current,size);
	}

	return current;
}

// to search for free space
blocks *request_space(blocks *last, int size){

	blocks *blk;

	blk = sbrk(0);

	void *request = sbrk(size + BLOCK__META_SIZE);

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

//set a new policy
void my_mallop(int newPolicy){
	policy = newPolicy;
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
			free = splitBlocks(free,size);
			free->inUse = true;
			free->address = 0x77777777;
		}
	}
	totalBytes+= size;// increment total bytes
	return (free+1); // return pointer to region after the meta information of the block
}

//to free blocks
void my_free(void *ptr){
	 if (!ptr) {
	    return;//TODO: RETURN ERROR
	  }

	 blocks *blk = get_block_ptr(ptr);


	 assert(blk->inUse == NULL);
	 assert(blk->address == 0x77777777 || blk->address == 0x12345678);
	 blk->inUse = false;
	 blk->address = 0x55555555;// for testing

	 printf("\n memory freed");
	 totalFree+= sizeof(*ptr);

	 mergeBlocks(blk);// merges the blocks

}

//for merging free blocks
void mergeBlocks(blocks *merge)
{
	merge = global_base;//start from head

	while(merge->next != NULL){
		 if(merge->next->inUse == false && merge->inUse == false){
			 merge->length+= merge->next->length;
			 merge->next = merge->next->next;
		 }
	 }
	largestCont+= sizeof(merge->length);

}

void my_mallinfo(){

	printf("\n total allocated bytes %i",totalBytes);
	printf("\n total free  %i",totalBytes);
	printf("\n total contiguous %i",largestCont);

}
// for testing
int main()
{
	int *ptr_one,newPolicy;
	void *ptr_two;

	printf("\n press 0 to use best fit policy  ");
	scanf("%i",&newPolicy);

	if(newPolicy == 0){
		my_mallop(newPolicy);
	}

	ptr_one = (int *)my_malloc(sizeof(int));
	ptr_two = (void *) my_malloc(400);

	if (ptr_one == 0)
	{
		printf("ERROR: Out of memory\n");
		return 1;
	}

	*ptr_one = 25;
	printf("%d\n", *ptr_one);

	my_free(ptr_one);
	my_free(ptr_two);

	my_mallinfo();

	return 0;
}
