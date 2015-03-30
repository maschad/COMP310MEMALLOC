/*
 * mem_alloc_api.h
 *
 *  Created on: Mar 30, 2015
 *      Author: cnehem
 */

#ifndef MEM_ALLOC_API_H_
#define MEM_ALLOC_API_H_
#endif /* MEM_ALLOC_API_H_ */

void *my_malloc(int size);/*returns the void the ptr that can be assigned to any c Pointer*/

extern char *my_malloc_error;/*error handling mechanism for malloc*/

void my_free(void *ptr);/*deallocates the block of memory pointed to by ptr*/

void my_mallopt(int policy);/*determines the policy to be used, whether best fit or first fit*/

void my_mallinfo();/*function to print stats about mem allocation that has been performed so far*/
