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
  uint64_t lo[MAX_RANGES];
  uint64_t hi[MAX_RANGES];
  size_t llLoAsc[MAX_RANGES];
  size_t llLoAscHead = 0;
  
  char line[MAX_LINE_LENGTH];

  int numRanges = 0;
  while (fgets(line, MAX_LINE_LENGTH, f) != NULL)
  {
    if(sscanf(line, "%llu-%llu\n", &lo[numRanges], &hi[numRanges]) != 2)
      break;

    //Create linked list of indices in order of lo value as we go.
    if(numRanges > 0)
    {
      size_t llIdxPrev;
      for(int i=0; i<numRanges; i++)
      {
        uint64_t thisLo = i==0 ? lo[llLoAscHead] : lo[llLoAsc[llIdxPrev]];

        if(lo[numRanges] <= thisLo) //then insert here
        {
          if(i==0) //then update head
          {
            llLoAsc[numRanges] = llLoAscHead;
            llLoAscHead = numRanges;
          }
          else //update ll
          {
            llLoAsc[numRanges] = llLoAsc[llIdxPrev];
            llLoAsc[llIdxPrev] = numRanges;
          }
          break;
        }
        else if(i == numRanges-1) //tack on to end
        {
          if(i==0)
            llLoAsc[llLoAscHead] = numRanges;
          else
            llLoAsc[llLoAsc[llIdxPrev]] = numRanges;
        }
        
        llIdxPrev = i==0 ? llLoAscHead : llLoAsc[llIdxPrev];
      }
    }

    numRanges++;
  }
  
  //Consolidate ranges
  size_t consIdx = llLoAscHead;
  size_t idx = llLoAsc[consIdx];
  int origNumRanges = numRanges;
  for(int i=1; i<origNumRanges; i++)
  {
    uint64_t thisLo = lo[idx], thisHi = hi[idx];

    if(thisLo <= hi[consIdx] + 1) //within the current cons, so extend it
    {
      hi[consIdx] = MAX(thisHi, hi[consIdx]);
      llLoAsc[consIdx] = llLoAsc[idx]; //now it's included, skip it
      numRanges--;
    }
    else //start new cons
    {
      consIdx = idx;
    }

    idx = llLoAsc[idx];
  }

  //Sum ranges
  uint64_t sum=0;
  idx = llLoAscHead;
  for(int i=0; i<numRanges; i++)
  {
    sum += hi[idx] - lo[idx] + 1;
    idx = llLoAsc[idx];
  }

  printf("%llu", sum);
}
