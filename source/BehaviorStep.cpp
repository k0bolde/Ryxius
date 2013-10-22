#include "BehaviorStep.h"

#include <iostream>

BehaviorStep::BehaviorStep(){
}
BehaviorStep::BehaviorStep(int sTime, int eTime){
	startTime=sTime;
	endTime=eTime;
}
BehaviorStep::~BehaviorStep(){
	for(int i=0;i<actions.size();i++){
		if(actions.at(i)!=NULL){
			delete actions.at(i);
		}
	}
	actions.clear();
}

void BehaviorStep::issueActions(){
	for(int i=0;i<actions.size();i++){
		actions.at(i)->takeAction();
	}
}

int BehaviorStep::getEndTime(){
	return endTime;
}

/*
 * Adds a new action to this BehaviorStep
 */
void BehaviorStep::addAction(Action* a){
	a->setActionTime(endTime-startTime);
	actions.push_back(a);
}

void BehaviorStep::removedPart(int index){
	for(int i=0; i<actions.size(); i++){
		delete actions.at(i);
		actions.at(i)=NULL;
	}
}