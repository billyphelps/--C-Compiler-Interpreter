#include "Set.h"
#include "C.tab.h"

//MAKE SURE ELEMENTS OF SIZE 1000
Set* createSet(TOKEN** elements){

	int size = 0;

	while (elements[size] != NULL)
	{
		size++;
	}

	Set* new_set = (Set*)malloc(sizeof(Set));
	if(new_set == NULL)
    {
        printf("Error creating a new BINDING.\n");
        exit(0);
    }

    new_set->elements = elements;
    new_set->number_of_elements = size;

    return new_set;
}

Set* emptySet(){
	TOKEN** c = (TOKEN**)malloc(1000*sizeof(TOKEN*));
	return createSet(c);
}

TOKEN* addToSet(Set* set, TOKEN* element){

	int index = 0;

	while(index < set->number_of_elements){
		if(set->elements[index] == element){
			return set->elements[index];
		}
		index++;
	}
	
	set->elements[set->number_of_elements] = element;
	set->number_of_elements++;

}

int setContains(Set* set, TOKEN* element){

	for (int i = 0; i < set->number_of_elements; ++i)
	{
		if(set->elements[i] == element){
			return 1;
		}
	}

	return 0;
}

Set* removeElementFromSet(Set* set, TOKEN* element){
	int index = 0;

	while(index < set->number_of_elements){
		if(set->elements[index] == element){
			break;
		}
		index++;
	}

	if(index == set->number_of_elements){
		return set;
	}

	while(index < set->number_of_elements){
		set->elements[index] = set->elements[index+1];
		index++;
	}

	set->number_of_elements--;

	return set;
}

Set* removeSetFromSet(Set* set, Set* removalSet){
	if(set->number_of_elements == 0 || removalSet->number_of_elements == 0){
		return set;
	}

	Set* dup = emptySet();
	duplicateSet(dup, set);

	for (int i = 0; i < 1000; ++i)
	{
		if(removalSet->elements[i] != NULL){
			removeElementFromSet(dup,removalSet->elements[i]);
		}
	}

	return dup;
}

Set* unionOfSets(Set* set1, Set* set2){
	if(set1->number_of_elements == 0){
		return set2;
	}

	if(set2->number_of_elements == 0){
		return set1;
	}

	Set* unionSet = emptySet();

	duplicateSet(unionSet,set2);

	for (int i = 0; i < set1->number_of_elements; ++i)
	{
		addToSet(unionSet,set1->elements[i]);
	}
	return unionSet;
}

void freeSet(Set* set){
	for (int i = 0; i < 1000; ++i)
	{
		free(set->elements[i]);
	}
	free(set->elements);
	free(set);
}

//sets must be ordered?
int setsAreEqual(Set* set1, Set* set2){

	int index1 = 0;

	while(set1->elements[index1] != NULL){
		int index2 = 0;
		while(set2->elements[index2] != NULL){
			if(set1->elements[index1] == set2->elements[index2]){
				break;
			}
			index2++;
		}
		if(set2->elements[index2] == NULL){
			return 0;
		}
		index1++;
	}

	return 1;

}

//copies set2 into set1
void duplicateSet(Set* destination, Set* set){

	destination->number_of_elements = set->number_of_elements;

	for (int i = 0; i < set->number_of_elements; ++i)
	{
		destination->elements[i] = set->elements[i];
	}
}

void printSet(Set* set){
	if(set != NULL && set->number_of_elements != 0){

		for (int i = 0; set->elements[i] != NULL; ++i)
		{
			TOKEN* temp = set->elements[i];
			if(temp->type == CONSTANT){
				printf("%d, ", temp->value);
			} else {
				if( temp->type == IDENTIFIER){
					printf("%s, ", temp->lexeme);
				}
			}
			
		}
	}
}
