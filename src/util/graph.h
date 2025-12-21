#include <stdlib.h>

typedef uint32_t  tGraphVertex;
typedef uint32_t  tGraphSize;
typedef uint8_t   tGraphWeight;

const extern tGraphVertex GRAPH_INVALID_VERTEX;
#define GRAPH_MAX_VERTICES (UINT32_MAX - 1) /* Not including INVALID_VERTEX */

typedef struct {
  tGraphSize      numVertices;
  tGraphWeight**  adjMatrix;
} sGraph;

sGraph*       GRAPH_new(tGraphSize numVertices);
void          GRAPH_delete(sGraph* g);

void          GRAPH_add_edge(sGraph* g, tGraphVertex src, tGraphVertex dst);
void          GRAPH_add_undirected_edge(sGraph* g, tGraphVertex src, tGraphVertex dst);
void          GRAPH_add_weighted_edge(sGraph* g, tGraphVertex src, tGraphVertex dst, tGraphWeight weight);
void          GRAPH_add_undirected_weighted_edge(sGraph* g, tGraphVertex src, tGraphVertex dst, tGraphWeight weight);

tGraphWeight GRAPH_get_edge_weight(sGraph* g, tGraphVertex src, tGraphVertex dst);
//Get the dst numerically after lastDst. Use GRAPH_INVALID_VERTEX to
//get the first. Returns GRAPH_INVALID_VERTEX if there are no more.
tGraphVertex  GRAPH_get_next_destination(sGraph* g, tGraphVertex src, tGraphVertex lastDst);
uint32_t      GRAPH_count_paths(sGraph* g, tGraphVertex src, tGraphVertex dst);
uint64_t      GRAPH_count_paths_via(sGraph* g, tGraphVertex src, tGraphVertex dst, tGraphSize numVia, tGraphVertex via[numVia]);

void          GRAPH_print_adjacency_matrix(sGraph* g);
