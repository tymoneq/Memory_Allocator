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

char* heap_start = NULL;
block_counter* my_stats = NULL;
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
void* my_malloc(size_t size) {
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
  heap_start = my_malloc(0);
  assert(heap_start != NULL);

  char* heap_end = my_malloc(PAGE_SIZE);

  if (*(heap_start) != MAGICAL_BYTES) {
    *(heap_start) = MAGICAL_BYTES;
    block_counter* my_stats = (block_counter*)heap_start;
    my_stats->number_of_blocks = 0;
    my_stats->number_of_pages = 1;
    return my_stats;
  }
  return NULL;
}

char* get_heap_end() {
  return sbrk(0);
}

memory_block* create_new_memory_block(size_t size) {
  uint64_t current_heap_size = get_heap_end() - heap_start;

  if (size > current_heap_size) {
    // need to allocate new page
    char* new_page_end = my_malloc(PAGE_SIZE);
    my_stats->number_of_pages += 1;
  }
}

int main() {
  printf("Start\n");

  my_stats = set_block_counter();

  if (my_stats != NULL) {
    printf("NICE, %p\n", my_stats);
    sbrk(-PAGE_SIZE);
  }

  return 0;
}