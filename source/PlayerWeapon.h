#ifndef __PLAYERWEAPON_H__
#define __PLAYERWEAPON_H__

#include "orx.h"
#include "Bullet.h"
#include <sstream>

using namespace std;

#define MAXCHARGE 90
#define NUMBER_OF_PLAYER_WEAPONS 6
#define MAX_WEAPON_RANK 2 //the highest index the weapon rank can be
#define CRYSTALS_TO_RANK 5 //the number of crystals needed to increase the weapon rank
#define UI_CRYSTAL_BAR_START_COOR 296.0 //the x coordinate to put the Crystal Bar at when the player has 0 crystals
#define LENGTH_OF_UI_CRYSTAL_BAR 102 //the length of the ui bar that indicates how many crystals the player has

#define MISSILE_FRAGMENT_SPEED1 300
#define MISSILE_FRAGMENT_SPEED2 500
#define MISSILE_FRAGMENT_SPEED3 700
#define MISSILE_COOLDOWN 50.0

#define SHOTGUN_SPEED1 700
#define SHOTGUN_SPEED2 650
#define SHOTGUN_SPEED3 600

#define CHANGE_NEXT_WEAPON 10
#define CHANGE_PREV_WEAPON -1

#define WAVETHETA (3.14159265/4)
#define WAVERADIUS 75
#define WAVE_SPEED1 300
#define WAVE_SPEED2 400
#define WAVE_SPEED3 500
#define WAVE_SPEED4 600
#define WAVE_SPEED5 700
#define WAVESEPERATION 5
#define WAVECOOLDOWN 30

#define BASEDAMAGEPERSECOND 15.0
#define DAMAGEMULTSTRONG 2
#define DAMAGEMULTPERRANK 1.2

#define LASER_X_OFFSET 32
#define LASER_Y_OFFSET -410
#define LASER_CHARGE_TIME1 10 //the number of frames the first laser charging image remains onscreen before switching
#define LASER_CHARGE_TIME2 10 //the number of frames the second laser charging image remains onscreen before switching

enum player_weapon_index{NO_WEAPON=-1,GATLING_CANNON,LASER_CANNON,LASER_SHOTGUN,MISSILE_LAUNCHER,RED_STAR_BOMB,BACKGUN};

class PlayerWeapon{
public:
	PlayerWeapon();
	~PlayerWeapon();
	void update(orxVECTOR playerPos, bool charging, bool fireFlag); //Updated each frame by the player, charging indicates not firing
	void changeWeapon(int newWeapon); //The int corresponds to each type instead of type, strength, and level        
	float randFloat(int low, int high);
	void saveInventory(); // Saves rank and unlock information so it can be restored on death (loses powerups until last save)
	void resetInventory(); // Resets the player's inventory back to the saved inventory
	void unlockWeapon(int weapon); // Unlock a weapon by identifier (See Weapon Types section below)
	void upgradeWeapon(int ranks); // Upgrades the current weapon by the given number of ranks
	void changeCrystalCount(int crystals); //increases the crystal count by the given number of crystals

	bool chargeAnim;
	int getRank();
	bool isUnlocked(int);
	player_weapon_index getCurrentWeapon();

	orxVECTOR farAwayVector;

private:
	void fire(orxVECTOR playerPos);
	void moveCrystalUI();//moves the bar in the UI that indicates how many crystals the player has

	orxOBJECT* playerWeaponRankText;
	orxOBJECT* playerCrystalTotalText;
	stringstream ss;//used to output the current rank and number of crystals

	int currentWave;

	orxOBJECT* uiCrystalBar;
	//Temporary Variables
	orxVECTOR tempVec;
	Bullet *tempBullet;
	int tempInt;
	
	orxSOUND* shootSound;


	bool dispensing; // Determines if weapon is current unloading the strong attack
	player_weapon_index currentWeapon; // Identifier for what weapon type is being used, does not include rank or strength
	int chargeCounter; // Keeps track of how long the player has been charging his weapon

	bool unlocks[NUMBER_OF_PLAYER_WEAPONS]; // Determines if weapon is unlocked, each weapon has an identifier
	int counter[NUMBER_OF_PLAYER_WEAPONS*6]; // Counters used for determining when the next bullet for a weapon can be fired
	int rank; // Determines the current rank of all weapons. When the counter reaches 0, that weapon is ready to fire
	int currentCrystals; //the current number of crystals the player has collected
	bool savedUnlocks[NUMBER_OF_PLAYER_WEAPONS]; // Saved to be restored if died
	int savedRank; 

};



#endif