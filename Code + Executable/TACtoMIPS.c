#include <string.h>
#include "TACtoMIPS.h"
#include "nodes.h"


LivenessSets** solved_sets;
TACLINE** tac_array;


int arg_stack_size = 0;
int branch_label = 0;

char* pushArg(TOKEN* arg){
	char* ans;
	char* save="";
	if(arg_stack_size<4){
		//first save the current value
		asprintf(&save,"sw $a%i %i($sp)",arg_stack_size,(4-arg_stack_size)*-4);

		if(arg != NULL && arg->type == CONSTANT){
			asprintf(&ans,"li $a%i %i",arg_stack_size,arg->value);
		} else {
			asprintf(&ans,"move $a%i %s",arg_stack_size,arg->lexeme);
		}
	} else {
		if(arg != NULL && arg->type == CONSTANT){
			asprintf(&ans,"li $v0 %i",arg->value);
			asprintf(&ans,"%s\nsw $v0 0($sp)",ans);
		} else {
			asprintf(&ans,"%s\nsw %s 0($sp)",ans,arg->lexeme);
		}
		asprintf(&ans,"%s\n%s",ans,"addi $sp $sp 4");
	}
	arg_stack_size++;
	return ans;
}

char* popArg(TOKEN* dest){
	char* ans;
	// asprintf(&ans,"%s","subi $sp $sp 4");
	// if(arg_stack_size<4){
	// 	asprintf(&ans,"move %s $a%i",dest->lexeme,arg_stack_size);
	// } else {
	// 	asprintf(&ans,"%s\nlw %s 0($sp)",ans,dest->lexeme);
	// 	asprintf(&ans,"%s\n%s",ans,"sub $sp $sp 4");
	// }
	// arg_stack_size--;
	return ans;
}

char* booleanOperation(TACLINE* tacline){

	char* opposite;

	switch(tacline->operator){
		case GT:{
			opposite = "ble";
			break;
		}

		case LT:{
			opposite = "bge";
			break;
		}

		case LTE:{
			opposite = "bgt";
			break;
		}

		case GTE:{
			opposite = "blt";
			break;
		}

		case EQ:{
			opposite = "bne";
			break;
		}

		case NE:{
			opposite = "beq";
			break;
		}

		default:{
			printf("not a boolean\n");
			exit(0);
			break;
		}
	}

	char* ans;

	char* boolean;
	char* boolean2;
	char* operator;
	char* label;
	asprintf(&boolean,"%s %s, %i","li",operandValue(tacline->destination),0);
	asprintf(&operator,"%s %s, %s, B_%i",opposite,operandValue(tacline->operand1),operandValue(tacline->operand2),branch_label);
	asprintf(&boolean2,"%s %s, %i","li",operandValue(tacline->destination),1);
	asprintf(&label,"B_%i:",branch_label);
	branch_label++;

	asprintf(&ans,"%s\n%s\n%s\n%s",boolean,operator,boolean2,label);
	return ans;
}



int getFunctionStart(TOKEN* func_name){
	for (int i = 0; i < getTACArraySize(); ++i)
	{
		if(tac_array[i]->operator == FUNC){
			if(tac_array[i]->destination == func_name){
				return i;
			}
		}
	}

	printf("Function:%s doesn't exist\n",func_name->lexeme);
	exit(0);
}

Set* getFunctionsKills(TOKEN* func_name){
	int func_start = getFunctionStart(func_name);
	Set* kills = emptySet();

	for (int i = 0; tac_array[i]->operator != END; ++i)
	{
		kills = unionOfSets(kills,solved_sets[i]->kill);
	}

	return kills;
}

Set* func_out;

int first_push = 1;

