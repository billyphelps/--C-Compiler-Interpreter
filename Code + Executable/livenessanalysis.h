#ifndef _liveness_
#define _liveness_

#include "Set.h"
#include "TAC.h"
#include "graphcoloring.h"

typedef struct LivenessSets{
	Set* kill;
	Set* gen;
	struct LivenessSets** succ;
	Set* in;
	Set* old_in;
	Set* out;
	Set* old_out;
} LivenessSets;

Set* createGenSet(TACLINE* tacline);
Set* createKillSet(TACLINE* tacline);
void setInOutEmpty(LivenessSets* lSets);
void setAllInOutEmpty(LivenessSets** ListOflSets);
Set* unionOfSuccessorIns(LivenessSets* lSets);
void updateLivenessSets(LivenessSets* lSets);
void updateAllLivenessSets(LivenessSets** ListOflSets);
int testConvergence(LivenessSets** ListOflSets);
LivenessSets* createLivenessSetsFromTAC(TACLINE* tacline);
LivenessSets** createListOfLSets(TACLINE** TACarray);
LivenessSets** setSuccesors(LivenessSets** ListOflSets);
LivenessSets** solveDataFlowEquations(LivenessSets** ListOflSets);
GraphEdge** interferingVariables(Set* kill, Set* out);
Graph* conflictingVariables(LivenessSets** ListOflSets);
void printAllKillOuts(LivenessSets** ListOflSets);

void printLivenessSets(LivenessSets* lSets);
void printAllLivenessSets(LivenessSets** ListOflSets);

#endif