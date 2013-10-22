#include "Player.h"
#include "Pattern.h"
#include <math.h>
#include "GameSystem.h"
#include <iostream>

using namespace std;

orxVECTOR Ppos;


int firing = 1;
int fireControl = 0;
float speed = 300;
float slow = 0;

//int time;

bool offleft, offright, offtop, offbottom;
bool flash;
bool GODMODE;
orxCOLOR orig;
orxOBJECT *activeShip;
orxVECTOR farAwayVector;
orxVECTOR zeroSpeed;
vector<orxOBJECT*> f;




Player::Player(GameSystem* m){
	orxConfig_Load("Player.ini");
	orxConfig_Load("VisualHitBox.ini");
	orxConfig_Load("Explosion.ini");
	ship = orxObject_CreateFromConfig("ShipGraphics");
	activeShip = ship;
	hitBox = orxObject_CreateFromConfig("VisualHitBox");
	exp = orxObject_CreateFromConfig("Explosion");
	ship2 = orxObject_CreateFromConfig("ShipGraphics2");
	ship3 = orxObject_CreateFromConfig("ShipGraphics3");
	shipdead = orxObject_CreateFromConfig("ShipGraphicsDead");
	showHitBox = false;
	pShoot = new PlayerWeapon();
	farAwayVector.fX = -10000;
	farAwayVector.fY = -10000;
	orxObject_SetPosition(ship2, &farAwayVector);
	orxObject_SetPosition(ship3, &farAwayVector);
	orxObject_SetPosition(shipdead, &farAwayVector);
	zeroSpeed.fX = 0;
	zeroSpeed.fY = 0;
	GODMODE = false;

	ObjectInfo *playerObj= new ObjectInfo();
	playerObj->type=PLAYER;
	playerObj->object=this;
	orxObject_SetUserData(ship,playerObj);
	orxObject_SetUserData(ship2,playerObj);
	orxObject_SetUserData(ship3,playerObj);
	hp = 3;
	orxCOLOR white;// = {0,0,0,0}; //this is the default for orx, white - fully lit
	orxVECTOR w = {255,255,255};
	white.vRGB = w;
	white.fAlpha = 1;
	orxObject_SetColor(activeShip, &white);

	orxCOLOR black;
	orxVECTOR bv = {0,0,0};
	black.vRGB = bv;
	black.fAlpha = 1.0;

	playerDead = false;
	charging = false;
	warpAnim = false;
	warpSec = 0;
	warpCounter = 0;
	controlsEnabled = true;
	currentSpacing = PA_INITIALDIST;

	weaponChangeCooldown=0;
	waitReleaseNextWeapon=false;
	waitReleasePrevWeapon=false;

	weaponChargeColorVector.fR = 255;
	weaponChargeColorVector.fG = 255;
	weaponChargeColorVector.fB = 255;
	weaponChargeColor.vRGB = weaponChargeColorVector;
	weaponChargeColor.fAlpha = 1;
	orxObject_SetColor(ship, &weaponChargeColor);
	orxObject_SetColor(ship2, &weaponChargeColor);
	orxObject_SetColor(ship3, &weaponChargeColor);
	fullyChargedAnim = false;
	reverseAnim = false;

	chargeAnimCounter = 0;
}

Player::~Player(){
	delete pShoot;
	orxObject_Delete(shipdead);
	orxObject_Delete(ship);
	orxObject_Delete(ship2);
	orxObject_Delete(ship3);
	orxObject_Delete(exp);
	orxObject_Delete(hitBox);
	GODMODE = false;

	for(int i=0;i<warpAnimGhosts.size();i++){
		if(warpAnimGhosts.at(i)!=NULL){
			orxObject_Delete(warpAnimGhosts.at(i));
		}
	}
	warpAnimGhosts.clear();

	//clean up fragments
	if(f.size()>0){
		for(int i = 0; i < f.size(); i++){
			orxObject_Delete(f.at(i));
		}
		f.clear();
	}
}

