/*
 * A class for defining a sequence of actions.
 */
#ifndef __BEHAVIORSTEP_H__
#define __BEHAVIORSTEP_H__

class Action;

#include "orx.h"
#include "Action.h"
#include <vector>

using namespace std;

class Boss;

class BehaviorStep{
public:
	BehaviorStep();
	BehaviorStep(int sTime, int eTime);
	~BehaviorStep();

	void issueActions();

	int getEndTime();

	void addAction(Action* a);

	//used to remove the action corresponding the removed boss part
	void removedPart(int index);

private:
	//the starting time of the move step relative to the creation of the boss, in update cycles
	int startTime;

	//the time to finish the move step relative to the creation time of the boss, in update cycles
	int endTime;

	//the actions each part should take for this move step
	vector<Action*> actions;

};

#endif