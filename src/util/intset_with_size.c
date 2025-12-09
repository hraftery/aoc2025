#include "intset.h"

#include <stdlib.h>

typedef uint64_t tData;
typedef struct IntSet {
  size_t  capacity;
  size_t  size;
  tData*  data;
} sIntSet;

#define DATA_SIZE                 (sizeof(tData))
#define DATA_SIZE_BITS            (8*DATA_SIZE)
#define NUM_DATA_ELEMS(capacity)  (((capacity) + DATA_SIZE - 1) / DATA_SIZE)
#define HAS_ELEM(data, e)         (data[(e) / DATA_SIZE_BITS] &  (1 << ((e) % DATA_SIZE_BITS)))
#define SET_ELEM(data, e)         (data[(e) / DATA_SIZE_BITS] |= (1 << ((e) % DATA_SIZE_BITS)))
#define CLEAR_ELEM(data, e)       (data[(e) / DATA_SIZE_BITS] &= ~(1 << ((e) % DATA_SIZE_BITS)))

tIntSet INTSET_new(size_t capacity)
{
  tIntSet is = malloc(sizeof(sIntSet));
  is->capacity = capacity;
  is->size = 0;
  is->data = calloc(NUM_DATA_ELEMS(capacity), DATA_SIZE);
  return is;
}

void INTSET_delete(tIntSet is)
{
  free(is->data);
  free(is);
}

void INTSET_add(tIntSet is, tIntsetElem e)
{
  if(!HAS_ELEM(is->data, e))
  {
    is->size++;
    SET_ELEM(is->data, e);
  }
}

void INTSET_remove(tIntSet is, tIntsetElem e)
{
  if(HAS_ELEM(is->data, e))
  {
    is->size--;
    CLEAR_ELEM(is->data, e);
  }
}

bool INTSET_contains(tIntSet is, tIntsetElem e)
{
  return HAS_ELEM(is->data, e);
}

size_t INTSET_size(tIntSet is)
{
  return is->size;
}

void INTSET_union(tIntSet is, const tIntSet with)
{
  //Could set size to something like size(is) + size(with - is) if we had a set difference
  //operation. Could even set it to size(is) + size(with) if they're disjoint. But for now
  //just reset it and count up manually.
  is->size = 0; 
  for(size_t i=0; i<NUM_DATA_ELEMS(is->capacity); i++)
  {
    is->data[i] |= with->data[i];
    is->size += __builtin_popcountll(is->data[i]);
  }
}
