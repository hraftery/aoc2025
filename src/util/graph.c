#include "graph.h"

#include <stdio.h>

const tGraphVertex  GRAPH_INVALID_VERTEX = UINT32_MAX;

sGraph* GRAPH_new(tGraphSize numVertices)
{
  sGraph* g = malloc(sizeof(sGraph));
  g->numVertices = numVertices;
  g->adjMatrix = malloc(numVertices * sizeof(tGraphSize*));
  for(tGraphSize i=0; i<numVertices; i++)
    g->adjMatrix[i] = calloc(numVertices, sizeof(tGraphSize));
  
  return g;
}

void GRAPH_add_edge(sGraph* g, tGraphVertex src, tGraphVertex dst)
{
  GRAPH_add_weighted_edge(g, src, dst, 1);
}

void GRAPH_add_undirected_edge(sGraph* g, tGraphVertex src, tGraphVertex dst)
{
  GRAPH_add_weighted_edge(g, src, dst, 1);
  GRAPH_add_weighted_edge(g, dst, src, 1);
}

void GRAPH_add_weighted_edge(sGraph* g, tGraphVertex src, tGraphVertex dst, tGraphWeight weight)
{
  if (src >= g->numVertices || dst >= g->numVertices)
  {
    printf("Invalid vertices!\n");
    return;
  }
  g->adjMatrix[src][dst] = weight;
}

void GRAPH_add_undirected_weighted_edge(sGraph* g, tGraphVertex src, tGraphVertex dst, tGraphWeight weight)
{
  GRAPH_add_weighted_edge(g, src, dst, weight);
  GRAPH_add_weighted_edge(g, dst, src, weight);
}

tGraphWeight GRAPH_get_edge_weight(sGraph* g, tGraphVertex src, tGraphVertex dst)
{
  return g->adjMatrix[src][dst];
}

tGraphVertex  GRAPH_get_next_destination(sGraph* g, tGraphVertex src, tGraphVertex lastDst)
{
  for(tGraphSize dst = (lastDst == GRAPH_INVALID_VERTEX ? 0 : lastDst+1); dst<g->numVertices; dst++)
  {
    if(g->adjMatrix[src][dst] != 0)
      return dst;
  }
  return GRAPH_INVALID_VERTEX;
}


typedef struct {
  sGraph* g;
  uint32_t *pathCountCache;
  const uint32_t COUNT_UNINITIALISED;
} sCountPathsContext;

uint32_t do_count_paths(const sCountPathsContext* ctx, tGraphVertex src, tGraphVertex dst)
{
  //printf("%c%c%c\n", 'a' + (src / (26*26)), 'a' + ((src % (26*26))/26), 'a' + (src % 26));
  
  if(ctx->pathCountCache[src] != ctx->COUNT_UNINITIALISED)
    return ctx->pathCountCache[src];
  
  uint32_t numPaths = 0;
  tGraphVertex thisDst, lastDst = GRAPH_INVALID_VERTEX;
  while((thisDst = GRAPH_get_next_destination(ctx->g, src, lastDst)) != GRAPH_INVALID_VERTEX)
  {
    if(thisDst == dst)
      numPaths++;
    else
      numPaths += do_count_paths(ctx, thisDst, dst);
    lastDst = thisDst;
  }
  ctx->pathCountCache[src] = numPaths;
  return numPaths;
}

uint32_t GRAPH_count_paths(sGraph* g, tGraphVertex src, tGraphVertex dst)
{
  //Index by src. dst is always dst.
  uint32_t *pathCountCache = malloc(sizeof(uint32_t) * g->numVertices);

  const uint32_t COUNT_UNINITIALISED = UINT32_MAX;
  for(tGraphVertex v=0; v<g->numVertices; v++)
    pathCountCache[v] = COUNT_UNINITIALISED;
  
  sCountPathsContext ctx = {
    .g = g,
    .pathCountCache = pathCountCache,
    .COUNT_UNINITIALISED = COUNT_UNINITIALISED
  };

  uint32_t count = do_count_paths(&ctx, src, dst);
  free(pathCountCache);
  return count;
}

typedef struct {
  uint64_t count;
  uint8_t  viaBitFieldSoFar;
} sPathCountCache;

typedef struct {
  sGraph* g;
  sPathCountCache *pathCountCache;
  const uint64_t COUNT_UNINITIALISED;
  tGraphVertex  *viaVertices;
  tGraphSize    numViaVertices;

} sCountPathsViaContext;

uint64_t do_count_paths_via(const sCountPathsViaContext* ctx, tGraphVertex src, tGraphVertex dst, uint8_t viaBitField)
{
  //printf("%c%c%c\n", 'a' + (src / (26*26)), 'a' + ((src % (26*26))/26), 'a' + (src % 26));
    
  for(tGraphSize i=0; i<ctx->numViaVertices; i++)
  {
    if(src == ctx->viaVertices[i])
    {
      viaBitField |= (1<<i);
      break;
    }
  }

  if(ctx->pathCountCache[src].count != ctx->COUNT_UNINITIALISED &&
     ctx->pathCountCache[src].viaBitFieldSoFar == viaBitField)
    return ctx->pathCountCache[src].count;

  uint64_t numPaths = 0;
  tGraphVertex thisDst, lastDst = GRAPH_INVALID_VERTEX;
  while((thisDst = GRAPH_get_next_destination(ctx->g, src, lastDst)) != GRAPH_INVALID_VERTEX)
  {
    if(thisDst == dst)
      numPaths += (viaBitField == ((1 << ctx->numViaVertices) - 1) ? 1 : 0);
    else
      numPaths += do_count_paths_via(ctx, thisDst, dst, viaBitField);
    lastDst = thisDst;
  }
  ctx->pathCountCache[src].viaBitFieldSoFar = viaBitField;
  ctx->pathCountCache[src].count = numPaths;
  return numPaths;
}

uint64_t GRAPH_count_paths_via(sGraph* g, tGraphVertex src, tGraphVertex dst, tGraphSize numVia, tGraphVertex via[numVia])
{
  sPathCountCache *pathCountCache = malloc(sizeof(sPathCountCache) * g->numVertices);

  const uint64_t COUNT_UNINITIALISED = UINT32_MAX;
  for(tGraphVertex v=0; v<g->numVertices; v++)
    pathCountCache[v].count = COUNT_UNINITIALISED;
  
  sCountPathsViaContext ctx = {
    .g = g,
    .pathCountCache = pathCountCache,
    .COUNT_UNINITIALISED = COUNT_UNINITIALISED,
    .numViaVertices = numVia,
    .viaVertices = via
  };

  uint64_t count = do_count_paths_via(&ctx, src, dst, 0);
  free(pathCountCache);
  return count;
}

void GRAPH_print_adjacency_matrix(sGraph* g)
{
  for(tGraphSize i=0; i<g->numVertices; i++)
  {
    for(tGraphSize j=0; j<g->numVertices; j++)
      printf("%u ", g->adjMatrix[i][j]);
    printf("\n");
  }
}

void GRAPH_delete(sGraph* g)
{
  for(tGraphSize i=0; i<g->numVertices; i++)
    free(g->adjMatrix[i]);
  free(g->adjMatrix);
  free(g);
}
