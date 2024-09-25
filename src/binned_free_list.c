#include "./binned_free_list.h"

//Array of circular linked lists
//Each index location stores the head of circular linked list, size@idx = 2 ^^ idx. 
//last bin is for unlimited category
my_free_list_header* binArray[NUM_BINS]; 

//initialize all bin headers. sizeOfHeader@idx = 2 ^^ idx.
int init_binArray(){
    for(int i  = 0; i < NUM_BINS; i++){
        binArray[i] = (my_free_list_header*)mem_sbrk(MY_FREE_LIST_HEADER_SIZE);
        if(!binArray[i]) {return -1;}
        binArray[i]->size = 0;
        binArray[i]->next = binArray[i];
        binArray[i]->prev = binArray[i];
    }
    return 0;
}

//Given a size x requirement, find the bin with free blocks 
// that are the next greater power of two in size.
int find_bin(size_t size){
    int num = 0;
    while(1 << num <= size){
        num += 1;
    }
    //last bin is for unlimited category (>= 2 ^^ NUM_BINS-1)
    return num >= NUM_BINS-1 ? NUM_BINS-1 : num;
}

my_free_list_header* find_next_free_node_bfl(size_t size){
    int bin_idx = find_bin(size);

    my_free_list_header* binHead = binArray[bin_idx];
    my_free_list_header* freeNode;

    for(freeNode = binHead->next; freeNode != binHead; freeNode = freeNode->next){
        if (freeNode != binHead && freeNode->size >= size){
            return freeNode;
        }
    }
    return NULL;
}


void* my_malloc_bfl(size_t size){
    //size 0 means no malloc!
    if (size == 0){
        return NULL;
    }
    int aligned_size = ALIGN(size + MY_FREE_LIST_HEADER_SIZE);
    //Finding the right bin header
    //Round up to bin class size
    
    my_free_list_header* my_free_node = find_next_free_node_bfl(aligned_size);

    if (my_free_node == NULL){
        my_free_node = (my_free_list_header*)mem_sbrk(aligned_size);

        if((long)my_free_node ==  -1){
            return NULL;
        } else {
            my_free_node->size = aligned_size;
        } 
    } else {
        // Completely take this node off of my data sturcture
        my_free_node->prev->next = my_free_node->next;
        my_free_node->next->prev = my_free_node->prev;
    }
    //return pointer to start of "usable" memory location, instead of the free node
    return (char*)my_free_node + MY_FREE_LIST_HEADER_SIZE;
}


void my_free_bfl(void* ptr){
    my_free_list_header* toFree = (my_free_list_header*)((char*)ptr - MY_FREE_LIST_HEADER_SIZE);
    int idx = find_bin(toFree->size);

    my_free_list_header* start = binArray[idx];
    // toFree->allocated = false;
    toFree->next = start->next;
    toFree->prev = start;
    start->next = toFree;
    toFree->next->prev = toFree;
    toFree->prev->next = toFree;
    return;
}