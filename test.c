#include <stdlib.h>

int main(int argc, char const *argv[]) {
  int *array = malloc(10 * sizeof(int));
  array[0] = 5;
  return 0;
}
