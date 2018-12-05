#ifndef _sets_
#define _sets_

#include "symbol_table.h"
#include "token.h"
#include "C.tab.h"

typedef struct Set{
	TOKEN** elements;
	int number_of_elements;
} Set;


Set* createSet(TOKEN** elements);

Set* emptySet();

TOKEN* addToSet(Set* set, TOKEN* element);

int setContains(Set* set, TOKEN* element);

Set* removeElementFromSet(Set* set, TOKEN* element);

Set* removeSetFromSet(Set* set, Set* removalSet);

Set* unionOfSets(Set* set1, Set* set2);

void freeSet(Set* set);

int setsAreEqual(Set* set1, Set* set2);

void duplicateSet(Set* destination, Set* set);

void printSet(Set* set);

#endif