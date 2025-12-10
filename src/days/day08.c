#define DAY 8
#include "main.h"
#include "common.h"

#include "intset.h"

#include <string.h>

#define MAX_BOXES     1000
#define MAX_EDGES     10000
#define MAX_SUBGRAPHS (MAX_BOXES/2)
#define MAX_LARGEST_CIRCUITS 3

typedef struct {
  uint32_t x;
  uint32_t y;
  uint32_t z;
} sCoord3D;

typedef struct {
  size_t nodeA;
  size_t nodeB;
  uint64_t distSquared;
} sEdge;

sCoord3D gBoxes[MAX_BOXES];
int gNumBoxes = 0;
sEdge gShortestEdges[MAX_EDGES];
tIntSet gSubGraphs[MAX_SUBGRAPHS];
int gNumSubGraphs = 0;
tIntsetElem gLargestSubGraphs[MAX_LARGEST_CIRCUITS];

//Turns out it's not needed!
//uint64_t gDistSquared[MAX_BOXES][MAX_BOXES]; //A cool 8MB

void load_boxes(FILE *f)
{
  uint32_t x, y, z;
  //Read location of all boxes
  while(fscanf(f, "%u,%u,%u", &x, &y, &z) == 3)
    gBoxes[gNumBoxes++] = (sCoord3D){ .x=x, .y=y, .z=z };
}

void find_shortest_edges(const int NUM_EDGES)
{
  //Calculate an adjacency matrix of distances between boxes.
  //As we go, keep a sorted array of the N shortest distances.
  for(int i=0; i<NUM_EDGES; i++)
    gShortestEdges[i].distSquared = UINT64_MAX; //Prime with longest distance
  
  for(int i=0; i<gNumBoxes; i++)
  {
    for(int j=0; j<i; j++)
    {
      uint64_t ix = gBoxes[i].x, iy = gBoxes[i].y, iz = gBoxes[i].z;
      uint64_t jx = gBoxes[j].x, jy = gBoxes[j].y, jz = gBoxes[j].z;
      //Note multiplying two negative numbers is safe to do in unsigned arithmetic.
      uint64_t distSquared = (ix-jx)*(ix-jx) + (iy-jy)*(iy-jy) + (iz-jz)*(iz-jz);
      //gDistSquared[i][j] = gDistSquared[j][i] = distSquared; //Adjacency matrix is symmetric.

      //If this distance is less than any in the array, then slot it in
      if(distSquared < gShortestEdges[NUM_EDGES-1].distSquared)
      {
        sEdge scratch[NUM_EDGES];
        memcpy(scratch, gShortestEdges, sizeof(scratch));

        for(int e=0; e<NUM_EDGES; e++)
        {
          if(distSquared < gShortestEdges[e].distSquared) //slot it in here
          {
            //insert the new and then shuffle the rest back one spot
            //j<i so nodeA is always < nodeB
            gShortestEdges[e] = (sEdge){ .nodeA = j, .nodeB = i, .distSquared = distSquared };
            memcpy(&gShortestEdges[e+1], &scratch[e], sizeof(sEdge) * (NUM_EDGES - e - 1));
            break;
          }
        }
      }
    }
  }
}

void add_connection(sEdge *newConn)
{
  //Three things can happen:
  //  case 0: The edge connects to no existing subgraph, so forms a new one.
  //  case 1: The edge connects to (or within) a single subgraph, so expands it (or is redundant).
  //  case 2: The edge connects two subgraphs, combining them into one.
  int matchingSubGraphs[2];
  int numMatchingSubGraphs = 0;
  for(int sg=0; sg<gNumSubGraphs; sg++)
  {
    if(INTSET_contains(gSubGraphs[sg], newConn->nodeA) ||
       INTSET_contains(gSubGraphs[sg], newConn->nodeB))
    {
      matchingSubGraphs[numMatchingSubGraphs++] = sg;
      if(numMatchingSubGraphs == 2) //no point looking further
        break;
    }
  }
  switch(numMatchingSubGraphs)
  {
    case 0: {
      tIntSet newSubGraph = INTSET_new(MAX_BOXES);
      INTSET_add(newSubGraph, newConn->nodeA);
      INTSET_add(newSubGraph, newConn->nodeB);
      gSubGraphs[gNumSubGraphs++] = newSubGraph;
      break;
    }
    
    case 1: {
      tIntSet existingSubGraph = gSubGraphs[matchingSubGraphs[0]];
      //One (or both) of these nodes will be redundant.
      INTSET_add(existingSubGraph, newConn->nodeA);
      INTSET_add(existingSubGraph, newConn->nodeB);
      break;
    }
    
    case 2: {
      tIntSet firstSubGraph  = gSubGraphs[matchingSubGraphs[0]];
      tIntSet secondSubGraph = gSubGraphs[matchingSubGraphs[1]];
      INTSET_union(firstSubGraph, secondSubGraph);
      INTSET_delete(secondSubGraph);
      for(int i=matchingSubGraphs[1]; i<gNumSubGraphs-1; i++)
        gSubGraphs[i] = gSubGraphs[i+1]; //Shuffle the rest down
      gNumSubGraphs--;
      break;
    }
    
    default:
      printf("OH NO!\n");
  }
}

