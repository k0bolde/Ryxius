#include "Unit.h"
#include <math.h>
#include "Pattern.h"
//#include "GameSystem.h"
#include "Bullet.h"
#include "Boss.h"
#include "LevelSystem.h"
#include <iostream>
using namespace std;

Unit::~Unit(){
	DeleteUnit();
	delete shoot;
}

Unit::Unit(orxVECTOR posA, orxSTRING enemyNum, double hpTemp, int score, int weapUnlock, Boss* b,int pIndex){
	shoot = new Pattern();
	boss=b; // If nothing is specified in the constructor, this becomes NULL
	partIndex=pIndex;//If nothing is specified in the constructor, this becomes -1
	weaponUnlocked=weapUnlock;//If nothing is specified in the constructor, this becomes -1
	scoreValue = score;
	flash = 0;

	pos = posA; // This is the initial position for the unit, this ideally should always be off screen
	hp = hpTemp;

	moveToTarget = false;

	enemyName = enemyNum;
	enemy = orxObject_CreateFromConfig( enemyNum ); //Creates the enemy specified, this assumes that the .ini is already loaded

	w.fR = 255;
	w.fG = 255;
	w.fB = 255;
	r.fR = 255;
	r.fG = 0;
	r.fB = 0;
	old.vRGB = w;
	old.fAlpha = 1;
	orxObject_SetColor(enemy, &old);

	//The following is for getting this object through collisions
	ObjectInfo *enemyObj= new ObjectInfo();
	enemyObj->type=UNIT;
	enemyObj->object=this;
	orxObject_SetUserData(enemy,enemyObj);

	//put it where we're told to put it
	SetPos(pos);

	angle.fX=0; angle.fY=0; angle.fZ=0;
	speed.fX=0; speed.fY=0; speed.fZ=0;
	pos.fX=0; pos.fY=0; pos.fZ=0;
	life=0;
	lastBullet = -1;
	hasPath = false;
	path = 0;
	stop.fX = 0;
	stop.fY = 0;
	stop.fZ = 0;
	circle.fX =0;
	circle.fY=0;
	circle.fZ=0;
	totalTime = 0;
	// The following are pre-defined units that when specified in enemyNum take on unique
	// properties, such as grahpic, movement, weapon, ect.
	if(strcmp(enemyName,"b2")==0){
		shoot->setPat(16);
	}else if(strcmp(enemyNum,"down")==0){
		orxVECTOR asdf = {0,200,0};
		SetSpeed(asdf);
		shoot->setPat(15);
	}else if(strcmp(enemyName,"side")==0){
		orxVECTOR asdf = {0,0,0};
		GetPos();
		if(pos.fX>0){
			asdf.fX=-200;
		}else{
			asdf.fX=200;
		}
		asdf.fY=200;
		SetSpeed(asdf);
		shoot->setPat(12);
	}else if(strcmp(enemyName,"stream")==0){
		/*orxVECTOR start = {(rand()%300)+200,-375,0};
		if(rand()%2==1){
			start.fX=-start.fX;
		}
		SetPos(start);*/
		orxVECTOR start = {0,0,0};
		start.fY = 200;
		start.fX = 0;
		SetSpeed(start);
		shoot->setPat(17); //11 for stream
	}else if(strcmp(enemyName,"topHoard")==0){
		shoot->setPat(10);
		orxVECTOR asdf;
		asdf.fX = 500 - rand()%1000;
		asdf.fY = -400;// -150 - rand()%200;
		asdf.fZ = 0;
		SetPos(asdf);
		//cout<<asdf.fX<<", "<<asdf.fY<<endl;


		asdf.fX = -100;
		if(rand()%2==0) asdf.fX*=-1;
		asdf.fY = 80;
		if(rand()%2==0) asdf.fY*=-1;
		SetSpeed(asdf);
		//cout<<asdf.fX<<", "<<asdf.fY<<endl;
	}else if(strcmp(enemyName,"stag")==0){
		shoot->setPat(13);
	}else if(strcmp(enemyName,"hope")==0){
		shoot->setPat(13);
	}else if(strcmp(enemyName,"peace")==0){
		shoot->setPat(15);
	}else if(strcmp(enemyName,"love")==0){
		shoot->setPat(13);
		//give it a random y location
		orxVECTOR asdf;
		asdf.fY = -1*(rand()%350);
		asdf.fX = 550;
		if(rand()%2==0) asdf.fX*=-1;
		asdf.fZ = 0;
		SetPos(asdf);
		int dp;
		if(asdf.fX>0){
			dp = rand()%450;
			dp*=-1;
			asdf.fX = -100;
		}else{
			dp = rand()%450;
			asdf.fX = 100;
		}
		lastBullet = dp;
		asdf.fY = 0;
		//set the speed
		SetSpeed(asdf);
	}else if(strcmp(enemyName,"hopeElite")==0){
		shoot->setPat(18);
		orxVECTOR asdf = {-200,0,0};
		orxVECTOR qwer = {550,-200,0};
		SetPos(qwer);
		SetSpeed(asdf);
	}
}

