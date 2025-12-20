#define DAY 10
#include "main.h"
#include "common.h"
#include "rref.h"

#include <string.h>
#include <math.h>

//#define VERBOSE
#ifdef VERBOSE
#include <time.h>
#endif

#define MAX_BUTTONS   16
#define MAX_JOLTAGES  16
#define MAX_MACHINES  200
#define MAX_LIGHTS    10  /* informational */
#define MAX_JOLTAGE   300 /* actual seems to be 287 */


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
#ifdef VERBOSE
        printf("%hu: %hu\n", mi, numButtonsInUse);
#endif
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

int compare_buttons(const void *a, const void *b)
{
  int aSize = __builtin_popcount(*((tLightBits*)a));
  int bSize = __builtin_popcount(*((tLightBits*)b));
  if(aSize > bSize) return  1;
  if(aSize < bSize) return -1;
  return 0;
}

int compare_buttons_rev(const void *a, const void *b)
{
  int aSize = __builtin_popcount(*((tLightBits*)a));
  int bSize = __builtin_popcount(*((tLightBits*)b));
  if(aSize > bSize) return -1;
  if(aSize < bSize) return  1;
  return 0;
}

uint16_t sum_presses(uint16_t presses[], uint8_t numButtons)
{
  uint16_t sum=0;
  for(uint8_t i=0; i<numButtons; i++)
    sum += presses[i];
  return sum;
}

typedef struct {
  sMachine    *m;
  tMatrixElem (*matFlat)[];//[m->numJoltages][m->numButtons+1]
  uint8_t     numFreeVariables;
  uint8_t     *freeVariables;//[numFreeVariables];
  uint16_t    *fvBounds;//[numFreeVariables];
} sRecursionContext;

//2D arrays lose their 2D-ness when pointed to, so use this define like:
//  MAT_TYPE(ctx->m, mat) = ctx->matFlat;
//where "ctx" is the pointer to sRecursionContext, to get mat back from matFlat.
#define MAT_TYPE(m, mat)  tMatrixElem (*const mat)[m->numButtons+1]


uint16_t do_find_solution2(const sRecursionContext *ctx,
                           uint8_t iFreeVariable, uint16_t solution[ctx->m->numButtons])
{
  if(iFreeVariable == ctx->numFreeVariables) //finished recursing, so solve the completed matrix
  {
    MAT_TYPE(ctx->m, mat) = ctx->matFlat;
    for(uint8_t iRow=0; iRow<ctx->m->numJoltages; iRow++)
    {
      tMatrixElem *row = mat[iRow];
      //Now all the free variables are set, solve:
      //  a*1 + b*f1 + c*f2 = j  =>  a = j - b*f1 - c*f2
      //Find the index of 1, the lead.
      uint8_t iLead;
      for(iLead=iRow; iLead<ctx->m->numButtons; iLead++)
        if(row[iLead] == 1)
          break;
      //Now do the calc in floats, because mat entries don't have to be whole numbers
      double a = row[ctx->m->numButtons]; //j
      for(uint8_t fvi=0; fvi<ctx->numFreeVariables; fvi++)
        a -= solution[ctx->freeVariables[fvi]] * row[ctx->freeVariables[fvi]];
      //We only accept solutions that are positive integers
      double aInt = round(a);
      if(aInt < 0 || fabs(aInt - a) > 0.0001f)
        return UINT16_MAX; //no solution
      solution[iLead] = aInt;
    }
    return sum_presses(solution, ctx->m->numButtons);
  }
  
  //Otherwise, continue recursing
  uint16_t minPresses = UINT16_MAX;
  uint16_t thisSolution[ctx->m->numButtons];
  //Turns out we only need to make one copy, because we update the same element each loop
  memcpy(thisSolution, solution, sizeof(thisSolution));

  for(uint16_t fv=0; fv<=ctx->fvBounds[iFreeVariable]; fv++)
  {
    thisSolution[ctx->freeVariables[iFreeVariable]] = fv; //Fill in freeVariable
    uint16_t thisPresses = do_find_solution2(ctx, iFreeVariable+1, thisSolution); //And recurse

    if(thisPresses < minPresses)
    {
      minPresses = thisPresses;
      memcpy(solution, thisSolution, sizeof(thisSolution)); //allow solution to bubble up
    }
  }

  return minPresses;
}

uint16_t find_solution2(const sRecursionContext *ctx, uint16_t solution[ctx->m->numButtons])
{
  //Kick off the recursion with an empty solution
  for(uint8_t i=0; i<ctx->m->numButtons; i++)
    solution[i] = 0;
  return do_find_solution2(ctx, 0, solution);
}

