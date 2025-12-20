#include "rref.h"

#include <string.h>
#include <stdio.h>

#define SWAP_ELEM(a, b) do { tMatrixElem tmp = a; (a) = b; b = tmp; } while (0)

void RREF_solve(tMatrixSize numRows, tMatrixSize numCols, tMatrixElem mat[numRows][numCols])
{ 
  int leadIdx = 0;
  for(tMatrixSize i=0; i<numRows; i++)
  {
    //Find next lead
    tMatrixSize j = i;
    while(mat[j][leadIdx] == 0)
    {
      j++;
      if(j == numRows)
      {
        j = i;
        leadIdx++;
        if(leadIdx == numCols)
          return;
      }
    }

    //If we found the lead in another row, swap rows j and i
    if(i != j)
      for(tMatrixSize k=leadIdx; k<numCols; k++)
        SWAP_ELEM(mat[j][k], mat[i][k]);
    
    //Normalise row i
    tMatrixElem lead = mat[i][leadIdx];
    if(lead != 1)
      for(tMatrixSize k=leadIdx; k<numCols; k++)
        mat[i][k] /= lead;
    
    //Use row i to reduce other rows
    for(j=0; j<numRows; j++)
    {
      if(j != i)
      {
        lead = mat[j][leadIdx];
        if(lead != 0)
          for(tMatrixSize k=leadIdx; k<numCols; k++)
            mat[j][k] -= lead * mat[i][k];
      }
    }

    if(++leadIdx >= numCols-1)
      break;
  }
}

void RREF_print(tMatrixSize numRows, tMatrixSize numCols, tMatrixElem mat[numRows][numCols])
{
  for(tMatrixSize row=0; row<numRows; row++)
    for(tMatrixSize col=0; col<numCols; col++)
      printf("mat[%hu][%hu]=%f\n", row, col, mat[row][col]);
}

bool RREF_test(void)
{
  tMatrixElem mat1[][8] = {
    { 1, 1, 0, 1, 1, 1, 0, 63 },
    { 1, 1, 0, 1, 1, 0, 1, 62 },
    { 1, 1, 0, 0, 1, 0, 0, 36 },
    { 1, 0, 1, 1, 0, 1, 0, 39 },
    { 0, 1, 1, 0, 0, 0, 1, 35 }
  };

  RREF_solve(sizeof(mat1)/sizeof(mat1[0]), sizeof(mat1[0])/sizeof(mat1[0][0]), mat1);

  tMatrixElem exp1[][8] = {
    { 1, 0, 0, 0,  0.5, 0, -0.5,  6.5 },
    { 0, 1, 0, 0,  0.5, 0,  0.5, 29.5 },
    { 0, 0, 1, 0, -0.5, 0,  0.5,  5.5 },
    { 0, 0, 0, 1, -0  , 0,  1  , 26   },
    { 0, 0, 0, 0,  0  , 1, -1  ,  1   }
  };

  tMatrixElem mat2[][4] = {
    {  1, 2, -1,  -4 },
    {  2, 3, -1, -11 },
    { -2, 0, -3,  22 }
  };

  RREF_solve(sizeof(mat2)/sizeof(mat2[0]), sizeof(mat2[0])/sizeof(mat2[0][0]), mat2);

  tMatrixElem exp2[][8] = {
    { 1, 0, 0, -8 },
    { 0, 1, 0,  1 },
    { 0, 0, 1, -2 }
  };

  return (memcmp(mat1, exp1, sizeof(exp1)) == 0) &&
         (memcmp(mat2, exp2, sizeof(exp2)) == 0);
}
