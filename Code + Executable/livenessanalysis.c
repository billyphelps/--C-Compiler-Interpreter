#include "livenessanalysis.h"

Set* createGenSet(TACLINE* tacline){
	//LABEL
	if(tacline->operator == LABEL){
		return emptySet();
	}

	//FUNC
	if(tacline->operator == FUNC){
		return emptySet();
	}

	//END
	if(tacline->operator == END){
		return emptySet();
	}

	//GOTO
	if(tacline->operator == GOTO){
		return emptySet();
	}

	//CALL
	if(tacline->operator == CALL){
		return emptySet();
	}

	//RTRN
	if(tacline->operator == RTRN){
		
		if(tacline->destination->type == CONSTANT){
			return emptySet();
		}
		//Returning Variable or Function
		TOKEN** set = (TOKEN**)malloc(1000*sizeof(TOKEN*));
		set[0] = tacline->destination;
		return createSet(set);
	}

	//PUSH
	if(tacline->operator == PUSH){
		//Assiging Constant
		if(tacline->destination->type == CONSTANT){
			return emptySet();
		}

		TOKEN** set = (TOKEN**)malloc(1000*sizeof(TOKEN*));
		set[0] = tacline->destination;
		return createSet(set);
	}

	//POP
	if(tacline->operator == POP){
		return emptySet();
	}

	//IFN
	if(tacline->operator == IFN){
		//Assiging Constant
		if(tacline->operand1->type == CONSTANT){
			return emptySet();
		}
		if(tacline->operand1->type == IDENTIFIER){
			TOKEN** set = (TOKEN**)malloc(1000*sizeof(TOKEN*));
			set[0] = tacline->operand1;
			return createSet(set);
		}
	}

	//assignment
	if(tacline->operator == LOAD){
		//Assiging Constant
		if(tacline->operand1->type == CONSTANT){
			return emptySet();
		}
		//Assigning Identifier
		if(tacline->operand1->type == IDENTIFIER){
			TOKEN** set = (TOKEN**)malloc(1000*sizeof(TOKEN*));
			set[0] = tacline->operand1;
			return createSet(set);
		}
	}

	//unary operation
	if(tacline->operand2 == NULL){
		//Operation on Identifier
		if(tacline->operand1->type == IDENTIFIER){
			TOKEN** set = (TOKEN**)malloc(1000*sizeof(TOKEN*));
			set[0] = tacline->operand1;
			return createSet(set);
		}
		//Operation on Constant
		if(tacline->operand1->type == CONSTANT){
			return emptySet();
		}
	}

	//binary operations
	if(tacline->operand1->type == IDENTIFIER){
		//Identifier and Identifier
		if(tacline->operand2->type == IDENTIFIER){
			TOKEN** set = (TOKEN**)malloc(1000*sizeof(TOKEN*));
			set[0] = tacline->operand1;
			set[1] = tacline->operand2;
			return createSet(set);
		}
		//Identifier and Constant
		if(tacline->operand2->type == CONSTANT){
			TOKEN** set = (TOKEN**)malloc(1000*sizeof(TOKEN*));
			set[0] = tacline->operand1;
			return createSet(set);
		}
	}
	//Constant and Constant
	if(tacline->operand1->type == CONSTANT && tacline->operand2->type == CONSTANT){
		return emptySet();
	}

	return emptySet();

}





