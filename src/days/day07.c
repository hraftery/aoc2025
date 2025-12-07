#define DAY 07
#include "main.h"
#include "common.h"

#include <string.h>

#define MAX_COLS 150
#define MAX_ROWS 150

void part1(FILE *f)
{
  char grid[MAX_ROWS][MAX_COLS];
  
  int maxRow = 0;
  while (fgets(grid[maxRow], MAX_COLS, f) != NULL)
    maxRow++;
  
  int maxCol = strcspn(grid[0], "\r\n") - 1;
  if((int)strcspn(grid[maxRow], "\r\n") - 1 < maxCol)
    maxRow--;
  int minRow = 0, minCol = 0;
  
  //printf("maxR[%d], maxC[%d]\n", maxRow, maxCol);

  bool tachyon[MAX_COLS] = { false };
  for(int col=minCol; col<=maxCol; col++)
  {
    if(grid[0][col] == 'S')
    {
      tachyon[col] = true;
      break;
    }
  }

  int numSplits = 0;
  for(int row=minRow; row<=maxRow; row++)
  {
    for(int col=minCol; col<=maxCol; col++)
    {
      if(grid[row][col] == '^' && tachyon[col])
      {
        numSplits++;
        tachyon[col] = false;
        if(col>minCol)
          tachyon[col-1] = true;
        if(col<maxCol)
          tachyon[col+1] = true;
      }
    }
  }

  // int numBeams = 0;
  // for(int col=0; col<=maxCol; col++)
  //   if(tachyon[col])
  //     numBeams++;

  printf("%d", numSplits);
}

void part2(FILE *f)
{
  char grid[MAX_ROWS][MAX_COLS];
  
  int maxRow = 0;
  while (fgets(grid[maxRow], MAX_COLS, f) != NULL)
    maxRow++;
  
  int maxCol = strcspn(grid[0], "\r\n") - 1;
  if((int)strcspn(grid[maxRow], "\r\n") - 1 < maxCol)
    maxRow--;
  int minRow = 0, minCol = 0;
  
  //printf("maxR[%d], maxC[%d]\n", maxRow, maxCol);

  uint64_t tachyon[MAX_COLS] = { 0 };
  for(int col=minCol; col<=maxCol; col++)
  {
    if(grid[0][col] == 'S')
    {
      tachyon[col] = 1;
      break;
    }
  }

  for(int row=minRow; row<=maxRow; row++)
  {
    for(int col=minCol; col<=maxCol; col++)
    {
      if(grid[row][col] == '^' && tachyon[col])
      {
        if(col>minCol)
          tachyon[col-1] += tachyon[col];
        if(col<maxCol)
          tachyon[col+1] += tachyon[col];
        tachyon[col] = 0;
      }
    }
  }

  uint64_t numTimelines = 0;
  for(int col=0; col<=maxCol; col++)
    numTimelines += tachyon[col];

  printf("%llu", numTimelines);
}
