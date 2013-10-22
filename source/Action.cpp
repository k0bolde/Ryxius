#include "Action.h"

#include <iostream>

Action::Action(){
	//the boss part is deleted by the boss

	delete movePosition;
}
Action::Action(Unit* u, orxVECTOR* mp, int pn){
	bossPart=u;
	movePosition=mp;
	patternId=pn;
}
Action::~Action(){
	delete movePosition;
}

void Action::takeAction(){
	//handle action requires time to be in seconds
	bossPart->handleAction(movePosition, actionTime/60.0, patternId);
}

void Action::setActionTime(int aTime){
	actionTime=aTime;
}

int Action::getActionTime(){
	return actionTime;
}

orxVECTOR* Action::getMovePosition(){
	return movePosition;
}
int Action::getPatternId(){
	return patternId;
}