Set* createKillSet(TACLINE* tacline){
	//LABEL
	if(tacline->operator == LABEL){
		return emptySet();
	}

	//FUNC
	if(tacline->operator == FUNC){
		return emptySet();
	}

	//END
	if(tacline->operator == END){
		return emptySet();
	}

	//GOTO
	if(tacline->operator == GOTO){
		return emptySet();
	}

	//RTRN
	if(tacline->operator == RTRN){
		return emptySet();
	}

	//IFN
	if(tacline->operator == IFN){
		return emptySet();
	}

	//PUSH
	if(tacline->operator == PUSH){
		return emptySet();
	}

	//POP
	if(tacline->operator == POP){
		return emptySet();
	}

	//CALL
	if(tacline->operator == CALL){
		TOKEN** set = (TOKEN**)malloc(1000*sizeof(TOKEN*));
		set[0] = tacline->destination;
		return createSet(set);
	}

	//assignment
	if(tacline->operator == LOAD){
		//Assigning Identifier
		if(tacline->operand1->type == IDENTIFIER){
			TOKEN** set = (TOKEN**)malloc(1000*sizeof(TOKEN*));
			set[0] = tacline->destination;
			return createSet(set);
		}
		//Assiging Constant
		if(tacline->operand1->type == CONSTANT){
			TOKEN** set = (TOKEN**)malloc(1000*sizeof(TOKEN*));
			set[0] = tacline->destination;
			return createSet(set);
		}
	}

	//unary operation
	if(tacline->operand2 == NULL){
		//Operation on Identifier
		if(tacline->operand1->type == IDENTIFIER){
			TOKEN** set = (TOKEN**)malloc(1000*sizeof(TOKEN*));
			set[0] = tacline->destination;
			return createSet(set);
		}
		//Operation on Constant
		if(tacline->operand1->type == CONSTANT){
			TOKEN** set = (TOKEN**)malloc(1000*sizeof(TOKEN*));
			set[0] = tacline->destination;
			return createSet(set);
		}
	}

	//binary operations
	if(tacline->operand1->type == IDENTIFIER){
		//Identifier and Identifier
		if(tacline->operand2->type == IDENTIFIER){
			TOKEN** set = (TOKEN**)malloc(1000*sizeof(TOKEN*));
			set[0] = tacline->destination;
			return createSet(set);
		}
		//Identifier and Constant
		if(tacline->operand2->type == CONSTANT){
			TOKEN** set = (TOKEN**)malloc(1000*sizeof(TOKEN*));
			set[0] = tacline->destination;
			return createSet(set);
		}
	}

	if(tacline->operand2->type == IDENTIFIER){
		//Identifier and Identifier
		if(tacline->operand1->type == IDENTIFIER){
			TOKEN** set = (TOKEN**)malloc(1000*sizeof(TOKEN*));
			set[0] = tacline->destination;
			return createSet(set);
		}
		//Identifier and Constant
		if(tacline->operand1->type == CONSTANT){
			TOKEN** set = (TOKEN**)malloc(1000*sizeof(TOKEN*));
			set[0] = tacline->destination;
			return createSet(set);
		}
	}

	//Constant and Constant
	if(tacline->operand1->type == CONSTANT && tacline->operand2->type == CONSTANT){
		TOKEN** set = (TOKEN**)malloc(1000*sizeof(TOKEN*));
		set[0] = tacline->destination;
		return createSet(set);
	}

	return emptySet();

}

void setInOutEmpty(LivenessSets* lSets){
	lSets->in = emptySet();
	lSets->out = emptySet();
	lSets->old_in = emptySet();
	lSets->old_out = emptySet();
}

void setAllInOutEmpty(LivenessSets** ListOflSets){
	for (int i = 0; i < getTACArraySize(); ++i)
	{
		setInOutEmpty(ListOflSets[i]);
	}
}

Set* unionOfSuccessorIns(LivenessSets* lSets){
	Set* unions = emptySet();

	if(lSets->succ == NULL){
		return unions;
	}

	for (int i = 0; lSets->succ[i] != NULL; ++i)
	{
		unions = unionOfSets(unions,lSets->succ[i]->in);
	}


	return unions;
}

void updateLivenessSets(LivenessSets* lSets){
	duplicateSet(lSets->old_in,lSets->in);
	duplicateSet(lSets->old_out,lSets->out);

	lSets->out = unionOfSuccessorIns(lSets);
	lSets->in = unionOfSets(lSets->gen,removeSetFromSet(lSets->out,lSets->kill));
}

void updateAllLivenessSets(LivenessSets** ListOflSets){
	for (int i = getTACArraySize() - 1; i > -1; --i)
	{
		updateLivenessSets(ListOflSets[i]);
	}
}

int testConvergence(LivenessSets** ListOflSets){

	int convergence = 1;

	for (int i = getTACArraySize()-1; i > -1; --i)
	{
		if(!(setsAreEqual(ListOflSets[i]->in,ListOflSets[i]->old_in) && setsAreEqual(ListOflSets[i]->out,ListOflSets[i]->old_out))){
			convergence = 0;
			break;
		}
	}
	return convergence;
}

//Creates a LivenessSets and creates Kill/Gen sets from provided TAC
LivenessSets* createLivenessSetsFromTAC(TACLINE* tacline){
	LivenessSets* new_liveness_sets = (LivenessSets*)malloc(sizeof(LivenessSets));
	if(new_liveness_sets == NULL)
    {
        printf("Error creating a new LivenessSets.\n");
        exit(0);
    }
    new_liveness_sets->kill = createKillSet(tacline);
	new_liveness_sets->gen = createGenSet(tacline);

	return new_liveness_sets;
}

