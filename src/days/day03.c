#define DAY 03
#include "main.h"
#include "common.h"

#include <string.h>

#define MAX_LINE_LENGTH 150

void part1(FILE *f)
{
  char line[MAX_LINE_LENGTH];
  int totalJoltage = 0;

  while (fgets(line, MAX_LINE_LENGTH, f) != NULL)
  {
    line[strcspn(line, "\r\n")] = '\0'; //chomp

    char max1='0';
    size_t maxIdx;
    for(size_t i=0; i<strlen(line)-1; i++)
    {
      if(line[i] > max1)
      {
        max1 = line[i];
        maxIdx = i;
      }
    }
    char max2='0';
    for(size_t i=maxIdx+1; i<strlen(line); i++)
    {
      if(line[i] > max2)
        max2 = line[i];
    }

    int joltage = 10 * (max1 - '0') + (max2 - '0');
    printf("Joltage = %d.\n", joltage);
    totalJoltage += joltage;
  }
  printf("%d", totalJoltage);
}

void part2(FILE *f)
{
  char line[MAX_LINE_LENGTH];
  uint64_t totalJoltage = 0;

  while (fgets(line, MAX_LINE_LENGTH, f) != NULL)
  {
    line[strcspn(line, "\r\n")] = '\0'; //chomp

    uint64_t joltage=0;

    size_t maxIdx = 0;
    for(int digit=1; digit<=12; digit++)
    {
      char max = '0';
      for(size_t i=maxIdx; i<strlen(line)-(12-digit); i++)
      {
        if(line[i] > max)
        {
          max = line[i];
          maxIdx = i;
        }
      }
      joltage = 10*joltage + max - '0';
      maxIdx++;
    }

    printf("Joltage = %llu.\n", joltage);
    totalJoltage += joltage;
  }
  printf("%llu", totalJoltage);
}
