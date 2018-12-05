#include <stdio.h>
#include "environment.h"
#include "interpreter.h"

FRAME* rootFRAME;
FRAME* currentFRAME;

bool returning = false;


bool declaring_variable = false;

NODE* root_node;
NODE* current_func_node;

int getLeafValue(NODE* ast){
	return ((TOKEN*)(ast->left))->value;
}

NODE* constantToLeaf(int constant){
	TOKEN* t = new_token(CONSTANT);
	t->value = constant;
	return make_leaf(t);
}


NODE* lessThan(NODE* node1, NODE* node2){
	return constantToLeaf(getLeafValue(interpret(node1)) < getLeafValue(interpret(node2)));
}

NODE* greaterThan(NODE* node1, NODE* node2){
	return constantToLeaf(getLeafValue(interpret(node1)) > getLeafValue(interpret(node2)));
}

NODE* lessThanEqualTo(NODE* node1, NODE* node2){
	return constantToLeaf(getLeafValue(interpret(node1)) <= getLeafValue(interpret(node2)));
}

NODE* greaterThanEqualTo(NODE* node1, NODE* node2){
	return constantToLeaf(getLeafValue(interpret(node1)) >= getLeafValue(interpret(node2)));
}

NODE* equalTo(NODE* node1, NODE* node2){
	return constantToLeaf(getLeafValue(interpret(node1)) == getLeafValue(interpret(node2)));
}

NODE* notEqualTo(NODE* node1, NODE* node2){
	return constantToLeaf(getLeafValue(interpret(node1)) != getLeafValue(interpret(node2)));
}

NODE* addNodes(NODE* node1, NODE* node2){
	return constantToLeaf(getLeafValue(interpret(node1)) + getLeafValue(interpret(node2)));
}

NODE* subtractNodes(NODE* node1, NODE* node2){
	return constantToLeaf(getLeafValue(interpret(node1)) - getLeafValue(interpret(node2)));
}

NODE* multiplyNodes(NODE* node1, NODE* node2){
	return constantToLeaf(getLeafValue(interpret(node1)) * getLeafValue(interpret(node2)));
}

NODE* divideNodes(NODE* node1, NODE* node2){
	return constantToLeaf(getLeafValue(interpret(node1)) / getLeafValue(interpret(node2)));
}