bool Player::YouAreAGod(string name){
	if(!name.compare("GODMODE")){
		GODMODE = true;
		return true;
	}
	else {
		GODMODE = false;
		return false;
	}
}

int Player::getCurrentWeapon(){
	return pShoot->getCurrentWeapon();
}

orxOBJECT* Player::getShip(){
	return activeShip;
}

orxVECTOR Player::GetPos(){
	//pos = posA;
	orxVECTOR temp;
	orxObject_GetPosition(activeShip,&temp);
	Ppos = temp;
	return temp;
}

orxSTATUS Player::SetInvuln(bool invuln){ //use to flash - set alpha from 255 to 150 and back fast
	//I did this in a bad way probably
	if(invuln){
		//start freaking out man
		flash = true;
		orxCOLOR white;// = {0,0,0,0}; //this is the default for orx, white - fully lit
		orxVECTOR w = {255,255,255};
		white.vRGB = w;
		white.fAlpha = 1;
		orxObject_SetColor(activeShip, &white);
	}else{
		flash = false;
		orxCOLOR white;// = {0,0,0,0}; //this is the default for orx, white - fully lit
		orxVECTOR w = {255,255,255};
		white.vRGB = w;
		white.fAlpha = 1;
		orxObject_SetColor(activeShip, &white);
	}

	return orxSTATUS_SUCCESS;
}


orxSTATUS Player::SetPos(orxVECTOR temp){
	//pos = posA;
	orxObject_SetPosition(activeShip,&temp);
	Ppos = temp;
	return orxSTATUS_SUCCESS;
}

orxSTATUS Player::checkPos(orxOBJECT* p, orxVECTOR v){
	if((GetPos().fX < -550)){
		orxVECTOR vec = v;
		vec.fX = 0;
		SetPos(vec);
	}

	orxObject_SetSpeed(p, &v);

	return orxSTATUS_SUCCESS;
}

orxSTATUS Player::setLeft(int set){
	if(!playerDead&& controlsEnabled){
		orxVECTOR tempSpeed;
		orxObject_GetSpeed(activeShip,&tempSpeed);
		GetPos();
		orxVECTOR speed = {-(set*slow), tempSpeed.fY, 0};

		//checkPos(ship, speed);
		if(!offleft){
			orxObject_SetSpeed(activeShip, &speed);
			orxObject_SetSpeed(hitBox, &speed);
			orxObject_SetSpeed(exp, &speed);
		}
	}
	return orxSTATUS_SUCCESS;
}

orxSTATUS Player::setUp(int set){
	if(!playerDead&& controlsEnabled){
		orxVECTOR tempSpeed;
		orxObject_GetSpeed(activeShip,&tempSpeed);
		GetPos();
		orxVECTOR speed = {tempSpeed.fX, -set*slow, 0};

		//checkPos(ship, speed);
		if(!offtop){
			orxObject_SetSpeed(activeShip, &speed);
			orxObject_SetSpeed(hitBox, &speed);
			orxObject_SetSpeed(exp, &speed);
		}
	}
	return orxSTATUS_SUCCESS;
}

orxSTATUS Player::setRight(int set){
	if(!playerDead&& controlsEnabled){
		orxVECTOR tempSpeed;
		orxObject_GetSpeed(activeShip,&tempSpeed);
		GetPos();
		orxVECTOR speed = {set*slow, tempSpeed.fY, 0};

		//checkPos(ship, speed);
		if(!offright){
			orxObject_SetSpeed(activeShip, &speed);
			orxObject_SetSpeed(hitBox, &speed);
			orxObject_SetSpeed(exp, &speed);
		}
	}
	return orxSTATUS_SUCCESS;
}

