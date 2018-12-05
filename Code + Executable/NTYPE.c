#include <stdio.h>
#include <stdlib.h>


enum dataType {
	TYPE_VOID = 0,
    TYPE_INT = 1,
    TYPE_STRING = 2
};

//NODE TYPE
typedef struct NTYPE {
    enum dataType DATA_TYPE;
    union {
    	int dInt;
    	char* dString;
    };
} NTYPE;

NTYPE* createStringNTYPE(char* string)
{
    NTYPE* nodeType = (NTYPE*)malloc(sizeof(NTYPE));
    if(nodeType == NULL)
    {
        printf("Error creating a new NTYPE.\n");
        exit(0);
    }
    nodeType->DATA_TYPE = TYPE_STRING;
    nodeType->dString = string;
 
    return nodeType;
}

NTYPE* createIntNTYPE(int number)
{
    NTYPE* nodeType = (NTYPE*)malloc(sizeof(NTYPE));
    if(nodeType == NULL)
    {
        printf("Error creating a new NTYPE.\n");
        exit(0);
    }
    nodeType->DATA_TYPE = TYPE_INT;
    nodeType->dInt = number;
 
    return nodeType;
}

NTYPE* createVoidNTYPE()
{
    NTYPE* nodeType = (NTYPE*)malloc(sizeof(NTYPE));
    if(nodeType == NULL)
    {
        printf("Error creating a new NTYPE.\n");
        exit(0);
    }
    nodeType->DATA_TYPE = TYPE_VOID;
 
    return nodeType;
}