//follow a set of vectors at set times, give times in how long a single move should take, np is number of points
/*void Unit::SetPath(orxVECTOR p[], int t[], int np){
	hasPath = true;
	endTimes = t;
	numPoints = np;
	path = p;
	currPath = 0;
	handleAction(&path[currPath],endTimes[currPath]/60,0); //pointer scares me here, divide by 60 so we get time in frames
}*/

//follow a set of vectors at set times, give times in how long a single move should take, np is number of points
void Unit::SetPath(pathInfo* p){
	hasPath = true;
	endTimes = p->times;
	numPoints = p->numPoints;
	path = p->path;
	//cout<<"Got "<<numPoints<<endl;
	currPath = 0;
	//cout<<"first path x "<<path[0].fX<<" y "<<path[0].fY<<" time "<<endTimes[0]<<endl;
	handleAction(&path[currPath],endTimes[currPath],0); //pointer scares me here, divide by 60 so we get time in frames
	delete p;
}

int Unit::getScoreValue(){
	return scoreValue;
}

orxSTATUS Unit::SetSpeed(orxVECTOR speedA){
	speed = speedA;
	orxObject_SetSpeed(enemy,&speed);

	return orxSTATUS_SUCCESS;
}

orxVECTOR Unit::GetSpeed(){
	orxVECTOR tempSpeed;
	orxObject_GetSpeed(enemy,&tempSpeed);
	return tempSpeed;
}


orxVECTOR Unit::GetPos(){
	//pos = posA;
	orxVECTOR temp;
	orxObject_GetPosition(enemy,&temp);
	pos = temp;
	return temp;
}

orxSTATUS Unit::SetPos(orxVECTOR posA){
	pos = posA;
	orxObject_SetPosition(enemy,&pos);

	return orxSTATUS_SUCCESS;
}

void Unit::test(){
	orxObject_Delete(enemy);
}

//update our enemy based on the command
//we don't take commands, just told to update. Handle enemy logic here based on enemyname.

