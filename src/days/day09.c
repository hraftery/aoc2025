#define DAY 9
#include "main.h"
#include "common.h"

#include "intset.h"

#include <time.h>

#define MAX_RED_TILES 500
#define MAX_COLUMNS   100000
#define MAX_ROWS      100000

typedef struct {
  uint32_t x;
  uint32_t y;
} sCoord2D;

typedef struct boundsNode {
  uint32_t dx;
  uint32_t dy;
  struct boundsNode *next;
} sBoundsNode;


sCoord2D gRedTiles[MAX_RED_TILES];
int gNumRedTiles = 0;

void part1(FILE *f)
{
  uint32_t x, y;
  //Read location of all tiles
  while(fscanf(f, "%u,%u", &x, &y) == 2)
    gRedTiles[gNumRedTiles++] = (sCoord2D){ .x=x, .y=y };

  uint64_t maxArea = 0;
  for(int i=0; i<gNumRedTiles; i++)
  {
    for(int j=0; j<i; j++)
    {
      uint64_t minx = MIN(gRedTiles[i].x, gRedTiles[j].x);
      uint64_t maxx = MAX(gRedTiles[i].x, gRedTiles[j].x);
      uint64_t miny = MIN(gRedTiles[i].y, gRedTiles[j].y);
      uint64_t maxy = MAX(gRedTiles[i].y, gRedTiles[j].y);
      uint64_t area = (1ull + maxx - minx) * (1ull + maxy - miny);
      maxArea = MAX(maxArea, area);
    }
  }

  printf("%llu", maxArea);
}


typedef struct vertLine {
  uint32_t          yMin;
  uint32_t          yMax;
  struct vertLine*  next;
} sVertLine;

sVertLine* gVertLines[MAX_COLUMNS] = { NULL };
tIntSet    gRowsOfInsideTiles[MAX_ROWS] = { NULL };

bool is_on_line(uint32_t row, uint32_t col)
{
  sVertLine *pLine = gVertLines[col];
  while(pLine != NULL && pLine->yMin < row)
  {
    if(pLine->yMax >= row)
      return true;
    pLine = pLine->next;
  }
  return false;
}

void free_bounds_list(sBoundsNode *pNode)
{
  pNode = pNode->next; //Skip the fisrt one, which is on the stack
  while(pNode) {
    sBoundsNode *next = pNode->next; //save it before we free pNode
    free(pNode);
    pNode = next;
  };

}

void print_grid(void)
{
  printf("\n");
  for(uint32_t row=0; row<9; row++)
  {
    tIntSet rowTiles = gRowsOfInsideTiles[row];
    for(uint32_t col=0; col<14; col++)
      printf("%c", INTSET_contains(rowTiles, col) ? 'X' : '.');
    printf("\n");
  }
}

