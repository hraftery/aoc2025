#define DAY 10
#include "main.h"
#include "common.h"

#define MAX_BUTTONS   16
#define MAX_JOLTAGES  16
#define MAX_MACHINES  200
#define MAX_LIGHTS    10  /* informational */

typedef uint16_t tLightBits;
typedef uint16_t tButtonBits;

typedef struct machine {
  uint8_t     numLights;
  tLightBits  targLights;
  //tLightBits  currLights;
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

    //pMachine->currLights = 0x0000;
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


void part2(FILE *f)
{
  printf("This is day %d, part 2.\n", DAY);
}
