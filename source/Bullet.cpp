#include "Bullet.h"
#include "GameSystem.h"

//Handles firing weapons and movement every update -- called every clock

//we store the enemy speed, angle, and position
orxVECTOR Bangle = {0,0,0};
orxVECTOR Bspeed = {0,0,0};
orxVECTOR Bpos = {0,0,0};
orxSTRING bulletName;

Bullet::Bullet()
{
}

Bullet::Bullet(orxVECTOR posA, orxSTRING pattNum, double bulletDamage){
	damage = bulletDamage;
	
	//orxSTRING enemyName(enemyNum);
	//hold all of our enemies in the ini. Create the specified enemy.
	Bpos = posA;
	//add a switch case for the pattNums, for now its just 1
	bullet = orxObject_CreateFromConfig( pattNum ); //create the bullet they gave us
	bulletName = pattNum;
	//put it where we're told to put it
	SetPos(Bpos);
	ObjectInfo *bulletObj= new ObjectInfo();
	if(patternIsLaser(pattNum)){
		bulletObj->type=LASER;
	}
	else{
		bulletObj->type=BULLET;
		orxObject_SetLifeTime(bullet,10.0f); //set lifetime in seconds
	}
	bulletObj->object=this;
	orxObject_SetUserData(bullet,bulletObj);
}

orxSTATUS Bullet::SetSpeed(orxVECTOR speedA){
	Bspeed = speedA;
	orxObject_SetSpeed(bullet,&Bspeed);

	return orxSTATUS_SUCCESS;
}

orxVECTOR Bullet::GetSpeed(){
	orxVECTOR tempSpeed;
	orxObject_GetSpeed(bullet,&tempSpeed);
	Bspeed = tempSpeed;

	return tempSpeed;
}

orxSTATUS Bullet::Spin(float amount){

	orxObject_ApplyTorque(bullet, amount);
	return orxSTATUS_SUCCESS;
}

orxVECTOR Bullet::GetPos(){
	orxVECTOR temp;
	orxObject_GetPosition(bullet,&temp);
	Bpos = temp;
	return temp;
}

orxSTATUS Bullet::SetPos(orxVECTOR posA){
	Bpos = posA;
	orxObject_SetPosition(bullet,&Bpos);

	return orxSTATUS_SUCCESS;
}

orxSTATUS Bullet::SetRotation(float rot){
	//Bpos = posA;
	orxObject_SetRotation(bullet,rot);

	return orxSTATUS_SUCCESS;
}

double Bullet::getDamage(){
	
	return damage;
}

bool Bullet::patternIsLaser(orxSTRING pattNum){
	if(strcmp(pattNum,"Laser1")==0 || strcmp(pattNum,"Laser3")==0){
		return true;
	}
	return false;
}