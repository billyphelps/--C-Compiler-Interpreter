#ifndef _gcoloring_
#define _gcoloring_

#include "token.h"
#include "C.tab.h"
#include "Set.h"

typedef struct GraphEdge{
	TOKEN* node1;
	TOKEN* node2;
} GraphEdge;

typedef struct Graph{
	GraphEdge** graph_edges;
	int number_of_edges;
} Graph;

typedef struct NodeStackElement{
	TOKEN* removed_node;
	TOKEN** connected_nodes;
} NodeStackElement;

typedef struct NodeStack{
	NodeStackElement** node_stack_elements;
	int size;
} NodeStack;

typedef struct NodeColor{
	TOKEN* node;
	int color;
} NodeColor;

typedef struct NodeColorStack{
	NodeColor** elements;
	int size;
} NodeColorStack;

GraphEdge* createGraphEdge(TOKEN* node1, TOKEN* node2);
Graph* createGraph(GraphEdge** graph_edges);
NodeStackElement* createNodeStackElement(TOKEN* removed_node, TOKEN** connected_nodes);
NodeStack* createNodeStack(NodeStackElement** node_stack_elements, int size);
NodeColorStack* createNodeColorStack(NodeColor** elements, int size);
int edgeContainsNode(GraphEdge* edge, TOKEN* node);
NodeStackElement* removeNodeFromGraph(Graph* graph, TOKEN* node);
int numberOfEdges(Graph* graph, TOKEN* node);
void printGraphEdges(Graph* graph);
void addGraphEdge(GraphEdge** edges, GraphEdge* edge);
TOKEN** getGraphNodes(Graph* graph);
NodeStack* graphToStack(Graph* graph, TOKEN** nodes, int noOfColors);
NodeColorStack* colorsFromStack(NodeStack* stack, int noOfColors);
void printNodeColors(NodeColor** colors, int totalNodes);


#endif