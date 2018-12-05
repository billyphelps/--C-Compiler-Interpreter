#include "tacgenerator.h"
#include "environment.h"
#include "TAC.h"

NODE* constantToLeaf(int constant);

bool declaring_variableTAC = false;

int function_args = 0;


int variable_number = 0;

TOKEN* getNewTemporary(){
	char* c;
	asprintf(&c,"%s%d","$T",variable_number++);
	return lookup_token(c);
}

int saved_number = 0;

TOKEN* getNewSaved(){
	char* c;
	asprintf(&c,"%s%d","$s",saved_number++);
	return lookup_token(c);
}

int arg_number = 0;

TOKEN* getNewArg(){
	char* c;
	asprintf(&c,"%s%d","$a",arg_number--);
	return lookup_token(c);
}

int label_number = 0;

TOKEN* getNewLabel(){
	char* c;
	asprintf(&c,"%s%s%d","L","_",label_number++);
	return lookup_token(c);
}

TOKEN* return_label;
TOKEN* return_value;

TOKEN* generateTAC(NODE* ast){
	return_label = lookup_token("$rl");
	return_value = lookup_token("$rv");

	return createTAC(ast);
}

TOKEN** arg_stack;
int arg_stack_pos = 0;

TOKEN* poptac(NODE* ast){
	if(ast->left->type == ','){
		poptac(ast->left);
	} else {
		arg_stack[arg_stack_pos++] = createTAC(ast->left);
	}

	if(ast->right->type == ','){
		poptac(ast->right);
	} else {
		arg_stack[arg_stack_pos++] = createTAC(ast->right);
	}
	return NULL;
}

TOKEN* popTAC(NODE* ast){
	arg_stack = (TOKEN**)malloc(getTACArraySize() * sizeof(TOKEN*));
	arg_stack_pos = 0;

	TOKEN* t = poptac(ast);

	for (int i = arg_stack_pos-1; i > -1; --i)
	{
		createTACLINE(POP,NULL,NULL,arg_stack[i]);
	}

	return t;
}

TACLINE** evaluateArgsFirst(NODE* ast){

}