void part2(FILE *f)
{
  if(gNumMachines == 0)
    parse_input(f);

  uint32_t totalPresses = 0;

  for(uint16_t mi=0; mi<gNumMachines; mi++)
  {
#ifdef VERBOSE
    clock_t start = clock();
#endif

    sMachine *m = &gMachines[mi];

    //Characteristic matrix has buttons as columns, augmented with joltages.
    tMatrixSize matRows = m->numJoltages;
    tMatrixSize matCols = m->numButtons+1;
    tMatrixElem mat[matRows][matCols];

    for(uint8_t bi=0; bi<m->numButtons; bi++)
      for(uint8_t ji=0; ji<m->numJoltages; ji++)
        mat[ji][bi] = m->buttons[bi] & (1<<ji) ? 1.0 : 0.0;
    for(uint8_t ji=0; ji<m->numJoltages; ji++)
      mat[ji][m->numButtons] = m->joltages[ji];

    RREF_solve(matRows, matCols, mat);

    //Find free variables
    bool isBound[MAX_BUTTONS] = { false };
    for(uint8_t ji=0; ji<m->numJoltages; ji++)
    {
      for(uint8_t bi=ji; bi<m->numButtons; bi++)
      {
        if(mat[ji][bi] == 1) //found a bound variable
        {
          isBound[bi] = true;
          break;
        }
      }
    }
    //Now collect them
    uint8_t freeVariables[MAX_BUTTONS];
    uint8_t numFreeVariables = 0;
    for(uint8_t bi=0; bi<m->numButtons; bi++)
    {
      if(!isBound[bi])
        freeVariables[numFreeVariables++] = bi;
    }
    //Now find their upper bounds. There's lower bounds too, but we'll simply use zero.
    uint16_t fvBounds[numFreeVariables];
    //Init to MAX_JOLTAGE, because there's no point pressing any button more than MAX_JOLTAGE times.
    for(int i=0; i<numFreeVariables; i++)
      fvBounds[i] = MAX_JOLTAGE;
    
    for(int i=0; i<2; i++) //apply bounds twice, to ensure bounds from first pass are applied
    {
      for(uint8_t rowi=0; rowi<matRows; rowi++)
      {
        tMatrixElem *row = mat[rowi];
        for(uint8_t fvi=0; fvi<numFreeVariables; fvi++)
        {
          //Because mat is in RREF, each row is equation of form (assuming two free variables):
          //  a*1 + b*f1 + c*f2 = j  =>  b*f1 = j - a - c*f2
          //Where:
          //  • a, b and c are number of presses of lead button, free variable 1 button and
          //    free variable 2 button respectively
          //  • f1 and f2 are coefficients of free variable buttons
          //  • j is joltage for that row
          //So max b can be is either:
          //  b_max*f1 = j - a_min - c_min*f2 = j                            if f1 is +ve and f2 is +ve, or
          //  b_max*f1 = j - a_min - c_max*f2 = j - c_max*f2                 if f1 is +ve and f2 is -ve, or
          //  b_max*f1 = j - a_max - c_max*f2 = j - MAX_JOLTAGE - c_max*f2   if f1 is -ve and f2 is +ve, or
          //  b_max*f1 = j - a_max - c_min*f2 = j - MAX_JOLTAGE              if f1 is -ve and f2 is -ve, or
          //So in general, the max is j, maybe minus MAX_JOLTAGE, minus the max of all other free
          //variables with opposite sign, all divided by the coefficient.
          double f = row[freeVariables[fvi]];
          if(f == 0) //special case, no constraint to apply
            continue;
          bool isNeg = f < 0;
          tMatrixElem max = row[matCols-1] - (isNeg ? MAX_JOLTAGE : 0); //j, maybe minus MAX_JOLTAGE
          for(uint8_t fvj=0; fvj<numFreeVariables; fvj++)
          {
            if(fvj==fvi)
              continue;
            if(row[freeVariables[fvj]] < 0 != isNeg) //opposite sign
              max -= row[freeVariables[fvj]] * fvBounds[fvj];
          }
          max /= f;
          if(max >= 0) //okay to floor to integer, because there's slack in MAX_JOLTAGE.
            fvBounds[fvi] = MIN((uint16_t)max, fvBounds[fvi]);
        }
      }
    }
    
    //Well finally! Solve the matrix, searching within fvBounds to find optimal (least presses) solution.
    const sRecursionContext ctx = { m, mat, numFreeVariables, freeVariables, fvBounds };
    uint16_t buttonPresses[m->numButtons];
    uint16_t minPresses = find_solution2(&ctx, buttonPresses);

#ifdef VERBOSE
    double timeElapsed = ((double) (clock() - start)) / CLOCKS_PER_SEC;
    printf("%hu: %hu (%f)\n", mi, minPresses, timeElapsed);
#endif
    totalPresses += minPresses;
  }

  printf("%u", totalPresses);
}
