#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include "allocator.h"

int main() {
  printf("Start\n");

  void* test = my_malloc(50);
  void* test1 = my_malloc(6000);
  void* test2 = my_malloc(501);
  void* test3 = my_malloc(6001);

  printf("Stats, %p\n", my_stats);
  printf("%p\n", test);
  printf("%p\n", test1);
  printf("%p\n", test2);
  printf("%p\n", test3);

  set_block_free(test);
  void* test4 = my_malloc(50);
  printf("%p\n", test4);

  free_all_pages();

  return 0;
}