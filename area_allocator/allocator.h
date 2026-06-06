#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>

typedef struct {
  unsigned char *buffer;
  size_t capacity;
  size_t offset;

} Arena;

static inline size_t align8(size_t capacity);

Arena init_area(size_t capacity);
void free_area(Arena *area);
void reset_offset(Arena *area);
void *allocate(Arena *area, size_t size);