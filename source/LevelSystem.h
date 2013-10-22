#ifndef __LEVELSYSTEM_H__
#define __LEVELSYSTEM_H__

class LevelSystem; // keep
struct MapEvent; // keep

class GameSystem;

#include <vector>
#include "GameSystem.h"
#include "PlayerWeapon.h"
#include <sstream>

#define ENEMIES_DESTROYED_TEXT "Enemies Destroyed: "
#define POINTS_EARNED_TEXT "Credits Acquired: "
#define TOTAL_POINTS_TEXT "Total Credits: "

#define ENEMY_POINT_VAL 100 //point value of enemies

using namespace std;

struct pathInfo{
	orxVECTOR* path;
	int* times;
	int numPoints;
};

struct MapEvent{
	short command;
	int time;
	int iValue1;
	int iValue2;
	int iValue3;
	int iValue4;
	string sValue1;
	pathInfo* sValue2;
	player_weapon_index weapUnlock;
};




class LevelSystem{
public:
	LevelSystem(GameSystem *m);
	~LevelSystem();
	void update();
	void restartLevel();
	void loadLevel(vector<MapEvent>* mpEV, vector<string>* mpE);
	void endLevel(); // Completes the level, not restart
	void displayEndLevelText();//displys all the end statistics for the level
	void deleteEndLevelText();//deletes all the end statistics text when going to the next level
	void highLoadLevel(); // Contains all the information for all levels and sends information on the current level to loadLevel()
	void modScore(int mod); // Used for changing score from GameSystem
	void enemyKilled();//tells the level system an enemy was killed
	void goToNextLevel();//tells the level system to go to the next level
	bool waitingForNextLevel();//returns true if the game is waiting for the player to signal to go to the next level

	//changeLevel(vector<MapEvent> mpEV, vector<string> mpE);
	static const short CHANGEMAPSPEED = 0;
	static const short CREATEENEMY = 1;
	void LevelSystem::Bezier2D(double* b, int npts, int cpts, double* p);
	pathInfo* LevelSystem::genBezier(double* p, int numPoints, int numReturnPoints);
	pathInfo* generatePath(double* p, int numPoints);
	pathInfo* LevelSystem::genPathConstTime(double* p,int numPoints, int time);
	pathInfo* LevelSystem::genSwarm(int constTime, bool goingRight);
	void LevelSystem::addJitter(double* p, int numPairs, double jitter);
	pathInfo* LevelSystem::genMantisRush(int x, int constTime);
	double LevelSystem::randFloat(int start, int end);
	pathInfo* LevelSystem::genSnake(bool right, int constTime);
	pathInfo* LevelSystem::genRand(int numPoints, int constTime);
	pathInfo* LevelSystem::genBezier(double* p, int numPoints, int numReturnPoints, int constTime);

	//For pathing
	pathInfo* LevelSystem::genCucumbers(double* p, int length, int newPathLength);
	orxVECTOR LevelSystem::whereToGoBro(int currentTime, int movementType);
	orxVECTOR LevelSystem::center(orxVECTOR* a,orxVECTOR* b,orxVECTOR* c);
	void LevelSystem::setList(orxVECTOR listOfPoints[]);
	double LevelSystem::len(orxVECTOR* a, orxVECTOR* b);
	double LevelSystem::ang(orxVECTOR* a, orxVECTOR* b, orxVECTOR* c);
	bool LevelSystem::line(orxVECTOR* a,orxVECTOR* b,orxVECTOR* c);
	double LevelSystem::slope(orxVECTOR* a,orxVECTOR* b);
	void LevelSystem::updateMultText(string);
	void LevelSystem::setRankBadge(int);	
	pathInfo* LevelSystem::genInvaders(int xPos, int yOffset);
	pathInfo* LevelSystem::genSweep(bool right);
	int scoreAtStart;
	string LevelSystem::getLevelName();
	void setHotbar(int);

private:
	bool mapLoaded;
	bool emptyMap;
	int currentLevel;
	int currentFrame;
	vector<MapEvent>* mapEvents; //Events vector
	vector<orxOBJECT*>* mapElements; //Background elements
	orxOBJECT* leading; //Leading background
	orxOBJECT* trailing;//Trailing background
	int currentBackground; //Keeps track of background scrolling position
	int mapSpeed; // Current speed of map
	orxVECTOR* offscreen; // Where do background elements go to?
	int eventPosition; // What event am I checking time for?
	bool EventsCompleted; // When all elements are completed...
	GameSystem* m_Instance;

	/*Movement variables start*/
	orxVECTOR* list;
	int current;
	/*Movement variables end*/
	
	int hiscore; // SCORE
	int enemiesCreated;//stores the number of enemies created in the level
	int enemiesKilled;
	orxOBJECT* endLevelHeaderObj;
	orxOBJECT* endLevelEnemiesDestroyedObj;
	orxOBJECT* endLevelScoreEarnedObj;
	orxOBJECT* endLevelTotalScoreObj;
	orxOBJECT* endLevelContinueTextObj;

	bool waitNextLevelFlag;//true if the game should be waiting for player input to go to the next level

	stringstream ss;//used to output the current score
	vector<orxOBJECT*> hotbar;
	void handleRank();

	//hotbar variables
	orxOBJECT* bronzeback;
	orxOBJECT* silverback;
	orxOBJECT* goldback;
	orxOBJECT* currentback;
};

#endif
