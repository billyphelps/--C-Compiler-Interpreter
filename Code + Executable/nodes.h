#ifndef _node_
#define _node_

#include "token.h"

typedef struct node {
  int          type;
  struct node *left;
  struct node *right;
  struct CLOSURE* closure;
} NODE;

NODE* make_leaf(TOKEN*);
NODE* make_node(int, NODE*, NODE*);

#endif