NODE* interpret(NODE* ast) {
	switch (ast->type) {
		default:
			return NULL;
			break;

		case '~':{

			NODE* left_result = interpret(ast->left);

			if(returning){
				return left_result;
			}

			return interpret(ast->right);
		}

		case 'D':{
			//left child: function definition
			TOKEN* t = (TOKEN*)(ast->left->right->left->left);

			CLOSURE* clos = createClosure(createFunction(ast,ast->left->left->type,NULL),NULL);
			addNewFuncBinding(currentFRAME,t, clos);
			//right child: function code
			if(!strcmp(t->lexeme,"main")){

				FRAME* oldFrame = currentFRAME;

				currentFRAME = createFrame(rootFRAME);

				NODE* i = interpret(ast->right);

				currentFRAME = oldFrame;

				return i;
			} else {
				return NULL;
			}
			break;
		}

		case 'd':
			//left child: function return type
			//right child: F
			break;

		case 'F':
			//left child: function name
			//right child: function arguments
			break;

		case APPLY:{

			//left child: function name
			NODE* lnode = interpret(ast->left);
			TOKEN* t = (TOKEN*)lnode;

			Function* the_func;
			FRAME* oldFrame = currentFRAME;

			CLOSURE* closure;

			if(ast->left->type == APPLY){
				if(lnode->closure != NULL){
					closure = lnode->closure;
				}
			} else {
				closure = getBindingFunction(currentFRAME,t);

			}

			if(closure->frame != NULL){
				currentFRAME = closure->frame;
			} else {

				currentFRAME = createFrame(currentFRAME);
				
			}
			the_func = closure->function;


			NODE* functionD = the_func->DNode;


			NODE* apply_values_root = ast->right;
			NODE* function_variables_root = functionD->left->right->right;


			bindFunctionArguments(currentFRAME,oldFrame,function_variables_root,apply_values_root);

			NODE* i = interpret(functionD->right);

			

			TOKEN* temp = (TOKEN*)i;
			int type = getBindingTypeFromToken(currentFRAME->initial_binding,temp);
			if(type == FUNCTION){
				Function* func = getBindingFunction(currentFRAME,temp)->function;
				i->closure = createClosure(func,currentFRAME);
				the_func->frame = currentFRAME;
			}

			returning = false;

			currentFRAME = oldFrame;

			return i;
		}

		case WHILE:{
			NODE* left = interpret(ast->left);
			int boolean = getLeafValue(left);

			NODE* right = NULL;

			while(boolean){
				right = interpret(ast->right);

				left = interpret(ast->left);
				boolean = getLeafValue(left);
			} 

			return right;
		}

		case IF:{
			//left node: conditional
			int boolean = getLeafValue(interpret(ast->left));

			if(ast->right->type == ELSE){
				if(boolean){
					return interpret(ast->right->left);
				} else {
					return interpret(ast->right->right);
				}
			} else {
				if(boolean){
					return interpret(ast->right);
				} else {
					return NULL;
				}
			}
			//right node: code to perform if left is true OR else node

			break;
		}

		case ELSE:{
			//left node: code to do if true
			//right node: code to do if false

			break;
		}

		case '<':{
			return lessThan(ast->left,ast->right);
		}

		case '>':{
			return greaterThan(ast->left,ast->right);
		}

		case LE_OP:{
			return lessThanEqualTo(ast->left,ast->right);
		}

		case GE_OP:{
			return greaterThanEqualTo(ast->left,ast->right);
		}

		case EQ_OP:{
			return equalTo(ast->left,ast->right);
		}

		case NE_OP:{
			return notEqualTo(ast->left,ast->right);
		}

		case ';':{
			NODE* left = interpret(ast->left);

			if(returning){
				return left;
			}

			return interpret(ast->right);
		}

		// case INT:
		// 	printf("int\n");
		// 	declaring_variable = true;
		// 	return ast;

		case '+':
			return addNodes(ast->left,ast->right);

		case '-':
			return subtractNodes(ast->left,ast->right);

		case '/':
			return divideNodes(ast->left,ast->right);

		case '*':
			return multiplyNodes(ast->left,ast->right);

		case LEAF:{	

			TOKEN* token = (TOKEN *)(ast->left);

			switch(token->type){

				default:	
	    			// printf("is not constant or int\n");

				case CONSTANT:
	    			return ast;

	    		case INT:
					declaring_variable = true;
					return ast;

				case FUNCTION:
					declaring_variable = true;
					return ast;

				case IDENTIFIER:{

					CLOSURE* c = getBindingFunction(currentFRAME,token);
					if(c != NULL && c->function != NULL){
						return ast->left;
					}

					int val = getBindingValue(currentFRAME,token);

					return constantToLeaf(val); 
				}

			}
		}

		case RETURN:{
			NODE* n = interpret(ast->left);

			returning = true;
			return n;
		}

		case '=':{

			TOKEN* t = ((TOKEN*)((ast->left)->left));

			if(declaring_variable){
				NODE* i = interpret(ast->right);

				TOKEN* temp = (TOKEN*)(i->left);

				int type = i->type;

				if(i->closure != NULL){

					CLOSURE* clos = i->closure;

					addNewFuncBinding(currentFRAME, t, clos);

				} else {

					addNewIntBinding(currentFRAME, t, getLeafValue(i));
					declaring_variable = false;
				}

			} else {
				int i = getLeafValue(interpret(ast->right));
				changeIntBindingValue(currentFRAME, t, i);
			}
			return NULL;

		}
	}
}

NODE* interpretWithFrame(NODE* ast, FRAME* frame){
	FRAME* temp = currentFRAME;
	currentFRAME = frame;
	NODE* n = interpret(ast);
	currentFRAME = temp;
	return n;
}

NODE* callInterpreter(FRAME* root_frame, FRAME* current_frame, NODE* tree){
	root_node = tree;
	rootFRAME = root_frame;
	currentFRAME = current_frame;
	return interpret(tree);
}