//Creates ListofLivenessSets from a supplied Array of TACLINEs. Creates Kill/Gen sets for each LivenessSets
LivenessSets** createListOfLSets(TACLINE** TACarray){
	int sizeOfArray = getTACArraySize();

	LivenessSets** ListOflSets = (LivenessSets**)malloc(sizeOfArray * sizeof(LivenessSets*));

	for (int i = 0; i < sizeOfArray; ++i)
	{
		ListOflSets[i] = createLivenessSetsFromTAC(TACarray[i]);
	}

	return ListOflSets;
}


LivenessSets** setSuccesors(LivenessSets** ListOflSets){
	TACLINE** tac_array = getTACArray();

	for (int i = getTACArraySize() - 2; i > -1; --i)
	{
		LivenessSets** ListOfSucc;

		switch(tac_array[i]->operator){
			default:{
				ListOfSucc = (LivenessSets**)malloc(1*sizeof(LivenessSets*));

				ListOfSucc[0] = ListOflSets[i+1];

				break;
			}

			case END:{
				ListOfSucc = NULL;
				break;
			}

			case IFN:{
				ListOfSucc = (LivenessSets**)malloc(2*sizeof(LivenessSets*));

				ListOfSucc[0] = ListOflSets[i+1];
				for (int j = i+1; j < getTACArraySize() - 1; ++j)
				{
					if(tac_array[j]->operator == LABEL && tac_array[j]->destination == tac_array[i]->destination){
						ListOfSucc[1] = ListOflSets[j];
						break;
					}
				}
				break;
			}


			case GOTO:{
				ListOfSucc = (LivenessSets**)malloc(1*sizeof(LivenessSets*));

				for (int j = 0; j < getTACArraySize()-1; ++j)
				{
					if(tac_array[j]->operator == LABEL && tac_array[j]->destination == tac_array[i]->destination){
						ListOfSucc[0] = ListOflSets[j];
						break;
					}
				}
				break;
			}

			case RTRN:{
				ListOfSucc = NULL;
				break;
			}

		}

		ListOflSets[i]->succ = ListOfSucc;
	}

	//Last TAC has no successor
	ListOflSets[getTACArraySize()-1]->succ = NULL;

	return ListOflSets;
}

LivenessSets** solveDataFlowEquations(LivenessSets** ListOflSets){

	setAllInOutEmpty(ListOflSets);

	do {
		updateAllLivenessSets(ListOflSets);
	} while(!testConvergence(ListOflSets));

	return ListOflSets;
}

GraphEdge** interferingVariables(Set* kill, Set* out){

	GraphEdge** interfering_variables = (GraphEdge**)malloc(1000*sizeof(GraphEdge*));
	int num_of_edges = 0;

	int i = 0;

	while(i < kill->number_of_elements){
		int j = 0;
		while(j < out->number_of_elements){
			if(kill->elements[i] != out->elements[j]){
				interfering_variables[num_of_edges++] = createGraphEdge(kill->elements[i],out->elements[j]);
			}
			j++;
		}
		i++;
	}

	return interfering_variables;

}

Graph* conflictingVariables(LivenessSets** ListOflSets){
	GraphEdge** conflicting_variables = (GraphEdge**)malloc(1000*sizeof(GraphEdge*));
	int num_of_edges = 0;
	int i = 0;

	while(i < getTACArraySize()){
		
		GraphEdge** temp = interferingVariables(ListOflSets[i]->kill, ListOflSets[i]->out);

		int j = 0;

		while(temp[j] != NULL){
			addGraphEdge(conflicting_variables,temp[j]);
			j++;
		}
		i++;
	}

	return createGraph(conflicting_variables);

}



void printAllKillOuts(LivenessSets** ListOflSets){
	for (int i = 0; ListOflSets[i] != NULL; ++i)
	{
		printf("kill: ");
		printSet(ListOflSets[i]->kill);
		printf("\n");
		printf("out:  ");
		printSet(ListOflSets[i]->out);
		printf("\n");
	}
}


void printLivenessSets(LivenessSets* lSets){
	printf("printLivenessSets\n");
	printf("kill:");
	printSet(lSets->kill);
	printf("\n");

	printf("gen:");
	printSet(lSets->gen);
	printf("\n");


	printf("in:");
	printSet(lSets->in);
	printf("\n");


	printf("old_in:");
	printSet(lSets->old_in);
	printf("\n");


	printf("out:");
	printSet(lSets->out);
	printf("\n");


	printf("old_out:");
	printSet(lSets->old_out);
	printf("\n");

}

void printAllLivenessSets(LivenessSets** ListOflSets){
	for (int i = 0; i < getTACArraySize(); ++i)
	{
		printLivenessSets(ListOflSets[i]);
		printf("\n");
	}
}





