#define DAY 11
#include "main.h"
#include "common.h"
#include "graph.h"

#define MAX_LINE_LENGTH 200
#define MAX_VERTICES    (26*26*26)

//Perfect hashing function. Assumes each char is [a-z]. Returns [0-17575].
tGraphVertex label_to_vertex(char label[3])
{
  tGraphVertex ret = 0;
  ret += (label[0]-'a')*26*26;
  ret += (label[1]-'a')*26;
  ret += (label[2]-'a');
  return ret;
}
//Should be 25*26*26 + 25*26 + 25 = 26^3 - 1
#define NUM_VERTICES (label_to_vertex("zzz")-label_to_vertex("aaa")+1)

sGraph* parse_input(FILE *f)
{
  char line[MAX_LINE_LENGTH];
  sGraph* g = GRAPH_new(NUM_VERTICES);
  
  while (fgets(line, MAX_LINE_LENGTH, f) != NULL)
  {
    int consumed;
    char src[4], dst[4];
    if(sscanf(line, "%3s:%n", src, &consumed) != 1)
      break;
    tGraphVertex vSrc = label_to_vertex(src);

    char *pLine = &line[consumed];
    while(sscanf(pLine, "%3s%n", dst, &consumed) == 1)
    {
      GRAPH_add_edge(g, vSrc, label_to_vertex(dst));
      pLine += consumed;
    }
  }
  return g;
}



void part1(FILE *f)
{
  sGraph* g = parse_input(f);

  printf("%u", GRAPH_count_paths(g, label_to_vertex("you"), label_to_vertex("out")));
}

void part2(FILE *f)
{
  sGraph* g = parse_input(f);

  tGraphVertex via[] = { label_to_vertex("fft"), label_to_vertex("dac") };
  printf("%llu", GRAPH_count_paths_via(g, label_to_vertex("svr"), label_to_vertex("out"), 2, via));
}
