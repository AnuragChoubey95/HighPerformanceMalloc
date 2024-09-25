/**
 * Copyright (c) 2015 MIT License by 6.172 Staff
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 **/

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "./allocator_interface.h"
#include "./binned_free_list.h"
#include "./memlib.h"

// Don't call libc malloc!
#define malloc(...) (USE_MY_MALLOC)
#define free(...) (USE_MY_FREE)
#define realloc(...) (USE_MY_REALLOC)



// check - This checks our invariant that the my_free_list_header header before every
// block points to either the beginning of the next block, or the end of the
// heap.
int my_check() {
  char* p;
  char* lo = (char*)mem_heap_lo();
  char* hi = (char*)mem_heap_hi() + 1;
  size_t size = 0;

  p = lo;
  while (lo <= p && p < hi) {
    size = *(size_t*)p;
    p += size;
  }

  if (p != hi) {
    printf("Bad headers did not end at heap_hi!\n");
    printf("heap_lo: %p, heap_hi: %p, size: %lu, p: %p\n", lo, hi, size, p);
    return -1;
  }

  return 0;
}

// init - Initialize the malloc package.  Called once before any other
// calls are made.  Since this is a very simple implementation, we just
// return success.
int my_init(size_t size) {
 
  int code = init_binArray();
  return code;
}

void* find_next_free_node(size_t size){
  my_free_list_header* p;
  my_free_list_header* lo = (my_free_list_header*)mem_heap_lo();
  for(p = ((my_free_list_header*)lo)->next; p != lo; p = p->next){
    if (p != mem_heap_lo() && p->size >= size){
      return p;
    }
  }
  return NULL;
}

//  malloc - Allocate a block by incrementing the brk pointer.
//  Always allocate a block whose size is a multiple of the alignment.
void* my_malloc(size_t size) {
  
  void* ans = my_malloc_bfl(size);
  return ans;
}

void my_free(void* ptr) {

  my_free_bfl(ptr);
}

// realloc - Implemented simply in terms of malloc and free
void* my_realloc(void* ptr, size_t size) {
  void* newptr;
  size_t copy_size;

  // Allocate a new chunk of memory, and fail if that allocation fails.
  int aligned_size = ALIGN(size + MY_FREE_LIST_HEADER_SIZE);
  newptr = my_malloc(aligned_size);
  if (NULL == newptr) {
    return NULL;
  }

  // Get the size of the old block of memory.  Take a peek at my_malloc(),
  // where we stashed this in the MY_FREE_LIST_HEADER_SIZE bytes directly before the
  // address we returned.  Now we can back up by that many bytes and read
  // the size.
  copy_size = *(size_t*)((uint8_t*)ptr - MY_FREE_LIST_HEADER_SIZE);

  // If the new block is smaller than the old one, we have to stop copying
  // early so that we don't write off the end of the new block of memory.
  if (aligned_size < copy_size) {
    copy_size = aligned_size;
  }

  // This is a standard library call that performs a simple memory copy.
  memcpy(newptr, ptr, copy_size);

  // Release the old block.
  my_free(ptr);

  assert(my_check() == 0);

  // Return a pointer to the new block.
  return newptr;
  // return NULL;
}