TOKEN* createTAC(NODE* ast){
	switch (ast->type) {

		case '~':
		{
			TOKEN* left_result = createTAC(ast->left);

			return createTAC(ast->right);
		}

		case 'D':{
			TOKEN* label = createTAC(ast->left->right->left);
			createTACLINE(FUNC,NULL,NULL,label);
			NODE* func_args = ast->left->right->right;
			
			TOKEN* t = (TOKEN*)(ast->left->right->left->left);
					
			if(func_args != NULL && strcmp(t->lexeme,"main")){
				if(func_args->type == ','){
					popTAC(func_args);
				} else {
					
					createTACLINE(POP,NULL,NULL,createTAC(func_args->right));
				
				}
			}
			
			createTAC(ast->right);
			createTACLINE(END,NULL,NULL,label);
			return NULL;
		}

		case APPLY:{
			function_args = 0;
			if(ast->right != NULL){

				if(ast->right->type == ','){
					int start = getTACArraySize();
					int args_found = 0;

					// printf("BEFORE CREATING\n");
					// printTACLINEs();

					createTAC(ast->right);

					// printf("AFTER CREATING\n");
					// printTACLINEs();

					int num_of_lines = getTACArraySize() - start;

					TACLINE* pushes[num_of_lines];
					int push_size = 0;
					TACLINE* others[num_of_lines];
					int other_size = 0;

					TACLINE** tac_array = getTACArray();


					for (int i = start; args_found < function_args ; ++i)
					{
						if(tac_array[i]->operator == PUSH){
							pushes[push_size++] = tac_array[i];
							args_found++;
						} else {
							others[other_size++] = tac_array[i];
						}
					}

					// printf("Pushes Size: %i\n",push_size);
					// for (int i = 0; i < push_size; ++i)
					// {
					// 	printTACLINE(pushes[i]);
					// }
					// printf("Others Size: %i\n",other_size);
					// for (int i = 0; i < other_size; ++i)
					// {
					// 	printTACLINE(others[i]);
					// }

					int tac_pos = start;
					// printf("Start: %i\n", start);

					for (int i = 0 ; i < other_size ; ++i)
					{
						tac_array[tac_pos++] = others[i];
					}

					for (int i = 0 ; i < (other_size + push_size) ; ++i)
					{
						tac_array[tac_pos++] = pushes[i];
					}

					// printf("AFTER REARRANGING\n");
					// printTACLINEs();

				} else {
					function_args++;
					createTACLINE(PUSH,NULL,NULL,createTAC(ast->right));
				}


			}
			TOKEN* num_of_args = (TOKEN*)(constantToLeaf(function_args)->left);
			TOKEN* call = createTACLINE(CALL,createTAC(ast->left),num_of_args,getNewTemporary())->destination;

			return call;
		}

		case ',':{
			if(ast->left->type == ','){
				createTAC(ast->left);
			} else {
				createTACLINE(PUSH,NULL,NULL,createTAC(ast->left));
				function_args++;
			}

			if(ast->right->type == ','){
				createTAC(ast->right);
			} else {
				createTACLINE(PUSH,NULL,NULL,createTAC(ast->right));
				function_args++;
			}
			return NULL;
		}



		case WHILE:{
			TOKEN* loop_label = getNewLabel();
			createTACLINE(LABEL,NULL,NULL,loop_label);
			TOKEN* conditional = createTAC(ast->left);
			TOKEN* end_label = getNewLabel();
			createTACLINE(IFN,conditional,NULL,end_label);
			createTAC(ast->right);
			createTACLINE(GOTO,NULL,NULL,loop_label);
			createTACLINE(LABEL,NULL,NULL,end_label);
			return NULL;
		}

		case IF:{
			TOKEN* conditional = createTAC(ast->left);
			TOKEN* end_label = getNewLabel();

			if(ast->right->type == ELSE){
				TOKEN* else_label = getNewLabel();

				createTACLINE(IFN,conditional,NULL,else_label);
				
				if(ast->right->left != NULL){
					createTAC(ast->right->left);
				}
				
				createTACLINE(GOTO,NULL,NULL,end_label);
				createTACLINE(LABEL,NULL,NULL,else_label);
				
				createTAC(ast->right->right);
			} else {
				createTACLINE(IFN,conditional,NULL,end_label);
				createTAC(ast->right);
			}
			createTACLINE(LABEL,NULL,NULL,end_label);
			return NULL;
		}

		case '<':{
			return createTACLINE(LT,createTAC(ast->left),createTAC(ast->right),getNewTemporary())->destination;
		}

		case '>':{
			return createTACLINE(GT,createTAC(ast->left),createTAC(ast->right),getNewTemporary())->destination;
		}

		case LE_OP:{
			return createTACLINE(LTE,createTAC(ast->left),createTAC(ast->right),getNewTemporary())->destination;
		}

		case GE_OP:{
			return createTACLINE(GTE,createTAC(ast->left),createTAC(ast->right),getNewTemporary())->destination;
		}

		case EQ_OP:{
			return createTACLINE(EQ,createTAC(ast->left),createTAC(ast->right),getNewTemporary())->destination;
		}

		case NE_OP:{
			return createTACLINE(NE,createTAC(ast->left),createTAC(ast->right),getNewTemporary())->destination;
		}

			

		case ';':
			createTAC(ast->left);
			return createTAC(ast->right);
			
		case '+':
			return createTACLINE(ADD,createTAC(ast->left),createTAC(ast->right),getNewTemporary())->destination;

		case '-':
			return createTACLINE(SUBT,createTAC(ast->left),createTAC(ast->right),getNewTemporary())->destination;

		case '/':
			return createTACLINE(DIVI,createTAC(ast->left),createTAC(ast->right),getNewTemporary())->destination;

		case '*':
			return createTACLINE(MULT,createTAC(ast->left),createTAC(ast->right),getNewTemporary())->destination;

		case LEAF:	
		{
			TOKEN* token = (TOKEN *)(ast->left);

			switch(token->type){

				default:	
					break;

				case CONSTANT:
					return token;

	    		case INT:
					declaring_variableTAC = true;
					return token;

				case IDENTIFIER:
					return token; 


			}
		}

			

		case RETURN:{

			TOKEN* t = createTAC(ast->left);

			if(t->type == CONSTANT){
				createTACLINE(RTRN,NULL,NULL,t);
				return t;
			} else {
				return createTACLINE(RTRN,NULL,NULL,t)->destination;
			}

		}

		case '=':
			return createTACLINE(LOAD,createTAC(ast->right),NULL,createTAC(ast->left))->destination;


	}
}

TACLINE** argsToArgs(TACLINE** tac_array){

	TOKEN* args[getTACArraySize()];
	TOKEN* temps[getTACArraySize()];
	int bindings = 0;
	arg_number = 0;
	int first_pop = 0;

	for (int i = 0; i < getTACArraySize(); ++i)
	{
		if(tac_array[i]->operator == POP && !first_pop){
			first_pop = 1;
			arg_number = 0;

			for (int j = i; tac_array[j]->operator == POP ; ++j)
			{
				arg_number++;
			}

			//decrement as arg registers start at 0
			arg_number--;
		}

		if(tac_array[i]->operator == POP){

			args[bindings] = tac_array[i]->destination;
			temps[bindings++] = getNewArg();
		}

		for (int j = 0; j < bindings; ++j)
		{
			if(tac_array[i]->operand1 == args[j] && tac_array[i]->operand1 != NULL){
				tac_array[i]->operand1 = temps[j];
			}

			if(tac_array[i]->operand2 == args[j] && tac_array[i]->operand2 != NULL){
				tac_array[i]->operand2 = temps[j];
			}

			if(tac_array[i]->destination == args[j] && tac_array[i]->destination != NULL){
				tac_array[i]->destination = temps[j];
			}
		}

		if(tac_array[i]->operator == END){
			for (int j = 0; j < bindings; ++j)
			{
				args[j] = NULL;
				temps[j] = NULL;
			}
			bindings = 0;
			arg_number = 0;
		}

	}


	return tac_array;
}

