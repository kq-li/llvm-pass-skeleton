#include <stdio.h>

int zero() {
  return 0;
}

int one() {
  return 1;
}

void print_int(char* s, int x) {
  printf("%s%d\n", s, x);
}

int main() {
  int z = 0;
  int o = 1;
  print_int("1 / 1 (const) = ", o / o);
  print_int("0 / 1 (const) = ", z / o);
  print_int("1 / 0 (const) = ", o / z);
  print_int("1 / 0 (func) = ", o / zero());
  print_int("1 / 1 (func) = ", o / one());
  return 0;
}