void find_largest_subgraphs(const int NUM_LARGEST_CIRCUITS)
{
  for(int i=0; i<NUM_LARGEST_CIRCUITS; i++)
    gLargestSubGraphs[i] = 1; //prime with lowest size

  for(int sg=0; sg<gNumSubGraphs; sg++)
  {
    tIntsetElem size = INTSET_size(gSubGraphs[sg]);
    //printf("sg[%u] = %u.\n", sg, size);
    if(size > gLargestSubGraphs[NUM_LARGEST_CIRCUITS-1])
    {
      tIntsetElem scratch[NUM_LARGEST_CIRCUITS];
      memcpy(scratch, gLargestSubGraphs, sizeof(scratch));

      for(int i=0; i<NUM_LARGEST_CIRCUITS; i++)
      {
        if(size > gLargestSubGraphs[i]) //slot it in here
        {
          //insert the new and then shuffle the rest back one spot
          gLargestSubGraphs[i] = size;
          memcpy(&gLargestSubGraphs[i+1], &scratch[i], sizeof(tIntsetElem) * (NUM_LARGEST_CIRCUITS - i - 1));
          break;
        }
      }
    }
  }
}

void part1(FILE *f)
{
  load_boxes(f);
  
  const int NUM_CONNECTIONS = gNumBoxes == 1000 ? 1000 : 10;
  find_shortest_edges(NUM_CONNECTIONS);
  
  //Now make the connections one by one, populating an array of sub graphs formed as we go.
  for(int c=0; c<NUM_CONNECTIONS; c++)
  {
    add_connection(&gShortestEdges[c]);

    // printf("\n");
    // for(int i=0; i<gNumSubGraphs; i++)
    //   printf("%u\t", INTSET_size(subGraphs[i]));
  }

  //Finally, assess the resulting subgraphs.
  const int NUM_LARGEST_CIRCUITS = 3;
  find_largest_subgraphs(NUM_LARGEST_CIRCUITS);

  uint64_t result = 1;
  for(int i=0; i<NUM_LARGEST_CIRCUITS; i++)
    result *= gLargestSubGraphs[i];
  
  printf("%llu", result);
}

void part2(FILE *f)
{
  if(!gNumBoxes)
    load_boxes(f);
  
  const int NUM_CONNECTIONS = gNumBoxes == 1000 ? MAX_EDGES : 40;
  find_shortest_edges(NUM_CONNECTIONS);
  
  //Now make the connections one by one, populating an array of sub graphs formed as we go.
  for(int c=0; c<NUM_CONNECTIONS; c++)
  {
    sEdge *pEdge = &gShortestEdges[c];
    add_connection(pEdge);

    printf("\n");
    for(int i=0; i<gNumSubGraphs; i++)
      printf("%u\t", INTSET_size(gSubGraphs[i]));

    if(INTSET_size(gSubGraphs[0]) == (tIntsetElem)gNumBoxes)
    {
      sCoord3D *a = &gBoxes[pEdge->nodeA];
      sCoord3D *b = &gBoxes[pEdge->nodeB];

      // printf("\n[%zu:%u,%u,%u]"  , pEdge->nodeA, a->x, a->y, a->z);
      // printf("\n[%zu:%u,%u,%u]\n", pEdge->nodeB, b->x, b->y, b->z);

      uint64_t result = (uint64_t)a->x * b->x;
      printf("%d, %llu", c, result);

      return;
    }
  }

  printf("No result!");
}
