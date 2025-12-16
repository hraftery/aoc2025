#define DAY 10
#include "main.h"
#include "common.h"

#include <string.h>
#include <time.h>

#define MAX_BUTTONS   16
#define MAX_JOLTAGES  16
#define MAX_MACHINES  200
#define MAX_LIGHTS    10  /* informational */

typedef uint16_t tLightBits;
typedef uint16_t tButtonBits;

typedef struct machine {
  uint8_t     numLights;
  tLightBits  targLights;
  uint8_t     numButtons;
  tLightBits  buttons[MAX_BUTTONS];
  uint8_t     numJoltages;
  uint16_t    joltages[MAX_JOLTAGES];
} sMachine;

sMachine gMachines[MAX_MACHINES];
uint16_t gNumMachines = 0;

void parse_input(FILE *f)
{
  int c;
  while(fscanf(f, "[%c", (char*)&c) == 1)
  {
    sMachine *pMachine = &(gMachines[gNumMachines++]);

    pMachine->targLights = 0x0000; //allow easy checks with equality
    pMachine->numLights = 0;

    while(c != ']')
    {
      if(c=='#')
        pMachine->targLights |= (1<<pMachine->numLights);
      pMachine->numLights++;
      c = fgetc(f);
      if(c == EOF)
        exit(1);
    }

    c = fgetc(f); //eat space
    c = fgetc(f);

    uint16_t u;
    pMachine->numButtons = 0;

    while(c == '(')
    {
      pMachine->buttons[pMachine->numButtons] = 0;

      while(fscanf(f, "%hu%c", &u, (char*)&c) == 2)
      {
        if(c!=',' && c!=')')
          exit(2);
        if(u >= pMachine->numLights)
          exit(3);
        pMachine->buttons[pMachine->numButtons] |= (1<<u);
      }
      pMachine->numButtons++;

      //fscanf eats whitespace even when it fails to make a conversion, so this gets character after that
      c = fgetc(f);
      
      if(c == EOF)
        exit(4);
    }

    pMachine->numJoltages = 0;

    if(c!='{')
      exit(5);

    while(fscanf(f, "%hu%c", &u, (char*)&c) == 2)
    {
      if(c!=',' && c!='}')
        exit(5);
      pMachine->joltages[pMachine->numJoltages++] = u;
    }
  }
}

bool do_find_solution(uint8_t i, sMachine *m, uint16_t depth, tButtonBits *pSolution)
{
  for(; i<m->numButtons; i++)
  {
    tButtonBits trySolution = *pSolution;
    trySolution |= (1<<i);

    if(depth > 1)
    {
      if(do_find_solution(i+1, m, depth-1, &trySolution))
      {
        *pSolution = trySolution;
        return true;
      }
    }
    else
    {
      tLightBits lights = 0;
      for(size_t i = 0; i < m->numButtons; i++)
      {
        if(trySolution & (1<<i))
          lights ^= m->buttons[i];
      }
      if(lights == m->targLights)
      {
        *pSolution = trySolution;
        return true;
      }
    }
  }
  return false;
}

bool find_solution(sMachine *m, uint16_t numButtonsInUse, tButtonBits *pSolution)
{
  return do_find_solution(0, m, numButtonsInUse, pSolution);
}

void part1(FILE *f)
{
  parse_input(f);

  uint32_t totalButtons = 0;

  for(uint16_t mi=0; mi<gNumMachines; mi++)
  {
    sMachine *m = &gMachines[mi];
    for(uint16_t numButtonsInUse = 1; numButtonsInUse <= m->numButtons; numButtonsInUse++)
    {
      tButtonBits buttonsInUse = 0;

      if(find_solution(m, numButtonsInUse, &buttonsInUse))
      {
        printf("%hu: %hu\n", mi, numButtonsInUse);
        totalButtons += numButtonsInUse;
        break;
      }
    }
  }

  printf("%u", totalButtons);
}



int compare_joltage(sMachine *m, uint8_t buttonPresses[])
{
  uint16_t joltage[MAX_JOLTAGES] = { 0 };
  for(uint8_t bi=0; bi<m->numButtons; bi++)
  {
    if(buttonPresses[bi] == 0)
      continue;

    for(uint8_t ji=0; ji<m->numJoltages; ji++)
    {
      if(m->buttons[bi] & (1<<ji))
      {
        joltage[ji] += buttonPresses[bi];
        if(joltage[ji] > m->joltages[ji])
          return +1;
      }
    }
  }

  for(uint8_t ji=0; ji<m->numJoltages; ji++)
  {
    if(joltage[ji] < m->joltages[ji])
        return -1;
  }

  return 0;
}

uint16_t sum_presses(uint8_t presses[], uint8_t numButtons)
{
  uint16_t sum=0;
  for(uint8_t i=0; i<numButtons; i++)
    sum += presses[i];
  return sum;
}

void part2(FILE *f)
{
  if(gNumMachines == 0)
    parse_input(f);

  uint32_t totalPresses = 0;

  for(uint16_t mi=0; mi<gNumMachines; mi++)
  {
    clock_t start = clock();

    sMachine *m = &gMachines[mi];
    uint8_t buttonPresses[MAX_BUTTONS] = { 0 };
    uint8_t bpi = 0;
    uint16_t minPresses = UINT16_MAX;

    while(bpi < m->numButtons)
    {
      buttonPresses[bpi]++;
      switch(compare_joltage(m, buttonPresses))
      {
        case 0: //winner
          minPresses = MIN(minPresses, sum_presses(buttonPresses, m->numButtons));
        case -1: //too small
          bpi=0;
          break;

        case +1: //too big
          buttonPresses[bpi++] = 0;
          break;          
      }
    }

    double timeElapsed = ((double) (clock() - start)) / CLOCKS_PER_SEC;
    printf("%hu: %hu (%f)\n", mi, minPresses, timeElapsed);
    totalPresses += minPresses;
  }

  printf("%u", totalPresses);
}