orxSTATUS Player::setDown(int set){
	if(!playerDead && controlsEnabled){
		orxVECTOR tempSpeed;
		orxObject_GetSpeed(activeShip,&tempSpeed);
		GetPos();
		orxVECTOR speed = {tempSpeed.fX, set*slow, 0};

		//checkPos(ship, speed);
		if(!offbottom){
			orxObject_SetSpeed(activeShip, &speed);
			orxObject_SetSpeed(hitBox, &speed);
			orxObject_SetSpeed(exp, &speed);
		}
	}
	return orxSTATUS_SUCCESS;
}

orxSTATUS Player::fire(){
	//pShoot->pVarUp(GetPos());
	//pShoot->setPat(currentWeapon); //IS WHAT WE WANT
	///pShoot->fire(GetPos(),GetPos());
	//the above statement should get the weapon equipped and do its particular
	//type of pattern and have a particular damage associated with its bullets.
	//This could be done with the weapons being ENUMs and using a switch
	//statement to get its particular pattern and damage
	SetRot(0.0);
	SetAngularVelocity(0.0);
	return orxSTATUS_SUCCESS;
}

orxSTATUS Player::setWeapon(int weapon){
	pShoot->changeWeapon(weapon);
	return orxSTATUS_SUCCESS;
}

orxSTATUS Player::SetRot(float rot){
	//Bpos = posA;
	orxObject_SetRotation(activeShip,rot);

	return orxSTATUS_SUCCESS;
}

orxSTATUS Player::SetAngularVelocity(float angVel){

	orxObject_SetAngularVelocity(activeShip,angVel);
	return orxSTATUS_SUCCESS;
}

