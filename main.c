#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include "allocator.h"

struct test_struct {
  int buffor[500];
  int xd;
  long long testttt;
};
typedef struct test_struct test_s;

int main() {
  printf("Start\n");

  test_s* test = my_malloc(10 * sizeof(test_s));

  printf("%p\n", test);

  int y = test[1].testttt;
  y = 5;
  printf("%d\n", y);

  for (size_t i = 0; i < 10; i++) {
    test[i].xd = i;
    printf("%d\n", test[i].xd);
  }

  free_all_pages();

  return 0;
}