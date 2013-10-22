#ifndef __Bullet_H__
#define __Bullet_H__

class Bullet;

#include "orx.h" // We want to interface with ORX, so including it is helpful! :)
#include <string> // Our 'GetObjectByName' and 'Update' functions both use strings for the comparison tests.
#include <iostream>

class Bullet
{
public:
	orxSTATUS SetSpeed(orxVECTOR speed);
	orxVECTOR Bullet::GetPos();
	orxSTATUS SetPos(orxVECTOR pos);
	orxSTATUS Bullet::SetRotation(float rot);
	orxVECTOR Bullet::GetSpeed();
	orxSTATUS Bullet::Spin(float amount);
	bool patternIsLaser(orxSTRING pattNum);
	
	orxOBJECT* bullet;

	Bullet(orxVECTOR pos, orxSTRING pattNum, double bulletDamage);
	Bullet();
 
	double getDamage();

private:
	double damage; // If applicable
};
 
#endif // __Bullet_H__