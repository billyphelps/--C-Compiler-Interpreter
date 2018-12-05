#include "main.h"
#include "tacgenerator.h"
#include "TACtoMIPS.h"

FRAME* rootFRAME;
FRAME* currentFRAME;

int interpreter_result = 0;


LivenessSets** solveDataFlow(TACLINE** tac_array){
	LivenessSets** genkillsets = createListOfLSets(tac_array);
	LivenessSets** genkillsuccsets = setSuccesors(genkillsets);

	LivenessSets** solved = solveDataFlowEquations(genkillsuccsets);

	return solved;
}


NodeColorStack* colorRegisters(LivenessSets** solved){

	Graph* interferingVar = conflictingVariables(solved);
	// printf("Interfering Variables:\n");
	// printGraphEdges(interferingVar);

	if(interferingVar->number_of_edges > 0){
		NodeStack* nStack = graphToStack(interferingVar, getGraphNodes(interferingVar), 18);
		NodeColorStack* colors = colorsFromStack(nStack, 18);
		// printNodeColors(colors->elements, colors->size);
		return colors;
	} else {
		// printf("No Interfering Variables.\n");
		return NULL;
	}
}

TOKEN* colorToToken(int color){
	char* c;
	asprintf(&c,"%s%d","$",color+8);
	return lookup_token(c);
}

void colorTAC(TACLINE** tac_array, NodeColorStack* colors){
	for (int i = 0; i < getTACArraySize(); ++i)
	{
		switch(tac_array[i]->operator){

			case END:
			case FUNC:
			case LABEL:
			case GOTO:{
				break;
			}

			case CALL:{
				if(tac_array[i]->destination != NULL && tac_array[i]->destination->type == IDENTIFIER){
					TOKEN* token_color = colorToToken(0);
					for (int j = 0; j < colors->size; ++j)
					{
						if(tac_array[i]->destination == colors->elements[j]->node){
							token_color = colorToToken(colors->elements[j]->color);
							break;
						} 
					}
					tac_array[i]->destination = token_color;
				}
				break;
			}

			case IFN:{
				if(tac_array[i]->operand1 != NULL && tac_array[i]->operand1->type == IDENTIFIER){
					TOKEN* token_color = colorToToken(0);
					for (int j = 0; j < colors->size; ++j)
					{
						if(tac_array[i]->operand1!= NULL && tac_array[i]->operand1 == colors->elements[j]->node){
							token_color = colorToToken(colors->elements[j]->color);
							break;
						}
					}
					tac_array[i]->operand1 = token_color;
				}
				break;
			}

			default:{
				if(tac_array[i]->operand1 != NULL && tac_array[i]->operand1->type == IDENTIFIER){
					TOKEN* token_color = colorToToken(0);
					for (int j = 0; j < colors->size; ++j)
					{
						if(tac_array[i]->operand1 != NULL && tac_array[i]->operand1 == colors->elements[j]->node){
							token_color = colorToToken(colors->elements[j]->color);
							break;
						}
					}
					tac_array[i]->operand1 = token_color;
				}

				if(tac_array[i]->operand2 != NULL && tac_array[i]->operand2->type == IDENTIFIER){
					TOKEN* token_color = colorToToken(0);
					for (int j = 0; j < colors->size; ++j)
					{
						if(tac_array[i]->operand2 != NULL && tac_array[i]->operand2 == colors->elements[j]->node){
							token_color = colorToToken(colors->elements[j]->color);
							break;
						}
					}
					tac_array[i]->operand2 = token_color;
				}

				if(tac_array[i]->destination != NULL && tac_array[i]->destination->type == IDENTIFIER){
					TOKEN* token_color = colorToToken(0);
					for (int j = 0; j < colors->size; ++j)
					{
						if(tac_array[i]->destination == colors->elements[j]->node){
							token_color = colorToToken(colors->elements[j]->color);
							break;
						} 
					}
					tac_array[i]->destination = token_color;
				}
				break;
			}
			
		}
		
	}
}


