#ifndef _interpreter_
#define _interpreter_

int getLeafValue(NODE* ast);
NODE* interpret(NODE* ast);
NODE* interpretWithFrame(NODE* ast, FRAME* frame);
NODE* callInterpreter(FRAME* root_frame, FRAME* current_frame, NODE* tree);

#endif