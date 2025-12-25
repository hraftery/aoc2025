#define DAY 12
#include "main.h"
#include "common.h"

#define MAX_LINE_LENGTH   100
#define MAX_SHAPES        6
#define MAX_ROTATIONS     8
#define MAX_REGIONS       1000
#define MAX_REGION_WIDTH  50
#define MAX_REGION_LENGTH 50

// bit: 76543210
// LSB: xxxxxXXX
//    : xxxxxXXX
//    : xxxxxXXX
// MSB: not used
typedef uint32_t shape_t;
const uint32_t SHAPE_ROW_MASK = 0x00000007; //0b0111 in LSB

typedef struct {
  uint8_t width;
  uint8_t length;
  uint8_t shapeCounts[MAX_SHAPES];
} sRegion;

typedef struct {
  shape_t  shapes[MAX_SHAPES];
  uint8_t  numShapes;
  sRegion  regions[MAX_REGIONS];
  uint8_t  numRegions;
} sPuzzle;

typedef struct {
  shape_t rotations[MAX_ROTATIONS];
  uint8_t numRotations;
} sShapeRotations;

void print_shape(shape_t s)
{
  uint8_t row[] = {
    (s >> 0) & 0x000000FF,
    (s >> 8) & 0x000000FF,
    (s >>16) & 0x000000FF
  };
  
  for(int i=0; i<3; i++)
    printf("%c%c%c\n", (row[i] & 0x04) ? 'X' : '.',
                       (row[i] & 0x02) ? 'X' : '.',
                       (row[i] & 0x01) ? 'X' : '.');
}

void print_shapes(uint8_t numShapes, shape_t s[numShapes])
{
  for(int irow=0; irow<3; irow++)
  {
    for(int is=0; is<numShapes; is++)
    {
      uint8_t row = (s[is] >> (8*irow)) & 0x000000FF;
      printf("%c%c%c ", (row & 0x04) ? 'X' : '.',
                        (row & 0x02) ? 'X' : '.',
                        (row & 0x01) ? 'X' : '.');
    }
    printf("\n");
  }
}

sPuzzle* parse_input(FILE *f)
{
  char line[MAX_LINE_LENGTH];
  static sPuzzle p;

  p.numShapes = 0;
  uint32_t index;
  while(1)
  {
    fgets(line, MAX_LINE_LENGTH, f);
    char c;
    if(sscanf(line, "%u%c", &index, &c) != 2)
      exit(EXIT_FAILURE);
    if(c != ':')
      break;
    
    shape_t s=0;
    for(int i=0; i<3; i++)
    {
      fgets(line, MAX_LINE_LENGTH, f);
      for(int j=0; j<3; j++)
        s |= (line[j] == '#' ? 1<<(i*8+2-j) : 0);
    }
    
    fgets(line, MAX_LINE_LENGTH, f); //empty line

    p.shapes[p.numShapes++] = s;
  }

  do {
    int consumed;
    uint32_t width, length;
    if(sscanf(line, "%ux%u:%n", &width, &length, &consumed) != 2)
      exit(EXIT_FAILURE);
    
    sRegion r = { .width = width, .length = length };

    char *pLine = &line[consumed];
    for(int i=0; i<p.numShapes; i++)
    {
      uint32_t count;
      if(sscanf(pLine, "%u%n", &count, &consumed) != 1)
        exit(EXIT_FAILURE);
      
      r.shapeCounts[i] = count;
      pLine += consumed;
    }

    p.regions[p.numRegions++] = r;
  } while(fgets(line, MAX_LINE_LENGTH, f) != NULL);

  return &p;
}

shape_t rotate_90deg_cw(shape_t s)
{
  uint8_t irow0 = (s >> 0) & 0x000000FF;
  uint8_t irow1 = (s >> 8) & 0x000000FF;
  uint8_t irow2 = (s >>16) & 0x000000FF;
  uint8_t orow0 = ((irow2 & 0x04) >> 0) | ((irow1 & 0x04) >> 1) | ((irow0 & 0x04) >> 2);
  uint8_t orow1 = ((irow2 & 0x02) << 1) | ((irow1 & 0x02) >> 0) | ((irow0 & 0x02) >> 1);
  uint8_t orow2 = ((irow2 & 0x01) << 2) | ((irow1 & 0x01) << 1) | ((irow0 & 0x01) << 0);

  return (orow2 << 16) | (orow1 << 8) | (orow0 << 0);
}