orxSTATUS Player::Update(bool fireFlag){
	if(!playerDead){
		setExplosion(); //when player is dead, don't move the explosion
	}else{
		orxObject_SetSpeed(exp, &zeroSpeed);
	}

	// End Level Animation ==========================================
	if(warpAnim){
		
		switch(warpSec){
		case 0:{//Initialization of warp animation
			m_Instance->setFireFlag(false);
			warpAnimGhosts.clear();
			orxObject_GetPosition(activeShip,&animLoc);

			disableControls();
			currentSpacing = PA_INITIALDIST;
			
			warpSec = 1;
			warpCounter = PA_ANIMSPEED;

			   }
		case 1:{//Build Ghosts until they hit the top of the screen
			tempVec.fX = animLoc.fX;
			tempVec.fY = animLoc.fY;
			tempVec.fZ = animLoc.fZ;
			if(tempVec.fY-(currentSpacing*PA_MULT) <= -400){
				warpSec = 2;
			}else if(warpCounter >= PA_ANIMSPEED){
				warpCounter = 0;
				orxOBJECT* ghost0;
				bool validGhost = false;
				if(hp == 3)
					ghost0 = orxObject_CreateFromConfig("ShipGraphics");
				if(hp == 2)
					ghost0 = orxObject_CreateFromConfig("ShipGraphics2");
				if(hp == 1)
					ghost0 = orxObject_CreateFromConfig("ShipGraphics3");

				currentSpacing = currentSpacing*PA_MULT;
				tempVec.fY -= currentSpacing;

				orxObject_SetPosition(ghost0,&tempVec);
				warpAnimGhosts.push_back(ghost0);
			}
			warpCounter++;
			break;
			   }
		case 2:{//Ghost have reached top edge - remove player

			orxObject_SetPosition(activeShip, &farAwayVector);
			activeShip = shipdead;
			warpSec = 3;
			warpCounter = 1;
			removePlace = 0;

			break;
			   }
		case 3:{//Remove Ghosts
			
			if(warpCounter >= PA_ANIMSPEED){
				warpCounter = 0;
				orxObject_Delete(warpAnimGhosts.at(removePlace));
				warpAnimGhosts.at(removePlace)=NULL;
				removePlace++;
				if(warpAnimGhosts.size() <= removePlace){
					warpAnim = false;
					warpCounter = 0;
					warpSec = 0;
					controlsEnabled = true;
					
				}else{
					warpCounter++;
				}
			}else{
				warpCounter++;
			}
			break;
			   }
		}
	}
	// END End Level Animation ======================================

	// Flash Blue Anim ==============================================
	if(pShoot->chargeAnim){
		pShoot->chargeAnim = false;
		fullyChargedAnim = true;
	}

	if(fullyChargedAnim){
		if(chargeAnimCounter == -PC_ANIMSPEED){
			weaponChargeColorVector.fR = 255;
			weaponChargeColorVector.fG = 255;
			weaponChargeColorVector.fB = 255;
			weaponChargeColor.vRGB = weaponChargeColorVector;
			orxObject_SetColor(ship, &weaponChargeColor);
			orxObject_SetColor(ship2, &weaponChargeColor);
			orxObject_SetColor(ship3, &weaponChargeColor);
			fullyChargedAnim = false;
			reverseAnim = false;
			chargeAnimCounter = -1;
		}else if(chargeAnimCounter == 0){
			weaponChargeColorVector.fR = 0;
			weaponChargeColorVector.fG = 0;
			weaponChargeColorVector.fB = 255;
			weaponChargeColor.vRGB = weaponChargeColorVector;
			orxObject_SetColor(ship, &weaponChargeColor);
			orxObject_SetColor(ship2, &weaponChargeColor);
			orxObject_SetColor(ship3, &weaponChargeColor);
		}else if(chargeAnimCounter == PC_ANIMSPEED){
			weaponChargeColorVector.fR = 150;
			weaponChargeColorVector.fG = 150;
			weaponChargeColorVector.fB = 255;
			weaponChargeColor.vRGB = weaponChargeColorVector;
			orxObject_SetColor(ship, &weaponChargeColor);
			orxObject_SetColor(ship2, &weaponChargeColor);
			orxObject_SetColor(ship3, &weaponChargeColor);
		}else if(chargeAnimCounter == PC_ANIMSPEED*2){
			weaponChargeColorVector.fR = 0;
			weaponChargeColorVector.fG = 0;
			weaponChargeColorVector.fB = 255;
			weaponChargeColor.vRGB = weaponChargeColorVector;
			orxObject_SetColor(ship, &weaponChargeColor);
			orxObject_SetColor(ship2, &weaponChargeColor);
			orxObject_SetColor(ship3, &weaponChargeColor);
		}else if(chargeAnimCounter == PC_ANIMSPEED*3){
			weaponChargeColorVector.fR = 50;
			weaponChargeColorVector.fG = 50;
			weaponChargeColorVector.fB = 255;
			weaponChargeColor.vRGB = weaponChargeColorVector;
			orxObject_SetColor(ship, &weaponChargeColor);
			orxObject_SetColor(ship2, &weaponChargeColor);
			orxObject_SetColor(ship3, &weaponChargeColor);
		}else if(chargeAnimCounter == PC_ANIMSPEED*4){
			weaponChargeColorVector.fR = 0;
			weaponChargeColorVector.fG = 0;
			weaponChargeColorVector.fB = 255;
			weaponChargeColor.vRGB = weaponChargeColorVector;
			orxObject_SetColor(ship, &weaponChargeColor);
			orxObject_SetColor(ship2, &weaponChargeColor);
			orxObject_SetColor(ship3, &weaponChargeColor);
			reverseAnim = true;
		}
		if(!reverseAnim){
			chargeAnimCounter++;
		}else{
			chargeAnimCounter--;
		}
	}
	// END Flash Blue Anim ==========================================

	//flash if invuln
	SetRot(0.0);
	SetAngularVelocity(0.0);
	if(flash){
		orxCOLOR old;
		orxObject_GetColor(activeShip,&old);
		if(old.fAlpha ==1){
			//decrease alpha level if alpha > 1
			old.fAlpha=0;
		}else{
			//increase alpha level if alpha < .3
			old.fAlpha=1;
		}
		orxObject_SetColor(activeShip,&old);	
	}

	checkChangeWeapon();

	if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_SPACE) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_LCTRL)){
		charging = true;
	}else{
		charging = false;
	}
	pShoot->update(GetPos(),charging,fireFlag);

	if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_LSHIFT) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_RSHIFT)){ //Slow down the ship button
		speed = 100;
		slow = .5;
	}else{	//Normal speed
		speed = 300;
		slow = 1;
	}

	if(GetPos().fX <= -500){ //disallow left movement on left edge of screen
		offleft = true;
		orxVECTOR p = GetPos();
		p.fX = -500;

		//handle "freaking out" at edge of screen"
		orxVECTOR tempSpeed;
		orxObject_GetSpeed(activeShip,&tempSpeed);
		orxVECTOR speed = {0, tempSpeed.fY, 0};
		orxObject_SetSpeed(activeShip,&speed);

		SetPos(p);
	}if(GetPos().fX > -500){
		offleft = false;
	}

	if(GetPos().fX >= 500-64){ //disallow right movement on right edge of screen
		offright = true;
		orxVECTOR p = GetPos();
		p.fX = 500-64;

		//handle "freaking out" at edge of screen"
		orxVECTOR tempSpeed;
		orxObject_GetSpeed(activeShip,&tempSpeed);
		orxVECTOR speed = {0, tempSpeed.fY, 0};
		orxObject_SetSpeed(activeShip,&speed);

		SetPos(p);
	}if(GetPos().fX < 500-64){
		offright = false;
	}

	if(GetPos().fY <= -350){ //disallow up movement on top edge of screen
		offtop = true;
		orxVECTOR p = GetPos();
		p.fY = -350;

		//handle "freaking out" at edge of screen"
		orxVECTOR tempSpeed;
		orxObject_GetSpeed(activeShip,&tempSpeed);
		orxVECTOR speed = {tempSpeed.fX, 0, 0};
		orxObject_SetSpeed(activeShip,&speed);

		SetPos(p);
	}if(GetPos().fY > -350){
		offtop = false;
	}

	if(GetPos().fY >= 350-64-25){ //disallow down movement on bottom edge of screen
		offbottom = true;
		orxVECTOR p = GetPos();
		p.fY = 350-64-25;

		//handle "freaking out" at edge of screen"
		orxVECTOR tempSpeed;
		orxObject_GetSpeed(activeShip,&tempSpeed);
		orxVECTOR speed = {tempSpeed.fX, 0, 0};
		orxObject_SetSpeed(activeShip,&speed);

		SetPos(p);
	}if(GetPos().fY < 350-64-25){
		offbottom = false;
	}

	//HitBox Controls

	if(slow == 0.5 && !showHitBox && !playerDead){
		showHitBox = true;
		orxVECTOR v = GetPos();
		v.fX+=32;
		v.fY+=32;
		v.fZ-=0.1;
		orxObject_SetPosition(hitBox,&v);
		orxObject_SetTargetAnim(hitBox, "Box");

	}else if (slow != 0.5 && showHitBox){
		showHitBox = false;
		orxObject_SetTargetAnim(hitBox, "NoBox");
	}
	if(showHitBox){
		orxVECTOR v = GetPos();
		v.fX+=32;
		v.fY+=32;
		v.fZ-=0.1;
		if(fireFlag){
			orxObject_SetPosition(hitBox,&v);
		}else
			orxObject_SetPosition(hitBox, &farAwayVector);
	}

	if(playerDead){
		orxVECTOR tempPDS = {0,0,0};
		orxObject_SetSpeed(activeShip,&tempPDS);
	}


	return orxSTATUS_SUCCESS;
}

