#include <stdlib.h>
#include <stdio.h>
int guarded_div(int a, int b) {
  if (!b) {
    printf("error: attempted to divide %d by zero, returning zero\n", a);
    return 0;
  }
  return a / b;
}
