#include "sum.h"

#include <stdio.h>
#include <stdlib.h>


int Sum(const struct SumArgs *args) {
  int sum = 0;
  int e = args[thrid].end;
  for(int i = args[thrid].begin; i<e; i++){
    sum+=args[0].array[i];
  }
  thrid++;
  return sum;
}