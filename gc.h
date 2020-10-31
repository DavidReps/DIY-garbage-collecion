#ifndef GC_H
#define GC_H

void gc(); // perform garbage collection on the heap
void *stackBottom();
void sweep();
void mark(int *p);


#endif
