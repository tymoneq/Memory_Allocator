#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include "allocator.h"

int main() {
  printf("Start\n");

  int* test = my_malloc(10 * sizeof(int));

  printf("%p\n", test);

  int y = test[1];
  y = 5;
  printf("%d\n", y);

  for (size_t i = 0; i < 10; i++) {
    test[i] = i;
    printf("%d\n", test[i]);
  }

  free_all_pages();

  return 0;
}