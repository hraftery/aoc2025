#include <stdint.h>
#include <stdbool.h>

typedef double        tMatrixElem;
typedef uint16_t      tMatrixSize;
typedef tMatrixElem** tMatrix;

void RREF_solve(tMatrixSize numRows, tMatrixSize numCols, tMatrixElem mat[numRows][numCols]);
bool RREF_test(void);
