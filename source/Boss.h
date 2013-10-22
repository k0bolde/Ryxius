#ifndef __BOSS_H__
#define __BOSS_H__

class Unit;
class Behavior;
class Boss;

#include "orx.h"
#include "Unit.h"
#include "Behavior.h"
#include <vector>

using namespace std;

class Boss{
public:
	Boss();
	Boss(int maxHP);
	~Boss();

	orxSTATUS Update(orxVECTOR playerPos);
	orxSTATUS startInitialBehavior();

	int Hit(Unit* part, int damage);

	void addBossPart(Unit* u , bool invul=false);
	void addBehavior(Behavior* b);

	Unit* getBossPart(int i);

private:
	int hp;

	vector<Unit*> bossParts;

	//the health of each individual boss part
	//all damage to each part is passed up to the total health
	//if a part's health reaches 0 it dies
	//a part's health can be negative, in which case the part cannot die without the whole boss dying
	vector<int> partHealth;
	vector<bool> partInvul;//used to allow individual boss parts to be invulnerable

	int currentBehaviorIndex;
	vector<Behavior*> behaviorSet;

};

#endif