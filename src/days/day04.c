#define DAY 04
#include "main.h"
#include "common.h"

#include <string.h>

#define MAX_LINE_LENGTH 150
#define MAX_ROWS        150

void part1(FILE *f)
{
  char grid[MAX_ROWS][MAX_LINE_LENGTH];
  
  int maxRow = 0;
  while (fgets(grid[maxRow], MAX_LINE_LENGTH, f) != NULL)
    maxRow++;
  
  int maxCol = strcspn(grid[0], "\r\n") - 1;
  if((int)strcspn(grid[maxRow], "\r\n") - 1 < maxCol)
    maxRow--;
  int minRow = 0, minCol = 0;
  
  printf("maxR[%d], maxC[%d]\n", maxRow, maxCol);

  int accessibleRolls = 0;
  for(int row=minRow; row<=maxRow; row++)
  {
    for(int col=minCol; col<=maxCol; col++)
    {
      if(grid[row][col] != '@')
        continue;
      
      int numRolls = 0;
      for(int r=MAX(minRow, row-1); r<=MIN(maxRow, row+1); r++)
      {
        for(int c=MAX(minCol, col-1); c<=MIN(maxCol, col+1); c++)
        {
          if(r==row && c==col)
            continue;
          if(grid[r][c] == '@')
            if(++numRolls == 4)
              goto tooManyRolls;
        }
      }

      printf("x at (%u,%u)\n", col, row);
      accessibleRolls++;
tooManyRolls:;
    }
  }

  printf("%d", accessibleRolls);
}

void part2(FILE *f)
{
  char grid[MAX_ROWS][MAX_LINE_LENGTH];
  
  int maxRow = 0;
  while (fgets(grid[maxRow], MAX_LINE_LENGTH, f) != NULL)
    maxRow++;
  
  int maxCol = strcspn(grid[0], "\r\n") - 1;
  if((int)strcspn(grid[maxRow], "\r\n") - 1 < maxCol)
    maxRow--;
  int minRow = 0, minCol = 0;
  
  printf("maxR[%d], maxC[%d]\n", maxRow, maxCol);

  int accessibleRolls = 0, thisAccessibleRolls;

  do {
    thisAccessibleRolls = 0;
    for(int row=minRow; row<=maxRow; row++)
    {
      for(int col=minCol; col<=maxCol; col++)
      {
        if(grid[row][col] != '@')
          continue;
        
        int numRolls = 0;
        for(int r=MAX(minRow, row-1); r<=MIN(maxRow, row+1); r++)
        {
          for(int c=MAX(minCol, col-1); c<=MIN(maxCol, col+1); c++)
          {
            if(r==row && c==col)
              continue;
            if(grid[r][c] == '@')
              if(++numRolls == 4)
                goto tooManyRolls;
          }
        }

        //printf("x at (%u,%u)\n", col, row);
        thisAccessibleRolls++;
        grid[row][col] = '.';
tooManyRolls:;
      }
    }
    printf("Got %d.\n", thisAccessibleRolls);
    accessibleRolls += thisAccessibleRolls;
  } while(thisAccessibleRolls > 0);

  printf("%d", accessibleRolls);
}