shape_t flip_horizontal(shape_t s)
{
  uint8_t irow0 = (s >> 0) & 0x000000FF;
  uint8_t irow1 = (s >> 8) & 0x000000FF;
  uint8_t irow2 = (s >>16) & 0x000000FF;
  uint8_t orow0 = irow2;
  uint8_t orow1 = irow1;
  uint8_t orow2 = irow0;

  return (orow2 << 16) | (orow1 << 8) | (orow0 << 0);
}

void make_rotations(sPuzzle *p, sShapeRotations r[p->numShapes])
{
  for(int i=0; i<p->numShapes; i++)
  {
    //Seed rotations with the shape as given.
    shape_t s = p->shapes[i];
    r[i].numRotations = 0;
    r[i].rotations[r[i].numRotations++] = s;

    for(int rot=90; rot<360; rot+=90)
    {
      shape_t s90 = rotate_90deg_cw(s);
      for(int j=0; j<r[i].numRotations; j++)
        if(s90 == r[i].rotations[j])
          goto rot_symmetric;
      
      r[i].rotations[r[i].numRotations++] = s90;
rot_symmetric:
      s = s90;
    }

    shape_t sflip = flip_horizontal(s);
    for(int j=0; j<r[i].numRotations; j++)
      if(sflip == r[i].rotations[j])
        goto flip_symmetric;
    
    r[i].rotations[r[i].numRotations++] = sflip;
    for(int rot=90; rot<360; rot+=90)
    {
      shape_t s90 = rotate_90deg_cw(sflip);
      for(int j=0; j<r[i].numRotations; j++)
        if(s90 == r[i].rotations[j])
          goto flip_rot_symmetric;
      
      r[i].rotations[r[i].numRotations++] = s90;
flip_rot_symmetric:
      sflip = s90;
    }

flip_symmetric:;
    // printf("\n");
    // print_shapes(r[i].numRotations, r[i].rotations);
  }
}

typedef uint64_t bit_field_t;
#define SO_BF (sizeof(bit_field_t))

typedef struct {
  sRegion         *pReg;
  sShapeRotations *rotations;
  uint8_t         numShapes;
} sRecursionContext;

typedef struct {
  uint8_t     bfRows;
#ifdef WIDTH_NOT_LESS_THAN_SO_BF
  uint8_t     bfCols;
  bit_field_t regBitField[MAX_REGION_LENGTH][ROUND_UP(MAX_REGION_WIDTH, SO_BF) / SO_BF];
#else
  bit_field_t regBitField[MAX_REGION_LENGTH];
#endif
  uint8_t     shapeIdx;
  uint8_t     shapeCountIdx;
} sRecursionState;

void print_recursion_state(sRecursionContext *pCtx, sRecursionState *pSt)
{
  for(int irow=0; irow<pSt->bfRows; irow++)
  {
    for(int icol=0; icol<pCtx->pReg->width; icol++)
      printf("%c", GET_BIT_IDX(pSt->regBitField[irow], icol) ? 'X' : '.');
    printf("\n");
  }
}