void replaceLabels(TACLINE** tac_array){
	int size = getTACArraySize();

	TOKEN* old_labels[size];
	char* new_labels[size];

	int label_counter = 0;

	// printf("FUNC loop\n");
	for (int i = 0; i < size; ++i)
	{
		if(tac_array[i]->operator == FUNC){
			old_labels[label_counter] = tac_array[i]->destination;
			char* new_label;

			if(strcmp(tac_array[i]->destination->lexeme,"main")){
				asprintf(&new_label,"%s%i%s%s","F",label_counter,"_",tac_array[i]->destination->lexeme);
			} else {
				new_label = "prog_main";
			}
			
			tac_array[i]->destination->lexeme = new_label;
			new_labels[label_counter] = new_label;
			label_counter++;
		}
	}

	// printf("CALL loop\n");
	for (int i = 0; i < size; ++i)
	{
		if(tac_array[i]->operator == CALL){
			for (int j = 0; j < label_counter; ++j)
			{
				if(tac_array[i]->operand1 == old_labels[j]){
					tac_array[i]->operand1->lexeme = new_labels[j];
					break;
				}
			}
		}
	}
	
}

int saves_assigned = 0;

TOKEN* getNewSaveRegister(){
	char* c;
	asprintf(&c,"$s%d",saves_assigned++);
	return lookup_token(c);
}

void resetSavedRegisters(){
	saves_assigned = 0;
}

void replaceWithSaved(TOKEN* token, Set* vars_to_save, Set* save_registers){
	for (int i = 0; i < vars_to_save->number_of_elements; ++i)
	{
		 if(token->lexeme) printf("%s %s %i\n", token->lexeme,vars_to_save->elements[i]->lexeme,token == vars_to_save->elements[i]);
		if(token == vars_to_save->elements[i]){
			token = save_registers->elements[i];
			break;
		}
	}
}

void saveVariables(TACLINE** tac_array, LivenessSets** lSets){
	int current_func;
	Set* vars_to_save = emptySet();
	Set* save_registers = emptySet();
	int renaming = 0;

	for (int i = 0; i < getTACArraySize(); ++i)
	{

		if(renaming){

			if(tac_array[i]->operator == END){
				renaming = 0;
				vars_to_save = emptySet();
				save_registers = emptySet();
			}
			
			
			if(tac_array[i]->operand1) replaceWithSaved(tac_array[i]->operand1,vars_to_save,save_registers);
			if(tac_array[i]->operand2) replaceWithSaved(tac_array[i]->operand2,vars_to_save,save_registers);
			if(tac_array[i]->destination) replaceWithSaved(tac_array[i]->destination,vars_to_save,save_registers);

		} else {

			switch(tac_array[i]->operator){
				default:{

				}

				case FUNC:{
					current_func = i;
				}

				case CALL:{
					vars_to_save = unionOfSets(vars_to_save,lSets[i]->out);
				}

				case END:{
					i = current_func;
					renaming = 1;

					// printf("\n");
					// printf("END vars_to_save->number_of_elements %i\n", vars_to_save->number_of_elements);
					// printSet(vars_to_save);
					// printf("\n");

					for (int j = 0; j < vars_to_save->number_of_elements; ++j)
					{
						save_registers->elements[i] = getNewSaveRegister();
						save_registers->number_of_elements++;
					}

					resetSavedRegisters();
				}
			}

			
		}

		




	}
}

int main(int argc, char** argv){
	NODE* tree = parserMain(argc, argv);

	rootFRAME = createFrame(NULL);
	currentFRAME = rootFRAME;

	// print_tree(tree);


	interpreter_result = getLeafValue(callInterpreter(rootFRAME, currentFRAME, tree));
	printf("\n\n~~ RESULT = %d ~~\n", interpreter_result);

	rootFRAME = createFrame(NULL);
	currentFRAME = rootFRAME;

	initializeTACArray();

	generateTAC(tree);
	// printf("\nTAC produced. \n");
	// printTACLINEs();


	TACLINE** tac_array = getTACArray();
	// printf("\n\n");

	LivenessSets** solved = solveDataFlow(tac_array);

	NodeColorStack* colors = colorRegisters(solved);
	// printf("\n");
	
	colorTAC(tac_array,colors);

	argsToArgs(tac_array);

	replaceLabels(tac_array);
	// printTACLINEs();

	LivenessSets** new_solved = solveDataFlow(tac_array);

	printMips(tac_array,new_solved);


	return 0;
}