char* TACToMIPS(TACLINE* tacline, int line_no){
	char* mips = "not set";


	switch(tacline->operator){
		case ADD:{
			asprintf(&mips,"%s %s, %s, %s","add",operandValue(tacline->destination),operandValue(tacline->operand1),operandValue(tacline->operand2));
			break;
		}

		case SUBT:{
			asprintf(&mips,"%s %s, %s, %s","sub",operandValue(tacline->destination),operandValue(tacline->operand1),operandValue(tacline->operand2));
			break;
		}

		case MULT:{
			char* mult;
			char* mflo;
			asprintf(&mult,"%s %s, %s","mult",operandValue(tacline->operand1),operandValue(tacline->operand2));
			asprintf(&mflo,"%s %s","mflo",operandValue(tacline->destination));
			asprintf(&mips,"%s\n%s",mult,mflo);
			break;
		}

		case DIVI:{
			char* div;
			char* mflo;
			asprintf(&div,"%s %s, %s","div",operandValue(tacline->operand1),operandValue(tacline->operand2));
			asprintf(&mflo,"%s %s","mflo",operandValue(tacline->destination));
			asprintf(&mips,"%s\n%s",div,mflo);
			break;
		}

		case LOAD:{
			if(tacline->operand1 != NULL && tacline->operand1->type == CONSTANT){
				asprintf(&mips,"%s %s, %s","li",operandValue(tacline->destination),operandValue(tacline->operand1));
			} else {
				asprintf(&mips,"%s %s, %s","move",operandValue(tacline->destination),operandValue(tacline->operand1));
			}
			break;
		}

		case FUNC:{
			func_out = solved_sets[line_no]->out;
			//Function Label
			char* label;
			asprintf(&label,"%s %s",operandValue(tacline->destination),":");
			
			
			//Sysbrk the frame (Return Address,Parent FP,framesize,padding, savedregisters, argument registers)
			int number_of_saves = 4;

			for (int i = line_no; i < getTACArraySize(); ++i)
			{
				if(tac_array[i]->operator == CALL){
					if(tac_array[i]->operand2->value > number_of_saves) number_of_saves = tac_array[i]->operand2->value;
				}
				if(tac_array[i]->operator == END){
					break;
				}
			}

			int spilling = 0;

			int frame_size = 4 + 4 + 4 + 4*number_of_saves +4*spilling +(4*4);

			//save return address
			char* retad = "sw $ra 0($sp)";

			//save static link


			//save dynamic link
			char* dynlink = "sw $fp 8($sp)";

			// //save $s registers
			// char* savesaves = "sw $s0 12($sp)";
			// asprintf(&savesaves,"%s\nsw $s1 16($sp)",savesaves);
			// asprintf(&savesaves,"%s\nsw $s2 20($sp)",savesaves);
			// asprintf(&savesaves,"%s\nsw $s3 24($sp)",savesaves);
			// asprintf(&savesaves,"%s\nsw $s4 28($sp)",savesaves);
			// asprintf(&savesaves,"%s\nsw $s5 32($sp)",savesaves);
			// asprintf(&savesaves,"%s\nsw $s6 36($sp)",savesaves);
			// asprintf(&savesaves,"%s\nsw $s7 40($sp)",savesaves);

			//set new fp
			char* newfp = "move $fp $sp";

			//move stack pointer after temps and for 4 args
			char* sptempargs;
			asprintf(&sptempargs,"addi $sp $sp %i",12+(spilling*4)+(4*4));


			asprintf(&mips,"%s\n%s\n%s\n%s\n%s",label,retad,dynlink,newfp,sptempargs);

			break;
		}

		case LABEL:{
			asprintf(&mips,"%s %s",operandValue(tacline->destination),":");
			break;
		}

		case CALL:{
			first_push = 1;
			arg_stack_size = 0;
			//save everything in the out set for the Functions start TAC line

			// int func_start_no = getFunctionStart(tacline->operand1);

			

			// Set* function_kills = getFunctionsKills(tacline->operand1);

			// for (int i = 0; i < function_kills->number_of_elements; ++i)
			// {
			// 	asprintf(&saves,"%s%s %s %i($fp)",saves,"sw",function_kills->elements[i]->lexeme, (i*4)+12);
			// 	if(i<function_kills->number_of_elements-1) asprintf(&saves,"%s\n",saves);
			// }

			char* jal;
			//Added NOP for branch delay slot
			asprintf(&jal,"%s %s\nnop","jal",operandValue(tacline->operand1));

			char* store_ret_value;
			asprintf(&store_ret_value,"move %s $v0",operandValue(tacline->destination));

			char* load_return_address = "lw $ra 0($fp)";

			char* loads = "";
			for (int i = 0; i < solved_sets[line_no]->in->number_of_elements; ++i)
			{
				asprintf(&loads,"%s%s %s %i($fp)",loads,"lw",solved_sets[line_no]->in->elements[i]->lexeme, (i*4)+12);
				if(i<solved_sets[line_no]->in->number_of_elements-1) asprintf(&loads,"%s\n",loads);
			}


			if(strlen(loads)>0){
				asprintf(&mips,"%s\n%s\n%s\n%s",jal,store_ret_value,load_return_address,loads);
			} else {
				asprintf(&mips,"%s\n%s\n%s",jal,store_ret_value,load_return_address);
			}
			
			break;
		}

		case RTRN:{

			//put the return value in v0
			char* retval;
			if(tacline->destination->type == CONSTANT){
				asprintf(&retval,"li $v0 %i",tacline->destination->value);
			} else {
				asprintf(&retval,"move $v0 %s",tacline->destination->lexeme);
			}

			//load back all saved values
			// char* loadsaves = "lw $s0 12($sp)";
			// asprintf(&loadsaves,"%s\nlw $s1 16($sp)",loadsaves);
			// asprintf(&loadsaves,"%s\nlw $s2 20($sp)",loadsaves);
			// asprintf(&loadsaves,"%s\nlw $s3 24($sp)",loadsaves);
			// asprintf(&loadsaves,"%s\nlw $s4 28($sp)",loadsaves);
			// asprintf(&loadsaves,"%s\nlw $s5 32($sp)",loadsaves);
			// asprintf(&loadsaves,"%s\nlw $s6 36($sp)",loadsaves);
			// asprintf(&loadsaves,"%s\nlw $s7 40($sp)",loadsaves);

			//set the stack pointer to the frame pointer
			char* setsp = "move $sp $fp";

			//set the frame pointer to the dynamic link
			char* setfp = "lw $fp 8($fp)";

			//jump back to the return address
			char* jrra = "jr $ra";

			

			asprintf(&mips,"%s\n%s\n%s\n%s",retval,setsp,setfp,jrra);
			break;
		}

		case END:{
			mips = "";
			break;
		}

		case PUSH:{

			char* saves = "";

			if(first_push){
				int call_line_no = line_no;

				for (; tac_array[call_line_no]->operator != CALL; call_line_no++)
				{
				}

				for (int i = 0; i < solved_sets[call_line_no]->in->number_of_elements; ++i)
				{
					asprintf(&saves,"%s%s %s %i($fp)",saves,"sw",solved_sets[call_line_no]->in->elements[i]->lexeme, (i*4)+12);
					if(i<solved_sets[call_line_no]->in->number_of_elements-1) asprintf(&saves,"%s\n",saves);
				}

				first_push = 0;
			}

			if(strlen(saves)>0){
				asprintf(&mips,"%s\n%s",saves,pushArg(tacline->destination));
			} else {
				asprintf(&mips,"%s",pushArg(tacline->destination));
			}
			
			break;
		}

		case POP:{
			mips = "dont print";
			break;
		}

		case IFN:{
			asprintf(&mips,"%s %s, %s, %s","beq",operandValue(tacline->operand1),"$zero",operandValue(tacline->destination));
			break;
		}

		case GT:
		case LT:
		case GTE:
		case LTE:
		case EQ:
		case NE:{
			asprintf(&mips,"%s",booleanOperation(tacline));
			break;
		}

		case GOTO:{
			asprintf(&mips,"%s %s","j",operandValue(tacline->destination));
			break;
		}

		default:{
			mips = "default";
			break;
		}

	}

	
	return mips;
}

void printMips(TACLINE** tacarray, LivenessSets** new_solved){
	solved_sets = new_solved;
	tac_array = tacarray;

	
	printf("\n");
	printf("\n");
	printf("## MIPS ##\n");
	printf("\n");

	char*MIPS="";

	asprintf(&MIPS,"%smain:\n",MIPS);
	asprintf(&MIPS,"%smove $fp $sp\n",MIPS);
	asprintf(&MIPS,"%sjal prog_main\n\n",MIPS);
	asprintf(&MIPS,"%smove $v1 $v0\n",MIPS);
	asprintf(&MIPS,"%sli $v0 10\n",MIPS);
	asprintf(&MIPS,"%ssyscall\n",MIPS);

	for (int i = 0; i < getTACArraySize(); ++i)
	{
		char* mips = TACToMIPS(tac_array[i],i);
		if(strcmp(mips,"dont print")) asprintf(&MIPS,"%s%s\n",MIPS, mips);
	}

	asprintf(&MIPS,"%s\n",MIPS);

	printf("%s", MIPS);

	FILE* fp = fopen("/Users/billywork/Documents/Compilers/mips.asm","w");

	fprintf(fp, "%s",MIPS);
	
    fclose(fp);

}
