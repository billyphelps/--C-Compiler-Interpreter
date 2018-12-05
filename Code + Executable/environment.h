#ifndef _environment_
#define _environment_

#include <stdio.h>
#include <stdlib.h>
#include "C.tab.h"
#include "nodes.h"

typedef int bool;
#define true 1
#define false 0


enum varType {
	VAR_FUN = 0,
    VAR_INT = 1
};

typedef struct Function
{
	NODE* DNode;
	int ReturnType;
	struct FRAME* frame;
} Function;

typedef struct BINDING {
	TOKEN* token;
	int VarType;

	int value;
	struct CLOSURE* closure;

	struct BINDING* next_binding;
} BINDING;

typedef struct FRAME {
	BINDING* initial_binding;
	int size;
	char* id;
	struct FRAME* parent_frame;
} FRAME;

typedef struct CLOSURE{
	Function* function;
	FRAME* frame;
} CLOSURE;


#include "interpreter.h"


FRAME* createFrame(FRAME* parent_frame);
Function* createFunction(NODE* DNode, int ReturnType,FRAME* frame);
CLOSURE* createClosure(Function* function, FRAME* frame);
BINDING* createBinding(TOKEN* token,int VarType, int value, CLOSURE* closure, BINDING* next_binding);
BINDING* createIntBinding(TOKEN* token, int value, BINDING* next_binding);
BINDING* createFuncBinding(TOKEN* token, CLOSURE* closure, BINDING* next_binding);
BINDING* getBindingFromToken(BINDING* binding, TOKEN* token);
int getBindingTypeFromToken(BINDING* binding, TOKEN* token);
void changeIntBindingValue(FRAME* frame, TOKEN* token, int value);
bool checkBindingForToken(BINDING* binding, TOKEN* token);
bool checkFrameForToken (FRAME* frame, TOKEN* token);
BINDING* getEndOfFrame(FRAME* frame);
void addNewIntBinding(FRAME* frame, TOKEN* token, int value);
void addNewFuncBinding(FRAME* frame, TOKEN* token, CLOSURE* closure);
int getBindingValue(FRAME* frame, TOKEN* token);
CLOSURE* getBindingFunction(FRAME* frame, TOKEN* token);
void bindFunctionArguments(FRAME* frame,FRAME* var_frame, NODE* variables, NODE* values);
void printAllBindings(BINDING* binding);

#endif