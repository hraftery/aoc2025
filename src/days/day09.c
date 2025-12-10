#define DAY 9
#include "main.h"
#include "common.h"

#define MAX_RED_TILES 100

typedef struct {
  uint32_t x;
  uint32_t y;
} sCoord2D;

sCoord2D gRedTiles[MAX_RED_TILES];
int gNumRedTiles = 0;

void part1(FILE *f)
{
  uint32_t x, y;
  //Read location of all boxes
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

void part2(FILE *f)
{
  printf("This is day %d, part 2.\n", DAY);
}
