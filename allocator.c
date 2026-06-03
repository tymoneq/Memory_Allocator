#include "allocator.h"
#include <assert.h>
#include <stdio.h>
#include <unistd.h>

const int MAGICAL_BYTES = 0x55;
const int PAGE_SIZE = 4096;


char* heap_start = NULL;
char* block_end = NULL;
memory_block* first_block = NULL;
memory_block* last_block = NULL;


block_counter* my_stats = NULL;


static void* allocate(size_t size) {
  void* page = sbrk(size);
  if (page == (void*)-1)
    return NULL;
  return page;
}

// Internal Helper: Get the top of the heap
static char* get_heap_end() {
  return sbrk(0);
}

// Internal Helper: Initialize block counter tracking
static block_counter* set_block_counter() {
  assert(heap_start == NULL);
  heap_start = allocate(0);
  assert(heap_start != NULL);

  allocate(PAGE_SIZE);

  if (*(heap_start) != MAGICAL_BYTES) {
    *(heap_start) = MAGICAL_BYTES;
    block_counter* stats_ptr = (block_counter*)heap_start;
    stats_ptr->number_of_blocks = 0;
    stats_ptr->number_of_pages = 1;
    block_end = heap_start + sizeof(block_counter);
    return stats_ptr;
  }
  return NULL;
}

// Internal Helper: Carve out a block metadata structure
static memory_block* create_memory_block(size_t size) {
  memory_block* new_block = (memory_block*)(block_end);
  new_block->size = size;
  new_block->used = true;
  new_block->prev_block = NULL;
  new_block->next_block = NULL;
  block_end += sizeof(memory_block) + size;
  return new_block;
}

// Internal Helper: Chain a new block into the list
static memory_block* create_new_memory_block(size_t size) {
  uint64_t current_heap_size_free = get_heap_end() - block_end;

  if (size > current_heap_size_free) {
    allocate(PAGE_SIZE);
    my_stats->number_of_pages += 1;
  }
  memory_block* new_block = create_memory_block(size);
  if (first_block == NULL)
    first_block = new_block;

  if (last_block != NULL) {
    last_block->next_block = new_block;
    new_block->prev_block = last_block;
  }
  last_block = new_block;

  my_stats->number_of_blocks += 1;
  return new_block;
}

// Internal Helper: Scan free list
static memory_block* find_free_node(size_t size) {
  memory_block* current_node = first_block;
  while (current_node != NULL) {
    if (current_node->size >= size && current_node->used == false) {
      return current_node;
    }
    current_node = current_node->next_block;
  }
  return NULL;
}


void set_block_free(memory_block* block) {
  block->used = false;
}

void set_block_used(memory_block* block) {
  block->used = true;
}

void free_all_pages() {
  uint64_t size_of_memory = my_stats->number_of_pages * PAGE_SIZE;
  sbrk(-size_of_memory);
  heap_start = NULL;
  block_end = NULL;
  my_stats = NULL;
  first_block = NULL;
  last_block = NULL;
}

void* my_malloc(size_t size) {
  if (my_stats == NULL)
    my_stats = set_block_counter();

  memory_block* block = find_free_node(size);
  if (block != NULL) {
    set_block_used(block);
    return block;
  }
  return create_new_memory_block(size);
}