//enemy ideas - back and forth across the top of the screen relaly fast
orxSTATUS Unit::Update(orxVECTOR playerPos){
	totalTime++;
	life++;
	//Testing target code
	/*	if(life%61==0&&!moveToTarget){ //send it somewhere after 1 second and its not already moving there
	orxVECTOR test = {200,200,0};
	handleAction(&test,2,0); //give it 1 second to get there

	}*/


	//flash if hit
	if(flash > 0){
		flash--;
		orxObject_GetColor(enemy,&old); //Get old color from enemy
		//Flashing
		if(old.vRGB.fG == 255){
			//decrease alpha level if alpha > 1
			old.vRGB = r;
		}else{
			//increase alpha level if alpha < .3
			old.vRGB = w;
		}
		orxObject_SetColor(enemy,&old);	
	}
	else {
		orxObject_GetColor(enemy,&old);
		old.vRGB = w;
		orxObject_SetColor(enemy,&old);	
	}


	if(hasPath){
		//we stored the last path number, so change path target to next thing if life >= time[currPath] & currPath++
		if(life>=endTimes[currPath]){//&&!moveToTarget){ //move to next path
			//cout<<"Life is: "<<life<<" EndTime is: "<<endTimes[currPath]<<endl;
			if(currPath+1<numPoints){ //if there are more points
				currPath++;
				//cout<<currPath<<": Moving to "<<path[currPath].fX<<", "<<path[currPath].fY<<endl;
				handleAction(&path[currPath],endTimes[currPath],0); //pointer scares me here, divide by 60 so we get time in frames
			}else{ //we ran out of points
				hasPath = false;
			}
		}
	}
	
	if(moveToTarget){//&&life%2==0){ //move to the target
		GetPos(); //set pos to be current
		orxVector_Sub(&angleT,&target,&pos); //set angle to be the vector between target and current position
		dist = orxVector_GetDistance(&pos,&target);//---------------------------------------------------safe
		if(dist<5){ //end if we are near the target
			moveToTarget = false;
		}
		orxVector_Normalize(&angleT,&angleT);
		if(hasPath){
			orxVector_Mulf(&angleT,&angleT,targetSpeed*endTimes[currPath]); //get rid of 8 for bosses
		}else{
			orxVector_Mulf(&angleT,&angleT,targetSpeed);
		}
		SetSpeed(angleT);
		
		//handle enemies that fire while moving along the path
		if(strcmp(enemyName,"stag")==0){
			if(totalTime%(rand()%1000+120)==0){
				shoot->fire(pos,playerPos);
			}
		}
		if(strcmp(enemyName,"stop")==0||strcmp(enemyName,"hope")==0||strcmp(enemyName,"peace")==0){
			//SetSpeed(stop);
			if(totalTime%(rand()%120+60)==0){
				shoot->fire(GetPos(),playerPos);
			}
		}
		if(strcmp(enemyName,"hopeElite")==0){
			//SetSpeed(stop);
			if(totalTime%240>200&&totalTime%2==0){
				for(int i =0; i < 5; i +=1){
					shoot->fire(GetPos(),playerPos);
				}
			}
		}
		
		//moveToTarget = false;
	}else{
		if(strcmp(enemyName,"BasicDynamic")==0){
			/*	if(life%10==0){
			orxVECTOR t; //shoots a constant stream of bullets at player - pretty cool

			Bullet *temp;
			t = pos;
			orxVECTOR angle;

			temp = new Bullet(t,"blue");
			//angle = playerPos - t;
			playerPos.fX+=32;
			playerPos.fY+=32;
			orxVector_Sub(&angle,&playerPos,&t);
			orxVector_Normalize(&angle,&angle);
			orxVector_Mulf(&angle,&angle,200);
			temp->SetSpeed(angle);
			}*/

			//orxVECTOR t; //-----------------------------------------------------safe

			//Bullet *temp;
			t = pos;
			//orxVECTOR angle;//--------------------------------------------------safe

			//temp = new Bullet(t,"blue");
			//angle = playerPos - t;
			playerPos.fX+=32;
			playerPos.fY+=32;
			orxVector_Sub(&angleT,&playerPos,&t);
			orxVector_Normalize(&angleT,&angleT);
			orxVector_Mulf(&angleT,&angleT,rand()%60+90);
			SetSpeed(angleT);
		}
		if(strcmp(enemyName,"down")==0){
			//do down stuff
			if(life%40==0){
				//shoot a bullet
				shoot->fire(GetPos(),playerPos);
			}
		}
		
		if(strcmp(enemyName,"side")==0){
			//do down stuff
			if(life%61==0){
				//shoot a bullet
				shoot->fire(GetPos(),playerPos);
			}
		}
		//spiral, turn around to aim
		if(strcmp(enemyName,"stream")==0){ //start at the top, move down, set speed to 0
			if(life>30){ //move down
				//orxVECTOR stop = {0,0,0}; //------------------------------------------------unsafe
				SetSpeed(stop);
				/*if(life%19==0){
				shoot->fire(GetPos(),playerPos);
				}*/
			}

			//every 100 ticks, turn spamming on
			if(life%500 < 240){
				if(life%3==0)
				shoot->fire(GetPos(),playerPos,life);

			}
		}
		if(strcmp(enemyName,"b2")==0){
			//do b2 stuff
			circle.fY = 200; //constant down speed
			//how to tell if its hit the left or right limits?
			if(life%80<40){
				circle.fX = 100;
			}else{
				circle.fX = -100;
			}
			SetSpeed(circle);
			
			if(life%60==0){ //maybe 60?
				//shoot a bullet
				shoot->fire(GetPos(),playerPos);
			}


		}

		if(strcmp(enemyName,"stop")==0||strcmp(enemyName,"hope")==0||strcmp(enemyName,"peace")==0){
			SetSpeed(stop);
			if(totalTime%(rand()%120+60)==0){
				shoot->fire(GetPos(),playerPos);
			}
		}
		/*if(strcmp(enemyName,"peace")==0){
			if(totalTime%(rand()%60+60)==0){
				shoot->fire(GetPos(),playerPos);
			}
		}*/
		if(strcmp(enemyName,"hopeElite")==0){
			orxVECTOR asdf = GetSpeed();
			GetPos();
			if(pos.fX < -320){
				asdf.fX = 200;
			}else if(pos.fX > 320){
				asdf.fX = -200;
			}
			SetSpeed(asdf);
			if(totalTime%240>120){
				shoot->fire(GetPos(),playerPos);
			}
		}

		if(strcmp(enemyName,"love")==0){
			if(totalTime%(rand()%120+60)==0){
				shoot->fire(GetPos(),playerPos);
			}
			GetPos();
			if(pos.fX>lastBullet-5&&pos.fX<lastBullet+5){ //if we are close to our target x
				orxVECTOR asdf = {0,400,0};
				SetSpeed(asdf);
			}
		}

		if(strcmp(enemyName,"topHoard")==0){
			//spawner must set speed to go somewhere
			GetPos();
			orxVECTOR nub = GetSpeed();
			bool mod = false;
			if(pos.fX > 500){ nub.fX =-100; mod = true;}
			if(pos.fX < -500){ nub.fX = rand()%100+60; mod = true; }
			if(pos.fY > -150){ nub.fY = -80; mod = true;}
			if(pos.fY < -350){ nub.fY = rand()%80+40; mod=true;}
			//cout<<nub.fX<<", "<<nub.fY<<endl;
			if(life%(rand()%1000+1)==0){
				shoot->fire(pos,playerPos);
			}
			 SetSpeed(nub);
		}
		if(strcmp(enemyName,"stag")==0){
			if(life%120==0){
				shoot->fire(pos,playerPos);
			}
		}
		//make bug swarms - just fire here. make pathing in levelsystem.
	}
	
	return orxSTATUS_SUCCESS;
}


