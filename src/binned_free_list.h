#ifndef BINNED_FREE_LIST_H
#define BINNED_FREE_LIST_H

#include <stdbool.h>
#include <stddef.h>
#include "./memlib.h"

#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))
#define MY_FREE_LIST_HEADER_SIZE (ALIGN(sizeof(my_free_list_header)))
#define NUM_BINS 25

typedef struct my_free_list_header my_free_list_header;

struct my_free_list_header {
    size_t size;
    my_free_list_header* next;
    my_free_list_header* prev;
};

// Function declarations
int init_binArray(void);
int find_bin(size_t size);
my_free_list_header* find_next_free_node_bfl(size_t size);
void* my_malloc_bfl(size_t size);
void my_free_bfl(void* ptr);

#endif // BINNED_FREE_LIST_H

