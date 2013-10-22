#include "Behavior.h"

#include <iostream>

Behavior::Behavior(){
	cycleCount=0;
	currentStepIndex=0;
}

Behavior::~Behavior(){
	for(int i=0; i<steps.size() ; i++){
		if(steps.at(i)!=NULL){
			delete steps.at(i);
		}
	}
	steps.clear();
}

void Behavior::readyBehavior(){
	cycleCount=0;
	currentStepIndex=0;
	steps[currentStepIndex]->issueActions();
}

//returns true if move is not done
//returns false if the move completes after this call.
bool Behavior::Update(){
	cycleCount++;

	if(cycleCount>=steps[currentStepIndex]->getEndTime()){
		currentStepIndex++;

		if(currentStepIndex<steps.size()){
			steps[currentStepIndex]->issueActions();
		}
		else{
			return false;
		}
	}

	return true;
}

/*
 * Adds a new behavior step to the end of this behavior.
 */
void Behavior::addBehaviorStep(BehaviorStep* step){
	steps.push_back(step);
}

void Behavior::removedPart(int index){
	for(int i=0; i<steps.size(); i++){
		steps.at(i)->removedPart(i);
	}
}