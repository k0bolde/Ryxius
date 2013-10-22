#ifndef __GAMESYSTEM_H__
#define __GAMESYSTEM_H__

class GameSystem; // keep

#include "orx.h" // We want to interface with ORX, so including it is helpful! :)
#include <string> // Our 'GetObjectByName' and 'Update' functions both use strings for the comparison tests.
#include "Unit.h" //holds our units
#include "Boss.h"
#include "Player.h" //Reference to player
#include "Parser.h"
#include "LevelSystem.h"
#include "Powerup.h"

#define SCREEN_MAX_X 500
#define SCREEN_MAX_Y 350
#define SCREEN_MIN_X -500
#define SCREEN_MIN_Y -350
#define MAXNAMELENGTH 10
#define POWERUP_PROBABILITY 25 //the probability (as a percent) that a powerup will spawn when an enemy dies
#define CRYSTALS_LOST_ON_HIT -4
#define POINTS_LOST_ON_HIT 100000 //the number of points the player loses when they are hit

enum physics_object_type{UNIT,PLAYER,BULLET,LASER,POWERUP,INVULN};
static LevelSystem* lvlSys;
static orxVIEWPORT* view;

static bool endOfLevel;


struct ObjectInfo{
	physics_object_type type;
	void * object;
};

class GameSystem
{
	
public:
	
	// Instance deals with ensuring we only have one copy of our GameSystem class.
	static GameSystem* Instance();

	static bool getFireFlag();
	static bool getPlayerDeathFlag();
	static void restartPlayer(); // Used everytime a level is load/reloaded
 
	// Init function sets up our default/first scene.
	static orxSTATUS orxFASTCALL Init();

	static Player* getPlayerStatic();
 
	// Run and Exit are both empty for now.
	static orxSTATUS orxFASTCALL Run();
	static void orxFASTCALL Exit();
 
	// GetObjectByName compares the names of the objects loaded in memory, with a string we pass in
	//   either returning the first matching object, or a null pointer.
	static orxOBJECT* orxFASTCALL GetObjectByName( std::string objectName );
	void removeUnit(Unit* u);
	void removePowerup(Powerup* p);

	Player* getPlayer();
 
	// Update is called on each 'tick' of a clock.
	static void orxFASTCALL Update( const orxCLOCK_INFO* clockInfo, void* context );
 
	// We currently use EventHandler to simply log when some of events happen, so we know they're working.
	static orxSTATUS orxFASTCALL EventHandler( const orxEVENT* currentEvent );
	// GetMouseWorldPosition gets the mouse 'screen' position, converts that to 'world'
	//   coordinates, and returns the vector.
	static orxVECTOR orxFASTCALL GetMouseWorldPosition();
	orxSTATUS Setup();
	void addUnit(Unit* u);
	void clearUnits();
	void clearPowerups();
	static void nameEntrySequence();
	static void startGame();

	void setFireFlag(bool b);
	void GameSystem::clearBullets();
	
	//returns true if an object at the given position and is a circle of radius size is off the screen
	bool isOffScreen(orxVECTOR* pos, float size);
	
	void spawnRandomPowerup(orxVECTOR pos, int type=-1, int v=-1);
	
	//creates the missile fragments when a missile hits an enemy
	void missileExplode(Bullet* missile, int useNumber);
	
	void savePlayerInventory(); // See Player.h

	void GameSystem::clearPlayer();
	bool getWeaponUnlock(int);
	void GameSystem::escapeGame();
	void GameSystem::completeGame();
	void GameSystem::goToScoreScreen();
	
protected:
	GameSystem();
	GameSystem(const GameSystem&);
	GameSystem& operator= (const GameSystem&);
	void GameSystem::clearGame();
	

private:
	static Boss* boss;
	static vector<Powerup*> powerups;

	static GameSystem* m_Instance;
	void HandleCollision(ObjectInfo* obj1, ObjectInfo* obj2);
	void UnitHit(Unit* unit,double damage);//handles dealing damage to a Unit
	static void spawnTestBoss();

	//temp var for update loop
	static int tempI;

	
	
};
 
#endif // __GameSystem_H__
