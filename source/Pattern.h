#ifndef __Pattern_H__
#define __Pattern_H__

class GameSystem;
class Pattern;

#include "orx.h" // We want to interface with ORX, so including it is helpful! :)
#include <string> // Our 'GetObjectByName' and 'Update' functions both use strings for the comparison tests.
#include "GameSystem.h"
 

class Pattern
{
public:
	
	orxSTATUS Pattern::varDown(orxVECTOR pos, int r);
	orxSTATUS Pattern::threeDown(orxVECTOR pos, int r);
	orxSTATUS Pattern::pVarUp(orxVECTOR pos);
	orxSTATUS Pattern::ThreeBulletPattern(int wState, orxVECTOR pos, int rot);
	orxSTATUS Pattern::setPat(int name);
	orxSTATUS Pattern::fire(orxVECTOR pos,orxVECTOR playerPos);
	orxSTATUS Pattern::fire(orxVECTOR pos,orxVECTOR playerPos, int time);
public:
	Pattern();
 
private:
	int pat;
	
	
};
 
#endif // __Pattern_H__