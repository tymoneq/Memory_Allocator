#include "allocator.h"
#include <stdlib.h>
#include <sys/mman.h>

static inline size_t align8(size_t capacity) {
  return (capacity + 7) & ~7;
}

Arena init_area(size_t capacity) {
  Arena a;
  capacity = align8(capacity);
  a.buffer = mmap(NULL, capacity, PROT_READ | PROT_WRITE,
                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  if (a.buffer != MAP_FAILED) {
    a.capacity = capacity;
    a.offset = 0;
  }
  return a;
}
void free_area(Arena* arena) {
  if (arena->buffer) {
    munmap(arena->buffer, arena->capacity);
    arena->buffer = NULL;
    arena->capacity = 0;
    arena->offset = 0;
  }
}
void reset_offset(Arena* arena) {
  arena->offset = 0;
}
void* allocate(Arena* arena, size_t size) {
  size = align8(size);
  if (arena->capacity - arena->offset - size < 0)
    return NULL;

  arena->offset += size;
  return (void*)arena->buffer + (arena->offset - size);
}