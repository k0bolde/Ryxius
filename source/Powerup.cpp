#include "Powerup.h"
#include "GameSystem.h"
#include <iostream>

Powerup::Powerup(){
	type=HEALTH;
	value=1;

	powerup=orxObject_CreateFromConfig( "Powerup" );

	//The following is for getting this object through collisions
	ObjectInfo *powerupObj= new ObjectInfo();
	powerupObj->type=POWERUP;
	powerupObj->object=this;
	orxObject_SetUserData(powerup,powerupObj);

	orxVECTOR pos={0,0,0};
	orxVECTOR speed={0,15,0};
	orxObject_SetPosition(powerup,&pos);
	orxObject_SetSpeed(powerup, &speed);
}
Powerup::Powerup(powerup_type t, int v, orxVECTOR pos, orxVECTOR speed){
	type=t;
	value=v;
	position=pos;
	
	switch(t){
	case WEAPON_UNLOCK:
		powerup=orxObject_CreateFromConfig( "WeaponUnlock" );
		break;

	case CRYSTAL:
		powerup=orxObject_CreateFromConfig( "Crystal" );
		break;
	default:
		powerup=orxObject_CreateFromConfig( "Powerup" );
		break;
	}

	//The following is for getting this object through collisions
	ObjectInfo *powerupObj= new ObjectInfo();
	powerupObj->type=POWERUP;
	powerupObj->object=this;
	orxObject_SetUserData(powerup,powerupObj);

	orxObject_SetPosition(powerup,&pos);
	orxObject_SetSpeed(powerup, &speed);
}

Powerup::~Powerup(){
	DeletePowerup();
}

orxSTATUS Powerup::DeletePowerup(){
	orxObject_Delete(powerup);
	return orxSTATUS_SUCCESS;
}

powerup_type Powerup::getType(){
	return type;
}

int Powerup::getValue(){
	return value;
}

orxVECTOR* Powerup::getPosition(){
	orxObject_GetPosition(powerup,&position);
	return &position;
}

orxOBJECT* Powerup::spawnPointObj(int val){
	orxVECTOR tempVec;
	orxObject_GetPosition(powerup, &tempVec);

	//make a point object
	stringstream pointsText;
	pointsText<<"Points";
	//cout<<enemyName; //debug to find enemy's name for the ini's
	orxOBJECT* pts = orxObject_CreateFromConfig(pointsText.str().c_str());
	stringstream string;

	string<<val;

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