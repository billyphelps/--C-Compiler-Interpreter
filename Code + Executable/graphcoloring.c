#include "graphcoloring.h"

GraphEdge* createGraphEdge(TOKEN* node1, TOKEN* node2){
	GraphEdge* new_graph_edge = (GraphEdge*)malloc(sizeof(GraphEdge));
	if(new_graph_edge == NULL)
    {
        printf("Error creating a new GraphEdge.\n");
        exit(0);
    }
    new_graph_edge->node1 = node1;
    new_graph_edge->node2 = node2;

	return new_graph_edge;
}

Graph* createGraph(GraphEdge** graph_edges){
	Graph* new_graph = (Graph*)malloc(sizeof(Graph));
	if(new_graph == NULL)
    {
        printf("Error creating a new Graph.\n");
        exit(0);
    }

    int size = 0;
    while(graph_edges[size] != NULL){
    	size++;
    }

    new_graph->graph_edges = graph_edges;
    new_graph->number_of_edges = size;

	return new_graph;
}

NodeStackElement* createNodeStackElement(TOKEN* removed_node, TOKEN** connected_nodes){
	NodeStackElement* new_node_stack_element = (NodeStackElement*)malloc(sizeof(NodeStackElement));
	if(new_node_stack_element == NULL)
    {
        printf("Error creating a new NodeStackElement.\n");
        exit(0);
    }

    new_node_stack_element->removed_node = removed_node;
    new_node_stack_element->connected_nodes = connected_nodes;

	return new_node_stack_element;
}

NodeStack* createNodeStack(NodeStackElement** node_stack_elements, int size){
	NodeStack* new_node_stack = (NodeStack*)malloc(sizeof(NodeStack));
	if(new_node_stack == NULL)
    {
        printf("Error creating a new NodeStack.\n");
        exit(0);
    }

    new_node_stack->node_stack_elements = node_stack_elements;
    new_node_stack->size = size;

	return new_node_stack;
}

NodeColor* createNodeColor(TOKEN* node, int color){
	NodeColor* new_node_color = (NodeColor*)malloc(sizeof(NodeColor));
	if(new_node_color == NULL)
    {
        printf("Error creating a new NodeColor.\n");
        exit(0);
    }

    new_node_color->node = node;
    new_node_color->color = color;

	return new_node_color;
}

NodeColorStack* createNodeColorStack(NodeColor** elements, int size){
	NodeColorStack* new_node_color_stack = (NodeColorStack*)malloc(sizeof(NodeColorStack));
	if(new_node_color_stack == NULL)
    {
        printf("Error creating a new NodeColorStack.\n");
        exit(0);
    }

    new_node_color_stack->elements = elements;
    new_node_color_stack->size = size;

	return new_node_color_stack;
}

int edgeContainsNode(GraphEdge* edge, TOKEN* node){
	if(edge->node1 == node || edge->node2 == node){
		return 1;
	}
	return 0;
}


//Returns a nodestack with all all nodes it was connected to
NodeStackElement* removeNodeFromGraph(Graph* graph, TOKEN* node){

	int noOfEdges = graph->number_of_edges;

	TOKEN** node_array = (TOKEN**)malloc(sizeof(TOKEN*) * noOfEdges);
	int tokenPos = 0;

	for (int i = 0; i<noOfEdges ; ++i)
	{
		if(graph->graph_edges[i] != NULL && edgeContainsNode(graph->graph_edges[i],node)){
			if(graph->graph_edges[i]->node1 == node){
				node_array[tokenPos++] = graph->graph_edges[i]->node2;
			} else {
				node_array[tokenPos++] = graph->graph_edges[i]->node1;
			}
			graph->graph_edges[i] = NULL;
		}
	}

	TOKEN** temp = realloc(node_array,(tokenPos+1)*sizeof(*node_array));
	node_array = temp;

	return createNodeStackElement(node,node_array);

}

int numberOfEdges(Graph* graph, TOKEN* node){
	int number = 0;
	for (int i = 0; i < graph->number_of_edges; ++i)
	{
		if(graph->graph_edges[i] != NULL && edgeContainsNode(graph->graph_edges[i],node)){
			number++;
		}
	}
	return number;
}

int getNodeColor(NodeColorStack* allocated_colors_stack, TOKEN* node){
	for (int i = 0; i < allocated_colors_stack->size; ++i)
	{
		if(allocated_colors_stack->elements[i] != NULL && allocated_colors_stack->elements[i]->node != NULL && allocated_colors_stack->elements[i]->node == node){
			return allocated_colors_stack->elements[i]->color;
		}
	}
	printf("no assigned color, error\n");
	return -1;
}