void part2(FILE *f)
{
  uint32_t x, y;
  //Read location of all tiles
  while(fscanf(f, "%u,%u", &x, &y) == 2)
    gRedTiles[gNumRedTiles++] = (sCoord2D){ .x=x, .y=y };

  printf("\nRead locations.\n");

  //Create store of vertical edges
  for(int i=0; i<gNumRedTiles; i++)
  {
    int j = (i+1)%gNumRedTiles;
    if(gRedTiles[i].x == gRedTiles[j].x) //vertical
    {
      //Make new line
      sVertLine *pNewLine = malloc(sizeof(sVertLine));
      pNewLine->yMin = MIN(gRedTiles[i].y, gRedTiles[j].y);
      pNewLine->yMax = MAX(gRedTiles[i].y, gRedTiles[j].y);
      
      //And find where to put it
      sVertLine **hLine = &gVertLines[gRedTiles[i].x];
      while(*hLine != NULL)
      {
        if((*hLine)->yMin > pNewLine->yMin) //insert it here
          break;
        hLine = &((*hLine)->next); //traverse the linked list
      }
      pNewLine->next = *hLine;
      *hLine = pNewLine;
    }
  }

  printf("Created vertical lines.\n");

  //Use the vertical lines to mark tiles "inside"
  for(uint32_t row=0; row<MAX_ROWS; row++)
  {
    tIntSet rowTiles = INTSET_new(MAX_COLUMNS);
    bool inside = false; //start outside
    for(uint32_t col=0; col<MAX_COLUMNS; col++)
    {
      if(is_on_line(row, col))
        inside = !inside;
      if(inside)
        INTSET_add(rowTiles, col);
    }
    gRowsOfInsideTiles[row] = rowTiles;
  }

  printf("Marked inside tiles between vertical lines.\n");

  //Now fill in the lines themselves. We only actually need to add the corners
  //and the top and right hand edges but easiest just to do them all.
  for(int i=0; i<gNumRedTiles; i++)
  {
    int j = (i+1)%gNumRedTiles;

    uint32_t rowMin = MIN(gRedTiles[i].y, gRedTiles[j].y);
    uint32_t rowMax = MAX(gRedTiles[i].y, gRedTiles[j].y);
    uint32_t colMin = MIN(gRedTiles[i].x, gRedTiles[j].x);
    uint32_t colMax = MAX(gRedTiles[i].x, gRedTiles[j].x);

    //Only of these loops will just have a single interation.
    for(uint32_t row=rowMin; row<=rowMax; row++)
      for(uint32_t col=colMin; col<=colMax; col++)
        INTSET_add(gRowsOfInsideTiles[row], col);
  }

  printf("Marked inside tiles on lines.\n");

  //print_grid();

  //gRowsOfInsideTiles is complete. Ready to roll!
  clock_t start;
  uint64_t maxArea = 0;

  start = clock();
  for(int i=0; i<gNumRedTiles; i++)
  {
    clock_t end = clock();
    double timeElapsed = ((double) (clock() - start)) / CLOCKS_PER_SEC;
    start = end;
    printf("%f: Pairing with tile %d of %d.\n", timeElapsed, i, gNumRedTiles);
    sBoundsNode boundsNE = {UINT32_MAX, UINT32_MAX, NULL};
    sBoundsNode boundsSE = {UINT32_MAX, UINT32_MAX, NULL};
    sBoundsNode boundsSW = {UINT32_MAX, UINT32_MAX, NULL};
    sBoundsNode boundsNW = {UINT32_MAX, UINT32_MAX, NULL};

    for(int j=0; j<i; j++)
    {
      sBoundsNode* bounds;
      //Only here, make the arbitrary choice that N = -ve and E = +ve.
      if     (gRedTiles[j].x > gRedTiles[i].x && gRedTiles[j].y < gRedTiles[i].y) 
        bounds = &boundsNE;
      else if(gRedTiles[j].x > gRedTiles[i].x && gRedTiles[j].y > gRedTiles[i].y) 
        bounds = &boundsSE;
      else if(gRedTiles[j].x < gRedTiles[i].x && gRedTiles[j].y < gRedTiles[i].y) 
        bounds = &boundsNW;
      else if(gRedTiles[j].x < gRedTiles[i].x && gRedTiles[j].y > gRedTiles[i].y) 
        bounds = &boundsSW;
      else
        goto skip; //don't worry about N, E, S, W - they're never going to be max area.

      uint64_t minx = MIN(gRedTiles[i].x, gRedTiles[j].x);
      uint64_t maxx = MAX(gRedTiles[i].x, gRedTiles[j].x);
      uint64_t miny = MIN(gRedTiles[i].y, gRedTiles[j].y);
      uint64_t maxy = MAX(gRedTiles[i].y, gRedTiles[j].y);

      uint32_t dx = maxx - minx;
      uint32_t dy = maxy - miny;
      do {
        if(dx >= bounds->dx && dy >= bounds->dy) //don't bother checking anything beyond a bound
          goto skip;
      } while(bounds->next && (bounds = bounds->next));

      
      for(uint64_t row=miny; row<=maxy; row++)
      {
        for(uint64_t col=minx; col<=maxx; col++)
        {
          if(!INTSET_contains(gRowsOfInsideTiles[row], col))
          {
            sBoundsNode* pNewBound = malloc(sizeof(sBoundsNode));
            *pNewBound = (sBoundsNode){dx, dy, NULL};
            bounds->next = pNewBound;
            goto skip;
          }
        }
      }

      uint64_t area = (1ull + maxx - minx) * (1ull + maxy - miny);
      maxArea = MAX(maxArea, area);

      skip:;
    }
    
    free_bounds_list(&boundsNE);
    free_bounds_list(&boundsSE);
    free_bounds_list(&boundsSW);
    free_bounds_list(&boundsNW);
  }

  printf("%llu", maxArea);
}
