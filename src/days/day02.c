#define DAY 02
#include "main.h"
#include "common.h"

#include <string.h>

int64_t ipow(int base, unsigned int exp)
{
  int64_t result = 1;
  while(1)
  {
    if(exp & 1)
      result *= base;
    exp >>= 1;
    if(exp == 0)
      break;
    base *= base;
  }

  return result;
}

const uint32_t SMALLEST_VALUE_FOR_NUM_DIGITS[] = {
  0, //0 digits (dummy value)
  1, //1 digit
  10, //2 digits
  100, //3 digits
  1000, //4 digits
  10000, //5 digits
  100000, //6 digits
  1000000, //7 digits
  10000000, //8 digits
  100000000, //9 digits
  1000000000  //10 digits
};
const uint32_t BIGGEST_VALUE_FOR_NUM_DIGITS[] = {
  0, //0 digits (dummy value)
  9, //1 digit
  99, //2 digits
  999, //3 digits
  9999, //4 digits
  99999, //5 digits
  999999, //6 digits
  9999999, //7 digits
  99999999, //8 digits
  999999999, //9 digits
  UINT32_MAX  //10 digits
};

void split_number_string(const char numStr[], int numDigits, uint32_t *pFirstHalf, uint32_t *pSecondHalf)
{
  uint32_t firstHalf = 0, secondHalf = 0;
  int i;
  for(i=0; i<numDigits/2; i++)
  {
    firstHalf *= 10;
    firstHalf += numStr[i] - '0';
  }
  for(; i<numDigits; i++)
  {
    secondHalf *= 10;
    secondHalf += numStr[i] - '0';
  }

  *pFirstHalf = firstHalf;
  *pSecondHalf = secondHalf;
}

uint32_t find_start_repeat(int numDigits, int loDigits, const char loStr[])
{
  uint32_t startRepeat;

  if(numDigits != loDigits) //then repeats start at the lowest possible number
    startRepeat = SMALLEST_VALUE_FOR_NUM_DIGITS[numDigits/2];
  else //then repeats start at the provided low value of the range
  {
    uint32_t firstHalf, secondHalf;
    split_number_string(loStr, loDigits, &firstHalf, &secondHalf);

    startRepeat = firstHalf;
    if(secondHalf > firstHalf) //then we miss the first repeat
      startRepeat++;
  }

  return startRepeat;
}

uint32_t find_end_repeat(int numDigits, int hiDigits, const char hiStr[])
{
  uint32_t endRepeat;

  if(numDigits != hiDigits) //then repeats end at the biggest possible number
    endRepeat = BIGGEST_VALUE_FOR_NUM_DIGITS[numDigits/2];
  else //then repeats start at the provided hi value of the range
  {
    uint32_t firstHalf, secondHalf;
    split_number_string(hiStr, hiDigits, &firstHalf, &secondHalf);

    endRepeat = firstHalf;
    if(secondHalf < firstHalf) //then we miss the last repeat
      endRepeat--;
  }

  return endRepeat;
}

void part1(FILE *f)
{
  uint64_t invalidIdSum = 0;

#define MAX_DIGITS 10
  char loStr[MAX_DIGITS+1], hiStr[MAX_DIGITS+1];
  uint64_t lo, hi;

  while(fscanf(f, "%llu-%llu", &lo, &hi) == 2)
  {
    //printf("Got %llu to %llu.\n", lo, hi);
    //printf("Got %s to %s.\n", lowStr, highStr);
    sprintf(loStr, "%llu", lo);
    sprintf(hiStr, "%llu", hi);

    int loDigits = strlen(loStr);
    int hiDigits = strlen(hiStr);

    for(int numDigits = loDigits; numDigits <= hiDigits; numDigits++)
    {
      if(IS_ODD(numDigits))
        continue;
      
      uint32_t startRepeat = find_start_repeat(numDigits, loDigits, loStr);
      uint32_t endRepeat   = find_end_repeat(numDigits, hiDigits, hiStr);

      //printf("  Got repeats [%u,%u].\n", startRepeat, endRepeat);
      for(uint32_t i=startRepeat; i<=endRepeat; i++)
      {
        uint64_t invalidId = i * ipow(10, numDigits/2) + i;
        //printf("  Invalid ID: %llu.", invalidId);
        invalidIdSum += invalidId;
        //printf("  Invalid ID sum: %llu.\n", invalidIdSum);
      }
    }
    //printf("\n");

    fgetc(f); //eat the ","
  }

  printf("%llu\n", invalidIdSum);
}


//=====================================================================
//=====================================================================

void part2(FILE *f)
{
  uint64_t invalidIdSum = 0;

#define MAX_DIGITS 10
  uint64_t lo, hi;

  while(fscanf(f, "%llu-%llu", &lo, &hi) == 2)
  {
    printf("Got %llu to %llu.\n", lo, hi);
    //printf("Got %s to %s.\n", lowStr, highStr);

    for(uint64_t id=lo; id<=hi; id++)
    {
      char idStr[MAX_DIGITS+1];
      sprintf(idStr, "%llu", id);
      int numDigits = strlen(idStr);

      int repeatLen;
      for(repeatLen=1; repeatLen<=numDigits/2; repeatLen++)
      {
        if(numDigits % repeatLen != 0)
          continue;
        
        int repeatIdx;
        for(repeatIdx=0; repeatIdx<repeatLen; repeatIdx++)
        {
          char c = idStr[repeatIdx];
          int idIdx;
          for(idIdx=repeatLen+repeatIdx; idIdx<numDigits; idIdx+=repeatLen)
            if(idStr[idIdx] != c)
              break;
          if(idIdx < numDigits)
            break;
        }
        if(repeatIdx >= repeatLen)
          break;
      }
      if(repeatLen<=numDigits/2)
      {
        invalidIdSum += id;
        printf("  Invalid ID: %s. Invalid ID sum: %llu. Repeat len: %d.\n", idStr, invalidIdSum, repeatLen);
      }
    }

    fgetc(f); //eat the ","
  }

  printf("%llu\n", invalidIdSum);
}
