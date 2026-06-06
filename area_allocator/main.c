#include <alloca.h>
#include <stdio.h>
#include "allocator.h"

int main() {
  Arena arena = init_area(10 * 4096);
  int* p = allocate(&arena, sizeof(int) * 10);

  printf("%p\n", p);
  p[1] = 10;
  printf("%d\n", p[1]);

  return 0;
}