#ifndef __UNIT_H__
#define __UNIT_H__


class Pattern;
class Unit;

class Boss;
struct pathInfo;
#include "orx.h" // We want to interface with ORX, so including it is helpful! :)
//#include "Boss.h"
#include <string> // Our 'GetObjectByName' and 'Update' functions both use strings for the comparison tests.
#include "Pattern.h"
#include "LevelSystem.h"

class Unit
{
public:
	~Unit();

	 orxVECTOR GetPos();
	//update enemy
	 orxSTATUS Update(orxVECTOR playerPos);
 
	// Init function for each new enemy you want
	orxSTATUS Init(std::string);

	orxSTATUS SetSpeed(orxVECTOR speed);
	void Unit::removeBullet(orxOBJECT* u);

	int getScoreValue();

	orxSTATUS SetPos(orxVECTOR pos);
	orxOBJECT* Unit::GetObjectByName( std::string objectName );
	orxVECTOR Unit::GetSpeed();
	void addBullet(Pattern* u);
	orxSTATUS Unit::DeleteUnit();
	double Hit(double damage);
	double getHealth();//returns the unit's current health
	int getPartIndex();
	int getWeaponUnlocked();

	// We currently use EventHandler to simply log when some of events happen, so we know they're working.
	static orxSTATUS orxFASTCALL EventHandler( const orxEVENT* currentEvent );
 
	void handleAction(orxVECTOR* movePosition, int actionTime, int patternId);

	//For pathing
	Unit(orxVECTOR pos, orxSTRING enemyName, double hp, int score, int weapUnlock=-1, Boss* b=NULL, int pIndex=-1);
	//void Unit::SetPath(orxVECTOR* positions, int* times, int numPoints);
	void Unit::SetPath(pathInfo* p);
	void test();

	//Holds Enemies
	//Handles firing weapons and movement every update -- called every clock

	//on-screen points text
	orxOBJECT* spawnPointObj(int);
private:
	//we store the enemy speed, angle, and position
	orxVECTOR angle;
	orxVECTOR speed;
	orxVECTOR pos;
	orxOBJECT* enemy;
	orxSTRING enemyName;
	int life;
	int totalTime;
	int lastBullet;
	Pattern *shoot;
	double hp;
	orxVECTOR target;
	bool moveToTarget;
	float targetSpeed;
	int scoreValue; // How many points the enemy is worth when destroyed
	//Variables for invulnerable flashing
	int flash;
	orxCOLOR old;
	orxVECTOR w;
	orxVECTOR r;

	//various variables for pathing code
	orxVECTOR* path;
	int* endTimes;
	int numPoints;
	bool hasPath;
	int currPath;
 
	//temp vars
	orxVECTOR angleT;
	orxVECTOR t;
	orxVECTOR circle;
	orxVECTOR stop;
	float dist;

	//if this unit drops a weapon unlock powerup this will determine which one
	//this will be negative if no weapon unlock powerup should be dropped
	int weaponUnlocked;

	Boss* boss;//if the unit is a boss part it will have a pointer to the boss it is a part of
	int partIndex;//if the unit is a boss part this is the index that this unit corresponds to in the boss's part vector
};
 
#endif // __Unit_H__