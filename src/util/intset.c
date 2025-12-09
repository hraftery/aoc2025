#include "intset.h"

#include <stdlib.h>

typedef uint64_t tData;
typedef struct IntSet {
  tIntsetElem capacity; //capacity need not exceed maximum tIntsetElem
  tData*      data;
} sIntSet;

#define DATA_SIZE                 (sizeof(tData))
#define DATA_SIZE_BITS            (8*DATA_SIZE)
#define NUM_DATA_ELEMS(capacity)  (((capacity) + DATA_SIZE_BITS - 1) / DATA_SIZE_BITS)
#define HAS_ELEM(data, e)         ((data[(e) / DATA_SIZE_BITS] &  (1ull << ((e) % DATA_SIZE_BITS))) != 0ull)
#define SET_ELEM(data, e)         ( data[(e) / DATA_SIZE_BITS] |= (1ull << ((e) % DATA_SIZE_BITS)))
#define CLEAR_ELEM(data, e)       ( data[(e) / DATA_SIZE_BITS] &= ~(1ull << ((e) % DATA_SIZE_BITS)))

tIntSet INTSET_new(tIntsetElem capacity)
{
  tIntSet is = malloc(sizeof(sIntSet));
  is->capacity = capacity;
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
  SET_ELEM(is->data, e);
}

void INTSET_remove(tIntSet is, tIntsetElem e)
{
  CLEAR_ELEM(is->data, e);
}

bool INTSET_contains(tIntSet is, tIntsetElem e)
{
  return HAS_ELEM(is->data, e);
}

tIntsetElem INTSET_size(tIntSet is)
{
  tIntsetElem size = 0;
  for(tIntsetElem i=0; i<NUM_DATA_ELEMS(is->capacity); i++)
    size += __builtin_popcountll(is->data[i]);

  return size;
}

void INTSET_union(tIntSet is, const tIntSet with)
{
  for(tIntsetElem i=0; i<NUM_DATA_ELEMS(is->capacity); i++)
    is->data[i] |= with->data[i];
}
