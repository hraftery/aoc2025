#ifndef DAY
#error "Source file must define DAY before incliding this file."
#endif

#include <stdio.h>
#include <stdlib.h>

void part1(FILE *f);
void part2(FILE *f);

int main(int argc, char* argv[])
{
  int part=0; //0 means both
  char inputVariant = '\0'; //NUL means the actual input
  
  if(argc > 1) //a part has been specified
  {
    part = atoi(argv[1]);

    if(argc > 2) //an alternative input file has been specified
      inputVariant = argv[2][0];
  }

  char inputFilename[] = "input/XXx.txt";
  if(inputVariant)
    sprintf(inputFilename, "input/%02u%c.txt", DAY, inputVariant);
  else
    sprintf(inputFilename, "input/%02u.txt", DAY);
  
  FILE *f = fopen(inputFilename, "r");

  if(part == 0 || part == 1)
  {
    printf("Part1: ");
    part1(f);
    printf("\n");
  }
  rewind(f);
  if(part == 0 || part == 2)
  {
    printf("Part2: ");
    part2(f);
    printf("\n");
  };
  
  fclose(f);
  return 0;
}
