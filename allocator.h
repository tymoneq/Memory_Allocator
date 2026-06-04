#pragma once
#include <stddef.h>
#include <stdint.h>

#define true 1
#define false 0

extern const int MAGICAL_BYTES;
extern const int PAGE_SIZE;

struct memory_block {
  size_t size;
  struct memory_block *next_block;
  struct memory_block *prev_block;
};

struct block_counter {
  uint8_t marker;
  uint32_t number_of_pages;
  uint32_t number_of_blocks;
};

typedef struct memory_block memory_block;
typedef struct block_counter block_counter;

// Global pointers shared with the application (exposed via extern)
extern block_counter *my_stats;

// Public Core Allocator Functions
void *my_malloc(size_t size);
void free_all_pages();
void my_free(void *ptr);