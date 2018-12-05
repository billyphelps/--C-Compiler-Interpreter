#ifndef _tac_
#define _tac_

#include "symbol_table.h"
#include "C.tab.h"

typedef enum operator {
	ADD,
	SUBT,
	MULT,
	DIVI,
	LOAD,
	STO,
	RTRN,
	LABEL,
	GOTO,
	IFN,
	LT,
	GT,
	LTE,
	GTE,
	EQ,
	NE,
	PUSH,
	POP,
	FUNC,
	CALL,
	END
} operator;

typedef struct TACLINE{
	operator operator;
	TOKEN* operand1;
	TOKEN* operand2;
	TOKEN* destination;
} TACLINE;


void initializeTACArray();
char* operandValue(TOKEN* operand);
void printTACLINE(TACLINE* tac_line);
void printTACLINEs();
TACLINE* createTACLINE(operator op, TOKEN* oper1, TOKEN* oper2, TOKEN* dest);
TACLINE** getTACArray();
int getTACArraySize();




#endif

