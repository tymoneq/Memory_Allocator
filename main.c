#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

const int MAGICAL_BYTES = 0x55;
const int BLOCK_MARKER = 0xDD;
const int PAGE_SIZE = 4096;

struct memory_block {
  size_t size;
  unsigned used;
  struct memory_block* next_block;
  struct memory_block* prev_block;
};

struct block_counter {
  uint8_t marker;
  uint32_t number_of_pages;
  uint32_t number_of_blocks;
};

typedef struct memory_block memory_block;
typedef struct block_counter block_counter;
#define true 1
#define false 2

char* heap_start = NULL;
char* block_end = NULL;
block_counter* my_stats = NULL;
memory_block* first_block = NULL;
memory_block* last_block = NULL;

/**
 * The function `my_malloc` allocates a new page of memory using the `sbrk`
 * system call.
 *
 * @param size The `size` parameter in the `my_malloc` function represents the
 * number of bytes of memory to allocate. This function allocates a new page of
 * memory using the `sbrk` system call, which increases the program's data
 * segment size by the specified amount. If the allocation is successful,
 *
 * @return The function `my_malloc` is returning a pointer to the newly
 * allocated page of memory. If the allocation is successful, it returns a
 * pointer to the start of the allocated memory block. If the allocation fails,
 * it returns `NULL`.
 */
void* allocate(size_t size) {
  void* page;
  page = sbrk(size);

  if (page == (void*)-1)
    return NULL;
  return page;
}

/**
 * The function `set_block_counter` allocates memory and initializes a block
 * counter if certain conditions are met.
 *
 * @return The function `set_block_counter` is returning a pointer to a
 * `block_counter` structure if the initialization is successful. If the
 * initialization fails or if the memory allocation is not done properly, it
 * returns a `NULL` pointer.
 */
block_counter* set_block_counter() {
  assert(heap_start == NULL);
  heap_start = allocate(0);
  assert(heap_start != NULL);

  allocate(PAGE_SIZE);

  if (*(heap_start) != MAGICAL_BYTES) {
    *(heap_start) = MAGICAL_BYTES;
    block_counter* my_stats = (block_counter*)heap_start;
    my_stats->number_of_blocks = 0;
    my_stats->number_of_pages = 1;
    block_end = heap_start + sizeof(block_counter);
    return my_stats;
  }
  return NULL;
}

char* get_heap_end() {
  return sbrk(0);
}

memory_block* create_memory_block(size_t size) {
  memory_block* new_block = (memory_block*)(block_end);
  new_block->size = size;
  new_block->used = true;
  new_block->prev_block = NULL;
  new_block->next_block = NULL;
  block_end += size;
  return new_block;
}

void create_new_memory_block(size_t size) {
  uint64_t current_heap_size_free = get_heap_end() - block_end;

  if (size > current_heap_size_free) {
    // need to allocate new page
    allocate(PAGE_SIZE);
    my_stats->number_of_pages += 1;
  }
  memory_block* new_block = create_memory_block(size);
  if (first_block == NULL)
    first_block = new_block;

  // connecting linked list
  if (last_block != NULL) {
    last_block->next_block = new_block;
    new_block->prev_block = last_block;
  }
  last_block = new_block;

  my_stats->number_of_blocks += 1;
}

int main() {
  printf("Start\n");

  my_stats = set_block_counter();

  if (my_stats != NULL) {
    printf("NICE, %p\n", my_stats);

    printf("Allocating memory block\n");
    create_new_memory_block(PAGE_SIZE / 4);

    printf("%p\n", first_block);
    create_new_memory_block(PAGE_SIZE * 2);

    printf("%p\n", first_block->next_block);

    printf("Number of pages  %d and block %d\n", my_stats->number_of_pages,
           my_stats->number_of_blocks);

    sbrk(-PAGE_SIZE);
  }

  return 0;
}