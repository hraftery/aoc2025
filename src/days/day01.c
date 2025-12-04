#define DAY 01
#include "main.h"
#include "common.h"

#include <assert.h>
#include <stdio.h>

int mod(int a, int b)
{
  assert(b > 0);
  int r = a % b;
  return r < 0 ? r + b : r;
}

void part1(FILE *f)
{
  char dir;
  int dist;
  
  int zeroCount = 0;
  int pos = 50;
  const int WRAP = 100;
  while(fscanf(f, "%c%d\n", &dir, &dist) == 2)
  {
    if(dir == 'L')
      pos = mod(pos - dist, WRAP);
    else
      pos = mod(pos + dist, WRAP);
    
    if(pos == 0)
      zeroCount++;
    
    //printf("%c %d => %d\n", dir, dist, pos);
  }

  fclose(f);

  printf("%d", zeroCount);
}

void part2(FILE *f)
{
  printf("This is day %d, part 2.\n", DAY);
}
