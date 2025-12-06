#define DAY 05
#include "main.h"
#include "common.h"

#include <string.h>

#define MAX_LINE_LENGTH 100
#define MAX_RANGES      2000
#define MAX_INGREDIENTS 2000

void part1(FILE *f)
{
  uint64_t lo[MAX_RANGES];
  uint64_t hi[MAX_RANGES];
  uint64_t in[MAX_INGREDIENTS];
  
  char line[MAX_LINE_LENGTH];

  int numRanges = 0;
  
  while (fgets(line, MAX_LINE_LENGTH, f) != NULL)
  {
    if(sscanf(line, "%llu-%llu\n", &lo[numRanges], &hi[numRanges]) != 2)
      break;
    numRanges++;
  }

  int numIngred = 0;
  while (fgets(line, MAX_LINE_LENGTH, f) != NULL)
  {
    if(sscanf(line, "%llu", &in[numIngred]) != 1)
      break;
    numIngred++;
  }
  
  //printf("numRanges[%d], numIngred[%d]\n", numRanges, numIngred);

  int numFresh = 0;
  for(int i=0; i<numIngred; i++)
  {
    for(int r=0; r<numRanges; r++)
    {
      if(lo[r] <= in[i] && in[i] <= hi[r])
      {
        //printf("in[%d] falls in r[%d]\n", i, r);
        numFresh++;
        break;
      }
    }
  }
  printf("%d", numFresh);
}

void part2(FILE *f)
{
  printf("This is day %d, part 2.\n", DAY);
}