orxSTATUS Player::setFiring(int num){
	firing = num;

	return orxSTATUS_SUCCESS;
}

int Player::getFiring(){
	return firing;
}

/*returns 1 if alive, 0 if dead*/
int Player::Hit(int damage){
	if(!GODMODE)
	hp-=damage;
	if(hp <= 0){
		return 0;
	}

	switch(3-hp){ //handle player "hit" animations
	case 1:
	case 2:
	case 3:
		orxObject_SetTargetAnim(exp, "Explosion1"); 
		orxObject_SetTargetAnim(exp, "IdleExp"); 
		updateGraphic();
	}

	return 1;
}

orxSTATUS Player::heal(int amt){
	if(hp < 3 && hp > 0){
		hp+=amt;
		updateGraphic();
	}

	return orxSTATUS_SUCCESS;
}

orxSTATUS Player::clear(){
	hp = 3000000;
	playerDead = false;
	orxVECTOR restartPos = {-10000,-10000,0};
	controlsEnabled = false;
	
	orxObject_SetPosition(activeShip, &farAwayVector);
	activeShip = shipdead;
	orxObject_SetPosition(activeShip,&restartPos);

	//clean up fragments
	if(f.size()>0){
		for(int i = 0; i < f.size(); i++){
			orxObject_Delete(f.at(i));
		}
		f.clear();
	}

	//reset hitbox position
	orxObject_SetPosition(hitBox, &farAwayVector);

	return orxSTATUS_SUCCESS;
}

