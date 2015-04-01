/*
 * library.c
 *
 *  Created on: Mar 30, 2015
 *      Author: cnehem
 */

#include "mem_alloc_api.h"
#define NULL 0

typedef struct blck{
	int length;// length of the block
	int mem;//memory for use by caller
}block;

typedef struct f_block{
	int length;// length of the block
	block prev;// pointer to previous block
	block next;// pointer to next block
	int rem;//amount of remaining memory
}free_blocks;

typedef struct bound{
	int length;// length of tag
	int inUse;// 1 for in use , 0 for free
}top_boundary;

typedef struct bound{
	int length;//length to indicate where the top is
}low_boundary;


/*global variables*/
top_boundary top;
low_boundary bottom;
free_blocks free;
int program_break;//location of the program break


void *my_malloc(int size){
	while(free.next != NULL){

		if(size > free.length){
			free = free.next;
		}
		else{
			free.prev.length = size;
			sbrk(size);
		}
	}
}

void my_free(void *ptr){

	if(*ptr == NULL)
	{
		return;
	}
	if(top.length > 128){
		sbrk(top.length);
	}
}
