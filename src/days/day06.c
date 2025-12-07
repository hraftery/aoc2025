#define DAY 06
#include "main.h"
#include "common.h"

#include <string.h>

#define MAX_LINE_LENGTH 4000
#define MAX_ROWS        10
#define MAX_COLS        2000

char line[MAX_LINE_LENGTH];
uint32_t vals[MAX_ROWS][MAX_COLS];
char ops[MAX_COLS];

void part1(FILE *f)
{
  size_t numRows = 0, numCols = 0;

  while (fgets(line, MAX_LINE_LENGTH, f) != NULL)
  {
    if(line[0] == '*' || line[0] == '+')
      break;
    
    uint32_t val;
    char *pLine = line;
    int offset;
    while(sscanf(pLine, "%d%n", &val, &offset) == 1)
    {
      vals[numRows][numCols++] = val;
      pLine += offset;
    }
    numRows++;
    numCols = 0;
  }
  char op;
  char *pLine = line; 
  int offset;
  while(sscanf(pLine, "%c %n", &op, &offset) == 1)
  {
    ops[numCols++] = op;
    pLine += offset;
  }

  uint64_t sumAns = 0;
  for(size_t col=0; col<numCols; col++)
  {
    bool sum = ops[col] == '+';
    uint64_t ans = sum ? 0 : 1;
    for(size_t row=0; row<numRows; row++)
      ans = sum ? ans + vals[row][col] : ans * vals[row][col];
    
    //printf("Col[%zu] = %llu.\n", col, ans);
    sumAns += ans;
  }
  printf("%llu", sumAns);
}

char lines[MAX_ROWS][MAX_LINE_LENGTH];
char linesT[MAX_LINE_LENGTH][MAX_ROWS];

void part2(FILE *f)
{
  size_t numRows = 0, numCols = 0;

  while (fgets(lines[numRows], MAX_LINE_LENGTH, f) != NULL)
  {
    if(lines[numRows][0] == '*' || lines[numRows][0] == '+')
      break;
    numRows++;
  }
  char op;
  char *pLine = lines[numRows];
  int offset;
  while(sscanf(pLine, "%c %n", &op, &offset) == 1)
  {
    ops[numCols++] = op;
    pLine += offset;
  }

  size_t len = strlen(lines[0]);
  for(size_t x=0; x<len; x++)
    for(size_t y=0; y<numRows; y++)
      linesT[x][y] = lines[y][x]; //turns out right-to-left doesn't matter. Stick to left-to-right.

  size_t linesRow = 0;
  for(size_t col=0; col<numCols; col++)
  {
    size_t row = 0;
    uint32_t val;
    while(sscanf(linesT[linesRow++], "%d", &val) == 1)
      vals[row++][col] = val;
    for(; row<numRows; row++)
      vals[row][col] = ops[col] == '+' ? 0 : 1; //fill the rest with dummy values
  }

  uint64_t sumAns = 0;
  for(size_t col=0; col<numCols; col++)
  {
    bool sum = ops[col] == '+';
    uint64_t ans = sum ? 0 : 1;
    for(size_t row=0; row<numRows; row++)
      ans = sum ? ans + vals[row][col] : ans * vals[row][col];
    
    printf("Col[%zu] = %llu.\n", col, ans);
    sumAns += ans;
  }
  printf("%llu", sumAns);
}