bool do_find_fit(sRecursionContext *pCtx, sRecursionState *pSt)
{
  sShapeRotations *pRot = &pCtx->rotations[pSt->shapeIdx];

  for(int irot=0; irot<pRot->numRotations; irot++)
  {
    shape_t s = pRot->rotations[irot];

    for(int x=0; x<pCtx->pReg->width-2; x++)
    {
      for(int y=0; y<pCtx->pReg->length-2; y++)
      {
        //HUGE simplification if x is always < SO_BF, which I think it is. No division,
        //no mod, and also no need for separate x0, x1, x2. Will the compiler realise?
#ifdef X_NOT_LESS_THAN_SO_BF
        shape_t bounds_x0y0 = (regBitField[y+0][(x+0)/SO_BF] >> ((x+0)%SO_BF)) & 0x01;
        shape_t bounds_x1y0 = (regBitField[y+0][(x+1)/SO_BF] >> ((x+1)%SO_BF)) & 0x01;
        shape_t bounds_x2y0 = (regBitField[y+0][(x+2)/SO_BF] >> ((x+2)%SO_BF)) & 0x01;
        shape_t bounds_x0y1 = (regBitField[y+1][(x+0)/SO_BF] >> ((x+0)%SO_BF)) & 0x01;
        shape_t bounds_x1y1 = (regBitField[y+1][(x+1)/SO_BF] >> ((x+1)%SO_BF)) & 0x01;
        shape_t bounds_x2y1 = (regBitField[y+1][(x+2)/SO_BF] >> ((x+2)%SO_BF)) & 0x01;
        shape_t bounds_x0y2 = (regBitField[y+2][(x+0)/SO_BF] >> ((x+0)%SO_BF)) & 0x01;
        shape_t bounds_x1y2 = (regBitField[y+2][(x+1)/SO_BF] >> ((x+1)%SO_BF)) & 0x01;
        shape_t bounds_x2y2 = (regBitField[y+2][(x+2)/SO_BF] >> ((x+2)%SO_BF)) & 0x01;
        shape_t bounds = (bounds_x2y2 << 18) | (bounds_x1y2 << 17) | (bounds_x0y2 << 16) | 
                          (bounds_x2y1 << 10) | (bounds_x1y1 <<  9) | (bounds_x0y1 <<  8) | 
                          (bounds_x2y0 <<  2) | (bounds_x1y0 <<  1) | (bounds_x0y0 <<  0);
#else
        shape_t bounds_y0 = (pSt->regBitField[y+0] >> x) & SHAPE_ROW_MASK;
        shape_t bounds_y1 = (pSt->regBitField[y+1] >> x) & SHAPE_ROW_MASK;
        shape_t bounds_y2 = (pSt->regBitField[y+2] >> x) & SHAPE_ROW_MASK;
        shape_t bounds = (bounds_y2 << 16) | (bounds_y1 << 8) | (bounds_y0 << 0);
#endif

        if((bounds & s) == 0) //No overlap. This is, it fits here.
        {          
          sRecursionState st = *pSt; //make copy for recursion
          if(++st.shapeCountIdx == pCtx->pReg->shapeCounts[st.shapeIdx])
          {
            st.shapeCountIdx = 0;
            do {
              if(++st.shapeIdx == pCtx->numShapes) //done!
                return true;
            } while(pCtx->pReg->shapeCounts[st.shapeIdx] == 0);
          }
          
#ifdef WIDTH_NOT_LESS_THAN_SO_BF
          TODO
#else
          st.regBitField[y+0] |= ((((bit_field_t)s >> 0) & SHAPE_ROW_MASK) << x);
          st.regBitField[y+1] |= ((((bit_field_t)s >> 8) & SHAPE_ROW_MASK) << x);
          st.regBitField[y+2] |= ((((bit_field_t)s >>16) & SHAPE_ROW_MASK) << x);
#endif

          printf("\x1b[%dF", pCtx->pReg->length);
          print_recursion_state(pCtx, &st);
          //printf("\x1b[2KLine 1: New text\n");
          fflush(stdout); // Ensure new output is written immediately
          
          // printf("\n");
          // print_recursion_state(pCtx, &st);

          if(do_find_fit(pCtx, &st))
            return true;
        }
      }//y
    }//x
  }//irot

  //Tried all y's for all x's for all rotations, and still didn't fit.
  return false;
}

bool find_fit(sRecursionContext *pCtx)
{
  sRecursionState st = {
    .bfRows = pCtx->pReg->length,
#ifdef WIDTH_NOT_LESS_THAN_SO_BF
    .bfCols = ROUND_UP(pCtx->pReg->width, SO_BF) / SO_BF, //might always be zero?
#endif
    .shapeIdx = 0,
    .shapeCountIdx = 0
  };

  for(int row=0; row<st.bfRows; row++)
#ifdef WIDTH_NOT_LESS_THAN_SO_BF
      for(int col=0; col<st.bfCols; col++)
        st.regBitField[row][col] = 0; //start with vacant region
#else
    st.regBitField[row] = 0; //start with vacant region
#endif

  while(pCtx->pReg->shapeCounts[st.shapeIdx] == 0)
    st.shapeIdx++;

  printf("\n");
  print_recursion_state(pCtx, &st);

  return do_find_fit(pCtx, &st);
}

sPuzzle* gPuzzle = NULL;

void part1(FILE *f)
{
  sPuzzle* gPuzzle = parse_input(f);

  sShapeRotations rotations[gPuzzle->numShapes];
  make_rotations(gPuzzle, rotations);

  int numFit = 0;
  
  for(int ireg=0; ireg<gPuzzle->numRegions; ireg++)
  //for(int ireg=0; ireg<1; ireg++)
  {
    sRecursionContext ctx = {
      .pReg         = &gPuzzle->regions[ireg],
      .rotations    = rotations,
      .numShapes    = gPuzzle->numShapes
    };

    bool didFit = find_fit(&ctx);
    printf("%d: %c\n", ireg, didFit ? 'Y' : 'N');
    if(didFit)
      numFit++;
  }
  
  printf("%d", numFit);
}

void part2(FILE *f)
{
  if(!gPuzzle)
    gPuzzle = parse_input(f);

  printf("This is day %d, part 2.\n", DAY);
}