NodeColor* calculateNodeColor(NodeColorStack* node_colors_stack, NodeStackElement* element, int noOfColors){

	TOKEN* current_node = element->removed_node;
	TOKEN** connected_nodes = element->connected_nodes;
	int connected_node_num = 0;
	while(connected_nodes[connected_node_num] != NULL){
		connected_node_num++;
	}

	//iterates through all available colors
	for (int color = 0; ; ++color)
	{
		int color_in_use = 0;
		//iterates through all nodes connected to current node
		for (int connection = 0; connection < connected_node_num; ++connection)
		{
			if(getNodeColor(node_colors_stack,connected_nodes[connection]) == color){
				color_in_use = 1;
				break;
			}

		}
		if(!color_in_use){
			return createNodeColor(current_node,color);
		}
	}

}



NodeColorStack* colorsFromStack(NodeStack* stack, int noOfColors){
	int spilled = 0;

	int stack_size = stack->size;

	NodeColor** node_colors = (NodeColor**)malloc(stack_size * sizeof(NodeColor*));
	for (int i = 0; i < stack_size; ++i)
	{
		node_colors[i] = NULL;
	}
	NodeColorStack* node_colors_stack = createNodeColorStack(node_colors,stack_size);

	int new_color = 0;

	node_colors_stack->elements[stack_size-1] = createNodeColor(stack->node_stack_elements[stack_size-1]->removed_node,new_color++);

	//iterates through each node on the stack
	for (int i = stack_size-2; i > -1; --i)
	{
		node_colors_stack->elements[i] = calculateNodeColor(node_colors_stack,stack->node_stack_elements[i],noOfColors);
		if(node_colors_stack->elements[i]->color > noOfColors){
			spilled = 1;
		}
	}

	// if(spilled){
	// 	for (int i = stack_size-2; i > -1; --i)
	// 	{
	// 		if
	// 	}
	// }

	return node_colors_stack;
}

void printNodeColors(NodeColor** colors, int totalNodes){

	for (int i = 0; i < totalNodes; ++i)
	{
		if(colors[i] != NULL){
			printf("\t%s is of color %i\n", colors[i]->node->lexeme, colors[i]->color);
		} else {
			printf("NULL\n");
		}
	}
}

NodeStack* graphToStack(Graph* graph, TOKEN** nodes, int noOfColors){
	int totalNodes = 0;
	while(nodes[totalNodes] != NULL){
		totalNodes++;
	}

	int nodesRemoved = 0;
	NodeStackElement** node_stack = (NodeStackElement**)malloc(totalNodes*sizeof(NodeStackElement*));
	int nodeRemovedInIteration = 0;
	int removeAny = 0;

	while(nodesRemoved<totalNodes){
		nodeRemovedInIteration = 0;
		for (int i = 0; i < totalNodes; ++i)
		{
			if(nodes[i]!=NULL && (numberOfEdges(graph,nodes[i])<noOfColors || removeAny)){
				node_stack[nodesRemoved++] = removeNodeFromGraph(graph,nodes[i]);
				nodes[i] = NULL;
				nodeRemovedInIteration = 1;
				removeAny = 0;
			}
		}
		if(!nodeRemovedInIteration){
			removeAny = 1;
		}
	}

	return createNodeStack(node_stack,nodesRemoved);	
}

void addGraphEdge(GraphEdge** edges, GraphEdge* edge){
	int i = 0;

	for (; edges[i] != NULL; ++i)
	{
		if((edges[i]->node1 == edge->node1 || edges[i]->node1 == edge->node2) && (edges[i]->node2 == edge->node1 || edges[i]->node2 == edge->node2)){
			break;
		}
	}

	if(edges[i] == NULL){
		edges[i] = edge;
	}
}

TOKEN** getGraphNodes(Graph* graph){
	
	int i = 0;

	Set* nodes = emptySet();

	while(i<graph->number_of_edges){
		addToSet(nodes,graph->graph_edges[i]->node1);
		addToSet(nodes,graph->graph_edges[i]->node2);
		i++;
	}

	return nodes->elements;
}

void printGraphEdges(Graph* graph){
	int i = 0;

	while(i<graph->number_of_edges){
		if(graph->graph_edges[i]!= NULL){
			printf("Edge: %s %s\n", graph->graph_edges[i]->node1->lexeme, graph->graph_edges[i]->node2->lexeme);
		}
		i++;
	}
}

