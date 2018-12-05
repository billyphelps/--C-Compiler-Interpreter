#include "TAC.h"

TACLINE** TAC_array;
int array_pos = 0;

char* operator_strings [] = {"ADD", "SUBT","MULT","DIVI","LOAD","STO","RTRN","LABEL","GOTO","IFN","LT","GT","LTE","GTE","EQ","NE","PUSH","POP","FUNC","CALL","END"};


void initializeTACArray(){
	TAC_array = (TACLINE**)malloc(100*sizeof(TACLINE*));
}

char* operandValue(TOKEN* operand){

	char* c1;
	if(operand == NULL){
		return NULL;
	}
	
	if(operand->type == CONSTANT){
		asprintf(&c1,"%d",operand->value);
	} else {
		c1 = operand->lexeme;
	}

	return c1;
}


void printTACLINE(TACLINE* tac_line){

	// char* c1 = operandValue(tac_line->operand1);
	// char* c2 = operandValue(tac_line->operand2);
	// char* c3 = operandValue(tac_line->destination);

	// if(c1 == NULL) c1 = "";
	// if(c2 == NULL) c2 = "";
	// if(c3 == NULL) c3 = "";

	// printf("%s\t%s\t%s\t%s \n", operator_strings[tac_line->operator], c3, c1, c2);

}

void printTACLINEs(){
	// printf("\n");
	// printf("Printing All TAC:\n");
	// printf("\n");
	// for (int i=0;i < array_pos;i++) {
	// 		if(TAC_array[i]->operator < sizeof(operator_strings)/sizeof(char*)){
	// 		printTACLINE(TAC_array[i]);
	// 	}
	    
	// }
	// fflush(stdout);
}

TACLINE* createTACLINE(operator op, TOKEN* oper1, TOKEN* oper2, TOKEN* dest){
	TACLINE* new_tacline = (TACLINE*)malloc(sizeof(TACLINE));
	if(new_tacline == NULL)
    {
        printf("Error creating a new TACLINE.\n");
        exit(0);
    }

    new_tacline->operator = op;
    new_tacline->operand1 = oper1;
    new_tacline->operand2 = oper2;
    new_tacline->destination = dest;

    TAC_array [array_pos] = new_tacline;

    return TAC_array[array_pos++];
}





TACLINE** getTACArray(){
	return TAC_array;
}

int getTACArraySize(){
	return array_pos;
}