orxSTATUS Player::restart(){
	hp = 3;
	playerDead = false;
	orxVECTOR restartPos = {0,200,0};
	controlsEnabled = true;
	
	orxObject_SetPosition(activeShip, &farAwayVector);
	activeShip = ship;
	orxObject_SetPosition(activeShip,&restartPos);

	//clean up fragments
	if(f.size()>0){
		for(int i = 0; i < 4; i++){
			orxObject_Delete(f.at(i));
		}
		f.clear();
	}

	//reset hitbox position
	orxObject_SetPosition(hitBox, &farAwayVector);

	return orxSTATUS_SUCCESS;
}

/*Kill player*/
orxSTATUS Player::Die(){
	//Handle death in some way

	orxObject_SetTargetAnim(exp, "Explosion1"); 
	orxObject_SetTargetAnim(exp, "IdleExp"); 

	orxVECTOR position;// = {20,20,0.5};
	orxObject_GetPosition(activeShip, &position);

	orxObject_SetPosition(activeShip, &farAwayVector);
	activeShip = shipdead;

	playerDead = true;
	showHitBox = false;

	//spawn fragments of ship on death!!!
	f.push_back(orxObject_CreateFromConfig("ShipFragment1"));
	f.push_back(orxObject_CreateFromConfig("ShipFragment2"));
	f.push_back(orxObject_CreateFromConfig("ShipFragment3"));
	f.push_back(orxObject_CreateFromConfig("ShipFragment4"));

	for(int i = 0; i < 4; i++){
		orxObject_SetPosition(f.at(i), &position);
		
		float s1 = (rand()%400) - 200;
		float s2 = (rand()%400) - 200;
		orxVECTOR speed = {s1,s2,0};
		orxObject_SetSpeed(f.at(i), &speed);
		orxObject_ApplyTorque(f.at(i), s1/4000);
	}
	//end spawn

	return orxSTATUS_SUCCESS;
}

//unlocks the weapon of the corresponding index
void Player::unlockWeapon(int i){
	pShoot->unlockWeapon(i);
}

void Player::upgradeWeapon(int ranks){
	pShoot->upgradeWeapon(ranks);
}

void Player::changeCrystalCount(int crystals){
	pShoot->changeCrystalCount(crystals);
}

int Player::getWeaponRank(){
	return pShoot->getRank();
}

bool Player::isWeaponUnlocked(int num){
	return pShoot->isUnlocked(num);
}

void Player::saveInventory(){
	pShoot->saveInventory();
}

void Player::resetInventory(){
	pShoot->resetInventory();
}

void Player::disableControls(){
	controlsEnabled = false;
	orxVECTOR zero = {0,0,0};
	orxObject_SetSpeed(activeShip,&zero);
}

void Player::setExplosion(){
	orxVECTOR pos;
	orxObject_GetPosition(activeShip, &pos);
	pos.fY-=2;
	pos.fX-=2;
	pos.fZ-=.1;
	orxObject_SetPosition(exp, &pos);
}