orxSTATUS Unit::DeleteUnit(){
	orxObject_Delete(enemy);
	if(path){
		free(endTimes);
		free(path);
	}
	return orxSTATUS_SUCCESS;
}

/*returns 1 if alive, 0 if dead*/
double Unit::Hit(double damage){
	//the unit passes the damage to the boss if this unit is a boss part
	if (flash > 0) {
		if (w.fX != 255){
			w.fX = 255;
			w.fY = 255;
			w.fZ = 255;
		}
		old.vRGB = w;
		old.fAlpha = 1;
		orxObject_SetColor(enemy,&old);
	}
	flash += 10;

	hp-=damage;
	if(hp<=0){
		return 0;
	}
	return 1;
}

double Unit::getHealth(){
	return hp;
}

int Unit::getPartIndex(){
	return partIndex;
}

int Unit::getWeaponUnlocked(){
	return weaponUnlocked;
}

void Unit::handleAction(orxVECTOR* movePosition, int actionTime, int patternId){
	//move to position and shoot the pattern - just keep doing that pattern
	if(patternId){ //if its not 0
		shoot->setPat(patternId);
		//shoot->fire;
	}

	if(movePosition!=NULL){ //if there is a poistion to move to
		target = *movePosition;
		life = 0; //reset fire timer
		moveToTarget = true;
		GetPos();
		targetSpeed = orxVector_GetDistance(&target,&pos)/(double)actionTime;
		//targetSpeed = orxVector_GetDistance(&target,&pos);
		//cout<<"targetSpeed "<<targetSpeed<<" actiontime "<<actionTime<<endl;
		//cout<<"distance "<<orxVector_GetDistance(&target,&pos)<<endl;
	}
}

orxOBJECT* Unit::spawnPointObj(int mult){
	orxVECTOR tempVec;
	orxObject_GetPosition(enemy, &tempVec);

	orxOBJECT* pts = orxObject_CreateFromConfig("Points");
	stringstream string;
	string<<scoreValue*mult;
	orxObject_SetTextString(pts, string.str().c_str());
	orxObject_SetPosition(pts, &tempVec);

	orxVECTOR ptSpeed;
	ptSpeed.fY = -50;
	ptSpeed.fX = 0;
	ptSpeed.fZ = 0;
	orxObject_SetSpeed(pts, &ptSpeed);
	//end point object

	orxCOLOR white;
	orxVECTOR w = {255,255,255};
	white.vRGB = w;
	white.fAlpha = 1;
	orxObject_SetColor(pts, &white);

	return pts;
}
