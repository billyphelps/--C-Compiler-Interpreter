#include "conditionals.h"


int evaluateConditional(NODE* operator, int operand1, int operand2){

	switch(operator->type){

		case '<':{
			return operand1 < operand2;
		}

		case '>':{
			return operand1 > operand2;
		}

		case LE_OP:{
			return operand1 <= operand2;
		}

		case GE_OP:{
			return operand1 >= operand2;
		}

		case EQ_OP:{
			return operand1 == operand2;
		}

		case NE_OP:{
			return operand1 != operand2;
		}




	}

}