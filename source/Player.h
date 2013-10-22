#ifndef __PLAYER_H__
#define __PLAYER_H__

class Pattern;
class Player;

#include <vector>
#include "orx.h"
#include <string>
#include "Pattern.h"
#include "PlayerWeapon.h"

#define WEAPON_CHANGE_COOLDOWN_TIME 60 //time between when the player can change weapons in frames

//Player Warp Animation Constants
#define PA_INITIALDIST 10.0
#define PA_ANIMSPEED 1
#define PA_MULT 1.3

//Player Charge Animation Constants
#define PC_ANIMSPEED 1

class Player{
	public:
		Player* Instance();
		orxSTATUS Init(std::string);
		orxOBJECT* Player::GetObjectByName( std::string objectName );
		orxOBJECT* Player::getShip();
		~Player();

		void startFullyChargedAnim();
		
		Player(GameSystem* m);
		orxSTATUS setLeft(int);
		orxSTATUS setRight(int);
		orxSTATUS setUp(int);
		orxSTATUS setDown(int);
		orxVECTOR GetPos();
		orxSTATUS fire();
		orxSTATUS Player::SetPos(orxVECTOR temp);
		orxSTATUS Player::SetRot(float rot);
		orxSTATUS Player::SetAngularVelocity(float angVel);
		orxSTATUS Player::Update(bool fireFlag);
		orxSTATUS Player::setFiring(int firing);
		int Player::getFiring();
		orxSTATUS Player::Die();
		int Hit(int damage);
		orxSTATUS Player::setWeapon(int weapon);
		orxSTATUS Player::checkPos(orxOBJECT* p, orxVECTOR v);
		orxSTATUS Player::SetInvuln(bool invuln);
		orxSTATUS Player::heal(int); //for healthpacks
		orxSTATUS Player::restart();
		orxSTATUS Player::clear();
		bool YouAreAGod(string name);

		bool warpAnim;
		bool playerDead;

		void unlockWeapon(int i);//unlocks the weapon of the corresponding index
		void upgradeWeapon(int ranks); // Upgrades the current weapon by the given number of ranks
		void changeCrystalCount(int crystals); //increases the crystal count by the given number of crystals
		void saveInventory(); // Saves players weapons and weapon ranks
		void resetInventory();// Resets the player's weapons and weapon ranks to the saved states

		void disableControls(); //Disable controls and sets the ship speed to 0
		
		//weapon types
		enum {VARUP, ONESHOT, THREESHOT};

		orxOBJECT* ship;
		orxOBJECT* ship2;
		orxOBJECT* ship3;
		orxOBJECT* shipdead;
		orxOBJECT* hitBox;
		orxOBJECT* exp;

		int getWeaponRank();
		bool isWeaponUnlocked(int);
		int getCurrentWeapon();
private:
		void checkChangeWeapon();
		PlayerWeapon *pShoot;
		int hp;
		bool showHitBox;
		bool charging; // Indicates if weapon is charging or firing standardly
		void updateGraphic();
		void setExplosion();
		
		int warpSec;
		int warpCounter;
		bool controlsEnabled;
		vector<orxOBJECT*> warpAnimGhosts;
		orxVECTOR tempVec;
		float currentSpacing;
		int removePlace;
		orxVECTOR animLoc;
		
		int weaponChangeCooldown;

		//used to prevent the player from cycling through weapons by holding down the next and previous weapon buttons
		bool waitReleaseNextWeapon;
		bool waitReleasePrevWeapon;

		GameSystem* m_Instance;

		bool fullyChargedAnim;
		bool reverseAnim;
		orxCOLOR weaponChargeColor;
		orxVECTOR weaponChargeColorVector;
		int chargeAnimCounter;


};

#endif