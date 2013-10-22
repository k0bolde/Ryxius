#ifndef __ACTION_H__
#define __ACTION_H__

class Unit;

#include "orx.h"
#include "Unit.h"
#include <string>

using namespace std;

class Action{
public:
	Action();
	Action(Unit* u, orxVECTOR* mp, int pn);
	~Action();

	void takeAction();

	void setActionTime(int aTime);
	int getActionTime();
	orxVECTOR* getMovePosition();
	int getPatternId();

private:
	orxVECTOR* movePosition;
	
	//time to perform the action in clock cycles
	int actionTime;

	//int to specify the weapon to fire, 0 to stop firing
	int patternId;

	//the part of the boss this action controls
	Unit* bossPart;
};

#endif