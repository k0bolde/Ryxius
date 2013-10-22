#ifndef __POWERUP_H__
#define __POWERUP_H__

#include "orx.h"

#define POWERUP_SIZE 26
#define NUM_POWERUP_TYPES 2 //how many different kinds of powerup there are
#define CRYSTAL_POINT_VAL 150
#define WEAPON_UNLOCK_POINT_VAL 5000
#define WEAPON_RANK_UP_POINT_VAL 2000

//do not explicitly assign the numeric values for this enum
//the method for randomly generating a powerup relies on the default behavior
enum powerup_type{WEAPON_UNLOCK,WEAPON_RANK_UP,CRYSTAL,HEALTH};

class Powerup{
public:
	Powerup();
	Powerup(powerup_type t, int v, orxVECTOR pos, orxVECTOR speed);
	~Powerup();

	orxSTATUS DeletePowerup();

	powerup_type getType();
	int getValue();
	orxVECTOR* getPosition();
	orxOBJECT* spawnPointObj(int);

private:
	//the type refers to what kind of powerup it is
	powerup_type type;

	//for a weapon change powerup the value is the number of the weapon to change to
	//for a health powerup the value is how many hit points the powerup heals
	int value;

	orxOBJECT* powerup;
	orxVECTOR position;
};

#endif