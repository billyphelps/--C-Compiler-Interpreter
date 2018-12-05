#include "environment.h"

int frame_no = 0;

FRAME* createFrame(FRAME* parent_frame){
    FRAME* new_frame = (FRAME*)malloc(sizeof(FRAME));
    if(new_frame == NULL)
    {
        printf("Error creating a new FRAME.\n");
        exit(0);
    }

    char* id;
    asprintf(&id,"Frame_%i",frame_no++);

    new_frame->size = 0;
    new_frame->parent_frame = parent_frame;
    new_frame->id = id;
 
    return new_frame;
}

Function* createFunction(NODE* DNode, int ReturnType,FRAME* frame){
	Function* new_function = (Function*)malloc(sizeof(Function));
	if(new_function == NULL)
    {
        printf("Error creating a new Function.\n");
        exit(0);
    }

    new_function->DNode = DNode;
    new_function->ReturnType = ReturnType;
    new_function->frame = frame;

    return new_function;
}

CLOSURE* createClosure(Function* function, FRAME* frame){
	CLOSURE* new_closure = (CLOSURE*)malloc(sizeof(CLOSURE));
	if(new_closure == NULL)
    {
        printf("Error creating a new CLOSURE.\n");
        exit(0);
    }

    new_closure->function = function;
    new_closure->frame = frame;

    return new_closure;
}

BINDING* createBinding(TOKEN* token,int VarType, int value, CLOSURE* closure, BINDING* next_binding){
	BINDING* new_binding = (BINDING*)malloc(sizeof(BINDING));
	if(new_binding == NULL)
    {
        printf("Error creating a new BINDING.\n");
        exit(0);
    }

    new_binding->token = token;
    new_binding->VarType = VarType;
    new_binding->value = value;
    new_binding->closure = closure;
    new_binding->next_binding = next_binding;

    return new_binding;
}

BINDING* createIntBinding(TOKEN* token, int value, BINDING* next_binding){
	BINDING* new_binding = (BINDING*)malloc(sizeof(BINDING));
	if(new_binding == NULL)
    {
        printf("Error creating a new INTBINDING.\n");
        exit(0);
    }

    new_binding->token = token;
    new_binding->VarType = INT;
    new_binding->value = value;
    new_binding->closure = NULL;
    new_binding->next_binding = next_binding;

    return new_binding;
}

BINDING* createFuncBinding(TOKEN* token, CLOSURE* closure, BINDING* next_binding){
	BINDING* new_binding = (BINDING*)malloc(sizeof(BINDING));
	if(new_binding == NULL)
    {
        printf("Error creating a new FUNCBINDING.\n");
        exit(0);
    }

    new_binding->token = token;
    new_binding->VarType = FUNCTION;
    new_binding->closure = closure;
    new_binding->next_binding = next_binding;

    return new_binding;
}


//gets the binding from a frame with the given token
BINDING* getBindingFromToken(BINDING* binding, TOKEN* token){
	if(binding == NULL){
		return NULL;
	}
	if ((binding->token) == token){
		return binding;
	} else {
		if(binding->next_binding == NULL){
			return NULL;
		} else {
			return getBindingFromToken((binding->next_binding),token);
		}
	}
}

//gets the bindingtype from a frame with the given token
int getBindingTypeFromToken(BINDING* binding, TOKEN* token){
	if(binding == NULL){
		return -1;
	}
	if ((binding->token) == token){
		return binding->VarType;
	} else {
		if(binding->next_binding == NULL){
			return -1;
		} else {
			return getBindingTypeFromToken((binding->next_binding),token);
		}
	}
}

//changes a binding value, not for new bindings
void changeIntBindingValue(FRAME* frame, TOKEN* token, int value){
	BINDING* binding = getBindingFromToken(frame->initial_binding,token);

	if(binding == NULL){

		if(frame->parent_frame !=NULL){
			changeIntBindingValue(frame->parent_frame, token, value);
		} else {
			printf("Variable %s does not exist in frame\n", token->lexeme);
			exit(0);
		}

	} else {
		binding->value = value;
	}
}

//checks if there is a binding for the supplied token
bool checkBindingForToken(BINDING* binding, TOKEN* token){

	if ((binding->token) == token){
		return true;
	} else {
		if(binding->next_binding == NULL){
			return false;
		} else {
			return checkBindingForToken(binding->next_binding,token);
		}
	}
}

