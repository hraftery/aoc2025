#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

//Since a IntSet can hold at most all the possible elements, tIntselElem
//doubles as the element type and the size/capacity type.
typedef uint32_t tIntsetElem;
typedef struct IntSet *tIntSet;

tIntSet     INTSET_new(tIntsetElem capacity);
void        INTSET_delete(tIntSet is);
void        INTSET_add(tIntSet is, tIntsetElem e);
bool        INTSET_contains(tIntSet is, tIntsetElem e);
tIntsetElem INTSET_size(tIntSet is);
void        INTSET_union(tIntSet is, const tIntSet with);
