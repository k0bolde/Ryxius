#ifndef __BEHAVIOR_H__
#define __BEHAVIOR_H__

class BehaviorStep;

#include "BehaviorStep.h"
#include <vector>

using namespace std;

class Behavior{
public:
	Behavior();
	~Behavior();

	//resets/reinitializes the behavior
	void readyBehavior();

	bool Update();
	void addBehaviorStep(BehaviorStep* step);
	
	//used to remove the action corresponding the removed boss part
	void removedPart(int index);

private:
	int cycleCount;//the number of clock cycles since the behavior has been begun
	int currentStepIndex;
	vector<BehaviorStep*> steps;

};

#endif