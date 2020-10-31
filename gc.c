#include "memorymgr.h"
#include <stdio.h>
#include <stdlib.h>

int * firstBlock1; //header
int * sentinelBlock;


int * firstBlock(){
    return firstBlock1;

}


int * lastBlock(){
  return sentinelBlock;

}


int isAllocated(int*p){
  if ((*p%2) != 0) return 1;
  else return 0;

}


int setLength(int heaplength) {
  return heaplength - 8;

}


int * nextBlock(int *p){
  int size;
  if (*p%2 == 1) size = *p -1;
  else size = *p;
  int * next;
  next = p + (size)/4;
  return next;

}


int next_8(int size){
  while (size%8 != 0)
    size++;
  return size;

}


void initmemory (int size){
  //initial malloc
  int heaplength = next_8(size + 12);
  int * heap = malloc(heaplength);
  firstBlock1 = heap + 1;
  *firstBlock1 = setLength(heaplength);
  sentinelBlock = nextBlock(firstBlock1);
  *sentinelBlock = 0;

}


void * myalloc(int length) {
  int newlen = next_8(length + 4);
  int * ptr = firstBlock();
  int * next;
  int yup = 0;
  while ((*ptr != 0) && (yup != 1)){
    if ((isAllocated(ptr) == 0)&& ((*ptr) >= newlen)){ //do we have enough space
      yup = 1;
      next = ptr + (newlen/4); //move to next block
      *next = *ptr - newlen;
      *ptr = newlen + 1;
      return (void*) ptr;
    }
    else{
      ptr = nextBlock(ptr);
    }
  }
  if (yup == 0) return NULL;

}


void myfree(void * p){
  int * ptr = (int*)p;
  (*ptr)--;
}


void coalesce(){
  int *ptr = firstBlock();
  int * next = nextBlock(ptr);
  int x1, x2 = 0; //value holders
  while (*next != 0){
    if (isAllocated(ptr) == 0) {
      if (isAllocated(next) == 0){
        x1 = *(ptr);
        x2 = *(next);
        *ptr = x1 + x2;
        next = nextBlock(ptr);
      }
      else{
        next = nextBlock(next);
      }
    }
    else{
      ptr = nextBlock(ptr);
      next = nextBlock(ptr);
    }
  }

}

void printallocation(){
  int counter = 0;
  int * ptr = firstBlock();
  while(ptr != sentinelBlock){
    if (isAllocated(ptr) == 1 )
    printf("Block %d\t Size: %d\t Allocated \n", counter, *ptr-1);
    else{
      printf("Block %d\t Size: %d\t Unallocated \n", counter, *ptr );
    }
    counter++;
    ptr = nextBlock(ptr);
  }
  printf("\n");
}



void *stackBottom() {
	unsigned long bottom;
    FILE *statfp = fopen("/proc/self/stat", "r");
    fscanf(statfp,
           "%*d %*s %*c %*d %*d %*d %*d %*d %*u "
           "%*u %*u %*u %*u %*u %*u %*d %*d "
           "%*d %*d %*d %*d %*u %*u %*d "
           "%*u %*u %*u %lu", &bottom);
    fclose(statfp);
	return (void *) bottom;
}

void mark(int *p){
  int * first = firstBlock();
  int * last = lastBlock();
	if(((int*)p >= first) && ((int*)p <= last)){
    while (first < last){
      if ((*p % 4) < 2 &&  p < nextBlock(first) && isAllocated(p)==1 && p > first){
        *(int*)p+=2;
        int * next = nextBlock(first);
        first+=1;
          while (first < next){
            mark(first);
            first+=1;
          }
      break;
    }
    first = nextBlock(first);
  }
}

}

void sweep(){
	int * ptr = (int*) firstBlock();
	int * last = (int*) lastBlock();
	while (ptr != last){
		if ((*ptr%4) != 3){
			myfree(ptr + 1);
			ptr = (int*) nextBlock(ptr);
		}
	}
	coalesce();
}



void gc(){
	int ** max = (int**) stackBottom();
	int * dummy;
	int** p = &dummy;
	while (p < max){
		mark(*p);
		p++;
	}
	sweep();
	coalesce();
}