//returns true if lexeme already exists in the given frame
bool checkFrameForToken (FRAME* frame, TOKEN* token){
	if(frame->size == 0){
		return false;
	}
	return checkBindingForToken(frame->initial_binding, token);
}

BINDING* getEndOfFrame0(BINDING* binding){
	if(binding->next_binding == NULL){
		return binding;
	} else {
		return getEndOfFrame0(binding->next_binding);
	}
}

//returns the pointer to the last binding in a frame
BINDING* getEndOfFrame(FRAME* frame){

	return (getEndOfFrame0(frame->initial_binding));
}

// BINDING* getEndOfFrame0(BINDING* binding,int frame_size, int i){
// 	if(i<frame_size){
// 		return getEndOfFrame0(binding->next_binding,frame_size,i++);
// 	} else {
// 		return binding;
// 	}
// }

// //returns the pointer to the last binding in a frame
// BINDING* getEndOfFrame(FRAME* frame){
// 	return (getEndOfFrame0(frame->initial_binding,frame->size,0));
// }


//adds a new binding to a frame, not for reassigning tokens
void addNewIntBinding(FRAME* frame, TOKEN* token, int value){
	if(checkFrameForToken(frame,token)){
		printf("Declaration Error. Variable %s already exists in the frame.\n", token->lexeme);
		exit(0);
	} else {
		if(frame->size == 0){
			frame->initial_binding = createIntBinding(token,value,NULL);
			frame->size ++;
		} else {
			getEndOfFrame(frame)->next_binding = createIntBinding(token,value,NULL);
			frame->size ++;
		}
	}
}

//adds a new binding to a frame
void addNewFuncBinding(FRAME* frame, TOKEN* token, CLOSURE* closure){
	if(checkFrameForToken(frame,token)){
		printf("Declaration Error. TOKEN %s already exists in the frame.\n", token->lexeme);
		exit(0);
	} else {
		if(frame->size == 0){
			frame->initial_binding = createFuncBinding(token,closure,NULL);
			frame->size ++;
		} else {
			getEndOfFrame(frame)->next_binding = createFuncBinding(token,closure,NULL);
			frame->size ++;
		}
	}
}


int getBindingValue(FRAME* frame, TOKEN* token){
	BINDING* b = NULL;
	// printf("getBindingValue, Frame: %s\n", frame->id);

	if(frame->initial_binding != NULL){
		b = getBindingFromToken(frame->initial_binding, token);
		if(b!=NULL){
			return b->value;
		}
	}

	if(frame->parent_frame != NULL){
		return getBindingValue(frame->parent_frame, token);
	}

	return -1;
}

CLOSURE* getBindingFunction(FRAME* frame, TOKEN* token){
	
	BINDING* b = NULL;
	//printf("checking frame: %s\n",frame->id);

	if(frame->initial_binding != NULL){
		b = getBindingFromToken(frame->initial_binding, token);
		if(b!=NULL){
			return b->closure;
		}
	}
	 
	if(frame->parent_frame != NULL){
		return getBindingFunction(frame->parent_frame,token);
	}
	

	// printf("%s: parent_frame is NULL %i\n",frame->id, frame->parent_frame==NULL);

	return NULL;

}

void bindFunctionArguments(FRAME* frame,FRAME* var_frame, NODE* variables, NODE* values){
	// printf("bindingfunctionarguments\n");
	if(values != NULL){
		if(values->type == ','){
			if(values->left != NULL){
				bindFunctionArguments(frame,var_frame, variables->left, (values->left));
			}

			if(values->right != NULL){
				bindFunctionArguments(frame,var_frame, variables->right, (values->right));
			}
		} else {

			TOKEN* var_token = (TOKEN*)(variables->right->left);
			TOKEN* val_token = (TOKEN*)(values->left);

			CLOSURE* closure = getBindingFunction(var_frame,val_token);

			// printf("%s\n", val_token->lexeme);

			if(closure != NULL){
				// printf("\tclosure not null\n");
					//TODO functions
				addNewFuncBinding(frame,var_token,closure);
			} else {
				// printf("\tclosure null\n");

				NODE* n = interpretWithFrame(values,var_frame);
				int i = getLeafValue(n);

				// printf("ARGS. BINDING: %s TO %i IN FRAME: %s\n",var_token->lexeme,i,frame->id);

				addNewIntBinding(frame,var_token,i);
			}
			
		}
	}
}

void printAllBindings(BINDING* binding){
	if(binding->next_binding == NULL){

	} else {
		printAllBindings(binding->next_binding);
	}
}