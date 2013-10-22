#include "Boss.h"

#include <iostream>

Boss::Boss(){
	currentBehaviorIndex=0;
}

Boss::Boss(int maxHP){
	currentBehaviorIndex=0;
	hp=maxHP;
}

Boss::~Boss(){
	for(int i=0; i<bossParts.size();i++){
		if(bossParts.at(i)!=NULL){
			delete bossParts.at(i);
		}
	}
	bossParts.clear();

	for(int j=0; j<behaviorSet.size() ; j++){
		if(behaviorSet.at(j)!=NULL){
			delete behaviorSet.at(j);
		}
	}
	behaviorSet.clear();
}

orxSTATUS Boss::startInitialBehavior(){
	behaviorSet[currentBehaviorIndex]->readyBehavior();
	return orxSTATUS_SUCCESS;
}

orxSTATUS Boss::Update(orxVECTOR playerPos){

	for(int i=0;i<bossParts.size();i++){
		if(bossParts.at(i)!=NULL){
			bossParts.at(i)->Update(playerPos);
		}
	}

	if(behaviorSet[currentBehaviorIndex]->Update()==false){
		//goes to the next behavior and readies it
		currentBehaviorIndex=(currentBehaviorIndex+1)%behaviorSet.size();
		
		behaviorSet[currentBehaviorIndex]->readyBehavior();
	}
	return orxSTATUS_SUCCESS;
}

/*returns 1 if alive, 0 if dead*/
int Boss::Hit(Unit* part, int damage){
	int partIndex=part->getPartIndex();
	//truncate the damage if the part that was hit has less health than the damage dealt
	if(partHealth[partIndex]>=0 && partHealth[partIndex]<damage){
		damage=partHealth[partIndex];
	}
	if(partHealth[partIndex]>0){
		partHealth[partIndex]-=damage;
		if(partHealth[partIndex]==0){
			delete bossParts.at(partIndex);//TODO let boss parts do cool stuff on death
			bossParts.at(partIndex)=NULL;
		}
	}
	hp-=damage;
	if(hp<=0){
		return 0;
	}

	return 1;
}

//u is the boss part to be added
//invuln is used to set whether the part should start out invulnerable or not
void Boss::addBossPart(Unit* u , bool invul){
	bossParts.push_back(u);
	partHealth.push_back(u->getHealth());
	partInvul.push_back(invul);
}

void Boss::addBehavior(Behavior* b){
	behaviorSet.push_back(b);
}

Unit* Boss::getBossPart(int i){
	return bossParts.at(i);
}