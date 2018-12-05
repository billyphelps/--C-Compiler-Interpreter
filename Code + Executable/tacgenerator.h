#ifndef _tacgen_
#define _tacgen_

#include "nodes.h"
#include "TAC.h"

TOKEN* generateTAC(NODE* ast);
TOKEN* createTAC(NODE* ast);
TOKEN* getNewTemporary();
TOKEN* getNewLabel();
TACLINE** argsToArgs(TACLINE** tac_array);

#endif