void Player::updateGraphic(){
	orxVECTOR v;
	orxObject_GetPosition(activeShip, &v);
	switch(hp){
	case 1:
		orxObject_SetPosition(ship3, &v);
		orxObject_SetPosition(activeShip, &farAwayVector);
		orxObject_SetSpeed(activeShip, &zeroSpeed);
		activeShip = ship3;
		break;
	case 2:
		orxObject_SetPosition(ship2, &v);
		orxObject_SetPosition(activeShip, &farAwayVector);
		orxObject_SetSpeed(activeShip, &zeroSpeed);
		activeShip = ship2;
		break;
	case 3:
		orxObject_SetPosition(ship, &v);
		orxObject_SetPosition(activeShip, &farAwayVector);
		orxObject_SetSpeed(activeShip, &zeroSpeed);
		activeShip = ship;
	}
}

void Player::checkChangeWeapon(){
	player_weapon_index currWeapon=pShoot->getCurrentWeapon();
	if(weaponChangeCooldown<=0){
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_1) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_NUMPAD_1) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_Z)){
			setWeapon(0);
			weaponChangeCooldown=WEAPON_CHANGE_COOLDOWN_TIME;
		}
		/*if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_2) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_NUMPAD_2) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_X)){
			setWeapon(1);
			weaponChangeCooldown=WEAPON_CHANGE_COOLDOWN_TIME;
		}*/ // No laser support
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_3) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_NUMPAD_3) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_C)){
			setWeapon(2);
			weaponChangeCooldown=WEAPON_CHANGE_COOLDOWN_TIME;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_4) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_NUMPAD_4) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_V)){
			setWeapon(3);
			weaponChangeCooldown=WEAPON_CHANGE_COOLDOWN_TIME;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_5) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_NUMPAD_5) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_B)){
			setWeapon(4);
			weaponChangeCooldown=WEAPON_CHANGE_COOLDOWN_TIME;
		}
		/*if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_5) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_NUMPAD_5) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_B)){
			setWeapon(5);
			weaponChangeCooldown=WEAPON_CHANGE_COOLDOWN_TIME;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_6) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_NUMPAD_6) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_N)){
			setWeapon(6);
			weaponChangeCooldown=WEAPON_CHANGE_COOLDOWN_TIME;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_7) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_NUMPAD_7) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_M)){
			setWeapon(7);
			weaponChangeCooldown=WEAPON_CHANGE_COOLDOWN_TIME;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_8) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_NUMPAD_8) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_COMMA)){
			setWeapon(8);
			weaponChangeCooldown=WEAPON_CHANGE_COOLDOWN_TIME;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_9) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_NUMPAD_9) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_PERIOD)){
			setWeapon(9);
			weaponChangeCooldown=WEAPON_CHANGE_COOLDOWN_TIME;
		}*/
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_E) && waitReleaseNextWeapon==false){
			//set the weapon to the next unlocked weapon in the array of weapons
			setWeapon(CHANGE_NEXT_WEAPON);
			weaponChangeCooldown=WEAPON_CHANGE_COOLDOWN_TIME;
			waitReleaseNextWeapon=true;
		}
		else if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_E) != true){
			waitReleaseNextWeapon=false;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_Q) && waitReleasePrevWeapon==false){
			//set the weapon to the previous unlocked weapon in the array of weapons
			setWeapon(CHANGE_PREV_WEAPON);
			weaponChangeCooldown=WEAPON_CHANGE_COOLDOWN_TIME;
			waitReleasePrevWeapon=true;
		}
		else if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_Q) != true){
			waitReleasePrevWeapon=false;
		}
		if(currWeapon==pShoot->getCurrentWeapon()){
			weaponChangeCooldown=0;
		}
	}
	else{
		weaponChangeCooldown--;
	}
}

void Player::startFullyChargedAnim(){
	fullyChargedAnim = true;
}

