#include "PlayerWeapon.h"
#include "Player.h"
#include <iostream>
using namespace std;


PlayerWeapon::PlayerWeapon(){
	currentWeapon = GATLING_CANNON;
	rank = 0;
	savedRank = 0;
	currentCrystals=0;
	shootSound = orxSound_CreateFromConfig("playerShoot");
	//Initialize Arrays
	for(int i = 0; i < NUMBER_OF_PLAYER_WEAPONS; i++){
		unlocks[i] = 0;
		counter[i] = 0;
		savedUnlocks[i] = 0;
	}
	unlocks[0]=true;
	savedUnlocks[0] = true;
	unlocks[MISSILE_LAUNCHER]=true;
	savedUnlocks[MISSILE_LAUNCHER] = true;

	unlocks[LASER_SHOTGUN] = true;
	savedUnlocks[LASER_SHOTGUN] = true;
	unlocks[4]=true;
	savedUnlocks[4]=true;

	chargeAnim = false;

	dispensing = false;

	orxCOLOR black;
	orxVECTOR bv = {0,0,0};
	black.vRGB = bv;
	black.fAlpha = 1.0;

	playerWeaponRankText=orxObject_CreateFromConfig("PlayerWeapon");
	playerCrystalTotalText=orxObject_CreateFromConfig("WeaponLevel");
	orxObject_SetColor(playerWeaponRankText, &black);
	orxObject_SetColor(playerCrystalTotalText, &black);
	orxObject_SetTextString(playerWeaponRankText, "1");
	orxObject_SetTextString(playerCrystalTotalText, "0");

	uiCrystalBar=orxObject_CreateFromConfig("uiCrystalBar");

	currentWave = 0;


	farAwayVector.fX=-10000;
	farAwayVector.fY=-10000;

	

}

PlayerWeapon::~PlayerWeapon(){
	orxObject_Delete(playerWeaponRankText);
	orxObject_Delete(playerCrystalTotalText);
	orxObject_Delete(uiCrystalBar);
}

void PlayerWeapon::update(orxVECTOR playerPos, bool charging, bool fireFlag){
	// Update Counters for all weapons
	for(int i = 0; i < NUMBER_OF_PLAYER_WEAPONS*6; i++){
		if(counter[i] > 0){
			counter[i]--;
		}
	}


	if(!charging){
		if(chargeCounter < 30 && !dispensing){
			chargeCounter = 0;
		}else if(chargeCounter >= 30){
			dispensing = true;
		}else if(chargeCounter == 1){
			dispensing = false;
		}

		if(fireFlag)
			fire(playerPos);

	}else{
		dispensing = false;
		if(chargeCounter < MAXCHARGE)
			chargeCounter++;
		else if(chargeCounter > MAXCHARGE){
			chargeCounter = MAXCHARGE;
			cout << "Charge fixed" << endl;
		}

		if(chargeCounter == MAXCHARGE-1){
			chargeAnim = true;
		}
	}

}

void PlayerWeapon::fire(orxVECTOR playerPos){

	int use = currentWeapon*6;
	// Use right version of weapon
	if(chargeCounter > 0){
		use++;
		chargeCounter--;
	}
	use+= rank*2;
	if(counter[use] > 0){
		return;
	}
	int numberOfBullets;
	int movement;
	int randC;
	float angle;
	
	if((use < 24) || (use > 29)){
		currentWave = 0;
	}

	orxSound_Play(shootSound);
	switch (use)
	{
		//code for porting straight from pattern.cpp
		orxVECTOR t;
		Bullet *temp;
		//end port code===============================

	case 0:// Gatling Cannon: Rank 1 - Standard

		tempVec = playerPos;
		tempVec.fY +=10;
		tempVec.fX +=32;
		tempBullet = new Bullet(tempVec, "blueOvalBullet",1);
		tempInt = 1;
		if(rand()%2 == 0){
			tempInt = -1;
		}

		tempVec.fX = (tempInt*randFloat(0,25));
		tempVec.fY = (-400)*randFloat(3,5);
		tempBullet->SetSpeed(tempVec);
		counter[0] = 60/BASEDAMAGEPERSECOND; // Cooldown

		break;
	case 1:// Gatling Cannon: Rank 1 - Strong
		tempVec = playerPos;
		tempVec.fY +=10;
		tempVec.fX +=32;
		tempBullet = new Bullet(tempVec, "blueOvalBullet",1);
		tempInt = 1;
		if(rand()%2 == 0){
			tempInt = -1;
		}

		tempVec.fX = (tempInt*randFloat(0,200));
		tempVec.fY = (-400)*randFloat(3,5);
		tempBullet->SetSpeed(tempVec);
		counter[1] = (60/BASEDAMAGEPERSECOND)/DAMAGEMULTSTRONG; // Cooldown
		break;
	case 2:// Gatling Cannon: Rank 2 - Standard
		tempVec = playerPos;
		tempVec.fY +=10;
		tempVec.fX +=32;
		tempBullet = new Bullet(tempVec, "blueOvalBullet",1);
		tempInt = 1;
		if(rand()%2 == 0){
			tempInt = -1;
		}

		tempVec.fX = (tempInt*randFloat(0,100));
		tempVec.fY = (-450)*randFloat(3,6);
		tempBullet->SetSpeed(tempVec);
		counter[2] = (60/(BASEDAMAGEPERSECOND*DAMAGEMULTPERRANK)); // Cooldown

		break;
	case 3:// Gatling Cannon: Rank 2 - Strong
		tempVec = playerPos;
		tempVec.fY +=10;
		tempVec.fX +=32;
		tempBullet = new Bullet(tempVec, "blueOvalBullet",1);
		tempInt = 1;
		if(rand()%2 == 0){
			tempInt = -1;
		}

		tempVec.fX = (tempInt*randFloat(0,200));
		tempVec.fY = (-400)*randFloat(3,5);
		tempBullet->SetSpeed(tempVec);
		counter[1] = (60/(BASEDAMAGEPERSECOND*DAMAGEMULTPERRANK*DAMAGEMULTSTRONG)); // Cooldown

		break;
	case 4:// Gatling Cannon: Rank 3 - Standard
		tempVec = playerPos;
		tempVec.fY +=10;
		tempVec.fX +=10;
		tempBullet = new Bullet(tempVec, "blueOvalBullet",1);
		tempInt = 1;
		if(rand()%2 == 0){
			tempInt = -1;
		}

		tempVec.fX = (tempInt*randFloat(0,100));
		tempVec.fY = (-450)*randFloat(3,6);
		tempBullet->SetSpeed(tempVec);

		tempVec = playerPos;
		tempVec.fY +=10;
		tempVec.fX +=54;
		tempBullet = new Bullet(tempVec, "blueOvalBullet",1);
		tempInt = 1;
		if(rand()%2 == 0){
			tempInt = -1;
		}

		tempVec.fX = (tempInt*randFloat(0,100));
		tempVec.fY = (-450)*randFloat(3,6);
		tempBullet->SetSpeed(tempVec);

		counter[4] = (60/(BASEDAMAGEPERSECOND*DAMAGEMULTPERRANK*DAMAGEMULTPERRANK))*2; // Cooldown
		break;
	case 5:// Gatling Cannon: Rank 3 - Strong
		tempVec = playerPos;
		tempVec.fY +=10;
		tempVec.fX +=10;
		tempBullet = new Bullet(tempVec, "blueOvalBullet",1);
		tempInt = 1;
		if(rand()%2 == 0){
			tempInt = -1;
		}

		tempVec.fX = (tempInt*randFloat(0,200));
		tempVec.fY = (-400)*randFloat(3,5);
		tempBullet->SetSpeed(tempVec);
		counter[5] = 2; // Cooldown

		tempVec = playerPos;
		tempVec.fY +=10;
		tempVec.fX +=54;
		tempBullet = new Bullet(tempVec, "blueOvalBullet",1);
		tempInt = 1;
		if(rand()%2 == 0){
			tempInt = -1;
		}

		tempVec.fX = (tempInt*randFloat(0,200));
		tempVec.fY = (-400)*randFloat(3,5);
		tempBullet->SetSpeed(tempVec);
		counter[5] = (60/(BASEDAMAGEPERSECOND*DAMAGEMULTPERRANK*DAMAGEMULTPERRANK*DAMAGEMULTSTRONG))*2;  // Cooldown

		break;
	case 6:// Laser Cannon: Rank 1 - Standard
		break;
	case 7:// Laser Cannon: Rank 1 - Strong
		break;
	case 8:// Laser Cannon: Rank 2 - Standard
		break;
	case 9:// Laser Cannon: Rank 2 - Strong
		break;
	case 10:// Laser Cannon: Rank 3 - Standard
		break;
	case 11:// Laser Cannon: Rank 3 - Strong
		break;

	case 12:// Shotgun: Rank 1 - Standard
		//spawn and launch some bullets under the player - 5 for start
		numberOfBullets = (BASEDAMAGEPERSECOND+5)/2;
		for(int i = 0; i < numberOfBullets; i +=1){
			//jitter the tempVec
			tempVec = playerPos;
			//cout<<"Running through the shoot loop "<<i<<endl;
			int x = rand()%16;//32
			int y = rand()%16;
			if(rand()%2==0) x *= -1;
			if(rand()%2==0) y *= -1;
			tempVec.fX+=x+32;
			tempVec.fY+=y;

			tempBullet = new Bullet(tempVec,"shotBullet",1);
			tempVec.fX = rand()%150;
			tempVec.fY = -400;
			if(rand()%2==0) tempVec.fX *=-1;
			randC = rand();
			if(randC <= (RAND_MAX*(1.0/3.0))){
				movement = SHOTGUN_SPEED1;
			}else if(randC <= (RAND_MAX*(2.0/3.0))){
				movement = SHOTGUN_SPEED2;
			}else{
				movement = SHOTGUN_SPEED3;
			}
			orxVector_Normalize(&tempVec,&tempVec);
			orxVector_Mulf(&tempVec,&tempVec,movement);
			tempBullet->SetSpeed(tempVec);
		}
		counter[12] = 30;
		break;

	case 13:// Shotgun: Rank 1 - Strong
		//spawn and launch some bullets under the player - 5 for start
		numberOfBullets = ((BASEDAMAGEPERSECOND+5)*DAMAGEMULTSTRONG)/2;
		for(int i = 0; i < numberOfBullets; i +=1){
			//jitter the tempVec
			tempVec = playerPos;
			//cout<<"Running through the shoot loop "<<i<<endl;
			int x = rand()%16;//32
			int y = rand()%16;
			if(rand()%2==0) x *= -1;
			if(rand()%2==0) y *= -1;
			tempVec.fX+=x+32;
			tempVec.fY+=y;

			tempBullet = new Bullet(tempVec,"shotBullet",1);
			tempVec.fX = rand()%150;
			tempVec.fY = -400;
			if(rand()%2==0) tempVec.fX *=-1;
			randC = rand();
			if(randC <= (RAND_MAX*(1.0/3.0))){
				movement = SHOTGUN_SPEED1;
			}else if(randC <= (RAND_MAX*(2.0/3.0))){
				movement = SHOTGUN_SPEED2;
			}else{
				movement = SHOTGUN_SPEED3;
			}
			orxVector_Normalize(&tempVec,&tempVec);
			orxVector_Mulf(&tempVec,&tempVec,movement);
			tempBullet->SetSpeed(tempVec);
		}
		counter[13] = 30;
		break;

	case 14:// Shotgun: Rank 2 - Standard
		//spawn and launch some bullets under the player - 5 for start
		numberOfBullets = ((BASEDAMAGEPERSECOND+5)*DAMAGEMULTPERRANK)/2;
		for(int i = 0; i < numberOfBullets; i +=1){
			//jitter the tempVec
			tempVec = playerPos;
			//cout<<"Running through the shoot loop "<<i<<endl;
			int x = rand()%16;//32
			int y = rand()%16;
			if(rand()%2==0) x *= -1;
			if(rand()%2==0) y *= -1;
			tempVec.fX+=x+32;
			tempVec.fY+=y;

			tempBullet = new Bullet(tempVec,"shotBullet",1);
			tempVec.fX = rand()%150;
			tempVec.fY = -400;
			if(rand()%2==0) tempVec.fX *=-1;
			randC = rand();
			if(randC <= (RAND_MAX*(1.0/3.0))){
				movement = SHOTGUN_SPEED1;
			}else if(randC <= (RAND_MAX*(2.0/3.0))){
				movement = SHOTGUN_SPEED2;
			}else{
				movement = SHOTGUN_SPEED3;
			}
			orxVector_Normalize(&tempVec,&tempVec);
			orxVector_Mulf(&tempVec,&tempVec,movement);
			tempBullet->SetSpeed(tempVec);
		}
		counter[14] = 30;
		break;

	case 15:// Shotgun: Rank 2 - Strong

		//spawn and launch some bullets under the player - 5 for start
		numberOfBullets = ((BASEDAMAGEPERSECOND+5)*DAMAGEMULTPERRANK*DAMAGEMULTSTRONG)/2;
		for(int i = 0; i < numberOfBullets; i +=1){
			//jitter the tempVec
			tempVec = playerPos;
			//cout<<"Running through the shoot loop "<<i<<endl;
			int x = rand()%16;//32
			int y = rand()%16;
			if(rand()%2==0) x *= -1;
			if(rand()%2==0) y *= -1;
			tempVec.fX+=x+32;
			tempVec.fY+=y;

			tempBullet = new Bullet(tempVec,"shotBullet",1);
			tempVec.fX = rand()%150;
			tempVec.fY = -400;
			if(rand()%2==0) tempVec.fX *=-1;
			randC = rand();
			if(randC <= (RAND_MAX*(1.0/3.0))){
				movement = SHOTGUN_SPEED1;
			}else if(randC <= (RAND_MAX*(2.0/3.0))){
				movement = SHOTGUN_SPEED2;
			}else{
				movement = SHOTGUN_SPEED3;
			}
			orxVector_Normalize(&tempVec,&tempVec);
			orxVector_Mulf(&tempVec,&tempVec,movement);
			tempBullet->SetSpeed(tempVec);
		}
		counter[15] = 30;
		break;

	case 16:// Shotgun: Rank 3 - Standard
		//spawn and launch some bullets under the player - 5 for start
		numberOfBullets = ((BASEDAMAGEPERSECOND+5)*DAMAGEMULTPERRANK*DAMAGEMULTPERRANK)/2;
		for(int i = 0; i < numberOfBullets; i +=1){
			//jitter the tempVec
			tempVec = playerPos;
			//cout<<"Running through the shoot loop "<<i<<endl;
			int x = rand()%16;//32
			int y = rand()%16;
			if(rand()%2==0) x *= -1;
			if(rand()%2==0) y *= -1;
			tempVec.fX+=x+32;
			tempVec.fY+=y;

			tempBullet = new Bullet(tempVec,"shotBullet",1);
			tempVec.fX = rand()%150;
			tempVec.fY = -400;
			if(rand()%2==0) tempVec.fX *=-1;
			randC = rand();
			if(randC <= (RAND_MAX*(1.0/3.0))){
				movement = SHOTGUN_SPEED1;
			}else if(randC <= (RAND_MAX*(2.0/3.0))){
				movement = SHOTGUN_SPEED2;
			}else{
				movement = SHOTGUN_SPEED3;
			}
			orxVector_Normalize(&tempVec,&tempVec);
			orxVector_Mulf(&tempVec,&tempVec,movement);
			tempBullet->SetSpeed(tempVec);
		}
		counter[16] = 30;
		break;

	case 17:// Shotgun: Rank 3 - Strong
		//spawn and launch some bullets under the player - 5 for start
		numberOfBullets = ((BASEDAMAGEPERSECOND+5)*DAMAGEMULTPERRANK*DAMAGEMULTPERRANK*DAMAGEMULTSTRONG)/2;
		for(int i = 0; i < numberOfBullets; i +=1){
			//jitter the tempVec
			tempVec = playerPos;
			//cout<<"Running through the shoot loop "<<i<<endl;
			int x = rand()%16;//32
			int y = rand()%16;
			if(rand()%2==0) x *= -1;
			if(rand()%2==0) y *= -1;
			tempVec.fX+=x+32;
			tempVec.fY+=y;

			tempBullet = new Bullet(tempVec,"shotBullet",1);
			tempVec.fX = rand()%150;
			tempVec.fY = -400;
			if(rand()%2==0) tempVec.fX *=-1;
			randC = rand();
			if(randC <= (RAND_MAX*(1.0/3.0))){
				movement = SHOTGUN_SPEED1;
			}else if(randC <= (RAND_MAX*(2.0/3.0))){
				movement = SHOTGUN_SPEED2;
			}else{
				movement = SHOTGUN_SPEED3;
			}
			orxVector_Normalize(&tempVec,&tempVec);
			orxVector_Mulf(&tempVec,&tempVec,movement);
			tempBullet->SetSpeed(tempVec);
		}
		counter[17] = 30;
		break;
	case 18:// Missile Launcher: Rank 1 - Standard
		tempVec = playerPos;
		tempVec.fY +=10;
		tempVec.fX +=32;
		tempBullet = new Bullet(tempVec, "Missile1",5);
		tempVec.fX = 0;
		tempVec.fY = -400;
		tempBullet->SetSpeed(tempVec);
		counter[18] = MISSILE_COOLDOWN;
		counter[20] = MISSILE_COOLDOWN;
		counter[22] = MISSILE_COOLDOWN;
		break;
	case 19:// Missile Launcher: Rank 1 - Strong
		if(chargeCounter == MAXCHARGE-1){
			tempVec = playerPos;
			tempVec.fY +=10;
			tempVec.fX +=32;
			tempBullet = new Bullet(tempVec, "Missile1Strong",5);
			tempVec.fX = 0;
			tempVec.fY = -800;
			tempBullet->SetSpeed(tempVec);
			counter[18] = MISSILE_COOLDOWN;
			counter[20] = MISSILE_COOLDOWN;
			counter[22] = MISSILE_COOLDOWN;
			chargeCounter = 0;
		}else{
			chargeCounter = 0;
		}
		break;
	case 20:// Missile Launcher: Rank 2 - Standard
		tempVec = playerPos;
		tempVec.fY +=10;
		tempVec.fX +=32;
		tempBullet = new Bullet(tempVec, "Missile2",5);
		tempVec.fX = 0;
		tempVec.fY = -400;
		tempBullet->SetSpeed(tempVec);
		counter[18] = MISSILE_COOLDOWN;
		counter[20] = MISSILE_COOLDOWN;
		counter[22] = MISSILE_COOLDOWN;
		break;
	case 21:// Missile Launcher: Rank 2 - Strong
		if(chargeCounter == MAXCHARGE-1){
			tempVec = playerPos;
			tempVec.fY +=10;
			tempVec.fX +=32;
			tempBullet = new Bullet(tempVec, "Missile2Strong",5);
			tempVec.fX = 0;
			tempVec.fY = -800;
			tempBullet->SetSpeed(tempVec);
			counter[18] = MISSILE_COOLDOWN;
			counter[20] = MISSILE_COOLDOWN;
			counter[22] = MISSILE_COOLDOWN;
			chargeCounter = 0;
		}else{
			chargeCounter = 0;
		}
		break;
	case 22:// Missile Launcher: Rank 3 - Standard
		tempVec = playerPos;
		tempVec.fY +=10;
		tempVec.fX +=32;
		tempBullet = new Bullet(tempVec, "Missile3",5);
		tempVec.fX = 0;
		tempVec.fY = -400;
		tempBullet->SetSpeed(tempVec);
		counter[18] = MISSILE_COOLDOWN;
		counter[20] = MISSILE_COOLDOWN;
		counter[22] = MISSILE_COOLDOWN;
		break;

	case 23:// Missile Launcher: Rank 3 - Strong
		if(chargeCounter == MAXCHARGE-1){
			tempVec = playerPos;
			tempVec.fY +=10;
			tempVec.fX +=32;
			tempBullet = new Bullet(tempVec, "Missile3Strong",5);
			tempVec.fX = 0;
			tempVec.fY = -800;
			tempBullet->SetSpeed(tempVec);
			counter[18] = MISSILE_COOLDOWN;
			counter[20] = MISSILE_COOLDOWN;
			counter[22] = MISSILE_COOLDOWN;
			chargeCounter = 0;
		}else{
			chargeCounter = 0;
		}
		break;
	case 24: // Wave: Rank 1 - Standard
		
		numberOfBullets = 50;
		angle = (3*3.14159265)/2 -  WAVETHETA;
		for(int i = 0; i < numberOfBullets; i++){
			tempVec.fX = (WAVERADIUS*cos(angle))+playerPos.fX+32; // Add circle center
			tempVec.fY = -(WAVERADIUS*sin(angle))+playerPos.fY;
			
			tempBullet = new Bullet(tempVec,"WaveBullet",(BASEDAMAGEPERSECOND*(WAVECOOLDOWN/60.0))/(numberOfBullets/4));
			tempVec.fX = WAVE_SPEED5*cos(angle);
			tempVec.fY = -WAVE_SPEED5*sin(angle);
			angle += (2*WAVETHETA)/(numberOfBullets-1);
			orxObject_SetSpeed(tempBullet->bullet, &tempVec);
		}
		counter[24] = WAVECOOLDOWN;
		counter[25] = WAVECOOLDOWN;
		counter[26] = WAVECOOLDOWN;
		counter[27] = WAVECOOLDOWN;
		counter[28] = WAVECOOLDOWN;
		counter[29] = WAVECOOLDOWN;
		break;
	case 25: // Wave: Rank 1 - Strong
		numberOfBullets = 50; // For two waves
		angle = (3*3.14159265)/2 -  WAVETHETA;
		if(currentWave == 0){
			for(int i = 0; i < numberOfBullets; i++){
				tempVec.fX = (WAVERADIUS*cos(angle))+playerPos.fX+32; // Add circle center
				tempVec.fY = -(WAVERADIUS*sin(angle))+playerPos.fY;

				tempBullet = new Bullet(tempVec,"WaveBullet",( ( (BASEDAMAGEPERSECOND*(WAVECOOLDOWN/60.0)) / (numberOfBullets/4) )* DAMAGEMULTSTRONG ) / 2);
				tempVec.fX = WAVE_SPEED4*cos(angle);
				tempVec.fY = -WAVE_SPEED4*sin(angle);
				angle += (2*WAVETHETA)/(numberOfBullets-1);
				orxObject_SetSpeed(tempBullet->bullet, &tempVec);
				
				
			}
			counter[24] = WAVESEPERATION;
			counter[25] = WAVESEPERATION;
			counter[26] = WAVESEPERATION;
			counter[27] = WAVESEPERATION;
			counter[28] = WAVESEPERATION;
			counter[29] = WAVESEPERATION;
			currentWave++;
		}else if(currentWave == 1){
			for(int i = 0; i < numberOfBullets; i++){
				tempVec.fX = (WAVERADIUS*cos(angle))+playerPos.fX+32; // Add circle center
				tempVec.fY = -(WAVERADIUS*sin(angle))+playerPos.fY;

				tempBullet = new Bullet(tempVec,"WaveBullet",( ( (BASEDAMAGEPERSECOND*(WAVECOOLDOWN/60.0)) / (numberOfBullets/4) )* DAMAGEMULTSTRONG ) / 2);
				tempVec.fX = WAVE_SPEED5*cos(angle);
				tempVec.fY = -WAVE_SPEED5*sin(angle);
				angle += (2*WAVETHETA)/(numberOfBullets-1);
				orxObject_SetSpeed(tempBullet->bullet, &tempVec);
				
			}
			counter[24] = WAVECOOLDOWN;
			counter[25] = WAVECOOLDOWN;
			counter[26] = WAVECOOLDOWN;
			counter[27] = WAVECOOLDOWN;
			counter[28] = WAVECOOLDOWN;
			counter[29] = WAVECOOLDOWN;

			currentWave = 0;
		}else{
			currentWave = 0;
		}
		break;
	case 26: // Wave: Rank 2 - Standard
		numberOfBullets = 50; // For two waves
		angle = (3*3.14159265)/2 -  WAVETHETA;
		if(currentWave == 0){
			for(int i = 0; i < numberOfBullets; i++){
				tempVec.fX = (WAVERADIUS*cos(angle))+playerPos.fX+32; // Add circle center
				tempVec.fY = -(WAVERADIUS*sin(angle))+playerPos.fY;

				tempBullet = new Bullet(tempVec,"WaveBullet",( ( (BASEDAMAGEPERSECOND*(WAVECOOLDOWN/60.0)) / (numberOfBullets/4) )* DAMAGEMULTPERRANK ) / 2);
				tempVec.fX = WAVE_SPEED4*cos(angle);
				tempVec.fY = -WAVE_SPEED4*sin(angle);
				angle += (2*WAVETHETA)/(numberOfBullets-1);
				orxObject_SetSpeed(tempBullet->bullet, &tempVec);
				
				
			}
			counter[24] = WAVESEPERATION;
			counter[25] = WAVESEPERATION;
			counter[26] = WAVESEPERATION;
			counter[27] = WAVESEPERATION;
			counter[28] = WAVESEPERATION;
			counter[29] = WAVESEPERATION;
			currentWave++;
		}else if(currentWave == 1){
			for(int i = 0; i < numberOfBullets; i++){
				tempVec.fX = (WAVERADIUS*cos(angle))+playerPos.fX+32; // Add circle center
				tempVec.fY = -(WAVERADIUS*sin(angle))+playerPos.fY;

				tempBullet = new Bullet(tempVec,"WaveBullet",( ( (BASEDAMAGEPERSECOND*(WAVECOOLDOWN/60.0)) / (numberOfBullets/4) )* DAMAGEMULTPERRANK ) / 2);
				tempVec.fX = WAVE_SPEED5*cos(angle);
				tempVec.fY = -WAVE_SPEED5*sin(angle);
				angle += (2*WAVETHETA)/(numberOfBullets-1);
				orxObject_SetSpeed(tempBullet->bullet, &tempVec);
				
			}
			counter[24] = WAVECOOLDOWN;
			counter[25] = WAVECOOLDOWN;
			counter[26] = WAVECOOLDOWN;
			counter[27] = WAVECOOLDOWN;
			counter[28] = WAVECOOLDOWN;
			counter[29] = WAVECOOLDOWN;

			currentWave = 0;
		}else{
			currentWave = 0;
		}
		break;
	case 27: // Wave: Rank 2 - Strong
		numberOfBullets = 50; // For two waves
		angle = (3*3.14159265)/2 -  WAVETHETA;
		if(currentWave == 0){
			for(int i = 0; i < numberOfBullets; i++){
				tempVec.fX = (WAVERADIUS*cos(angle))+playerPos.fX+32; // Add circle center
				tempVec.fY = -(WAVERADIUS*sin(angle))+playerPos.fY;

				tempBullet = new Bullet(tempVec,"WaveBullet",( ( (BASEDAMAGEPERSECOND*(WAVECOOLDOWN/60.0)) / (numberOfBullets/4) )* DAMAGEMULTPERRANK * DAMAGEMULTSTRONG ) / 3);
				tempVec.fX = WAVE_SPEED3*cos(angle);
				tempVec.fY = -WAVE_SPEED3*sin(angle);
				angle += (2*WAVETHETA)/(numberOfBullets-1);
				orxObject_SetSpeed(tempBullet->bullet, &tempVec);
				
				
			}
			counter[24] = WAVESEPERATION;
			counter[25] = WAVESEPERATION;
			counter[26] = WAVESEPERATION;
			counter[27] = WAVESEPERATION;
			counter[28] = WAVESEPERATION;
			counter[29] = WAVESEPERATION;
			currentWave++;
		}else if(currentWave == 1){
			for(int i = 0; i < numberOfBullets; i++){
				tempVec.fX = (WAVERADIUS*cos(angle))+playerPos.fX+32; // Add circle center
				tempVec.fY = -(WAVERADIUS*sin(angle))+playerPos.fY;

				tempBullet = new Bullet(tempVec,"WaveBullet",( ( (BASEDAMAGEPERSECOND*(WAVECOOLDOWN/60.0)) / (numberOfBullets/4) )* DAMAGEMULTPERRANK * DAMAGEMULTSTRONG ) / 3);
				tempVec.fX = WAVE_SPEED4*cos(angle);
				tempVec.fY = -WAVE_SPEED4*sin(angle);
				angle += (2*WAVETHETA)/(numberOfBullets-1);
				orxObject_SetSpeed(tempBullet->bullet, &tempVec);
				
			}
			counter[24] = WAVESEPERATION;
			counter[25] = WAVESEPERATION;
			counter[26] = WAVESEPERATION;
			counter[27] = WAVESEPERATION;
			counter[28] = WAVESEPERATION;
			counter[29] = WAVESEPERATION;

			currentWave++;
		}else if(currentWave == 2){
			for(int i = 0; i < numberOfBullets; i++){
				tempVec.fX = (WAVERADIUS*cos(angle))+playerPos.fX+32; // Add circle center
				tempVec.fY = -(WAVERADIUS*sin(angle))+playerPos.fY;

				tempBullet = new Bullet(tempVec,"WaveBullet",( ( (BASEDAMAGEPERSECOND*(WAVECOOLDOWN/60.0)) / (numberOfBullets/4) )* DAMAGEMULTPERRANK * DAMAGEMULTSTRONG ) / 3);
				tempVec.fX = WAVE_SPEED5*cos(angle);
				tempVec.fY = -WAVE_SPEED5*sin(angle);
				angle += (2*WAVETHETA)/(numberOfBullets-1);
				orxObject_SetSpeed(tempBullet->bullet, &tempVec);
				
			}
			counter[24] = WAVECOOLDOWN;
			counter[25] = WAVECOOLDOWN;
			counter[26] = WAVECOOLDOWN;
			counter[27] = WAVECOOLDOWN;
			counter[28] = WAVECOOLDOWN;
			counter[29] = WAVECOOLDOWN;

			currentWave = 0;
		}else{
			currentWave = 0;
		}
		break;
	case 28: // Wave: Rank 3 - Standard
		numberOfBullets = 50; // For two waves
		angle = (3*3.14159265)/2 -  WAVETHETA;
		if(currentWave == 0){
			for(int i = 0; i < numberOfBullets; i++){
				tempVec.fX = (WAVERADIUS*cos(angle))+playerPos.fX+32; // Add circle center
				tempVec.fY = -(WAVERADIUS*sin(angle))+playerPos.fY;

				tempBullet = new Bullet(tempVec,"WaveBullet",( ( (BASEDAMAGEPERSECOND*(WAVECOOLDOWN/60.0)) / (numberOfBullets/4) )* DAMAGEMULTPERRANK * DAMAGEMULTPERRANK ) / 3);
				tempVec.fX = WAVE_SPEED3*cos(angle);
				tempVec.fY = -WAVE_SPEED3*sin(angle);
				angle += (2*WAVETHETA)/(numberOfBullets-1);
				orxObject_SetSpeed(tempBullet->bullet, &tempVec);
				
				
			}
			counter[24] = WAVESEPERATION;
			counter[25] = WAVESEPERATION;
			counter[26] = WAVESEPERATION;
			counter[27] = WAVESEPERATION;
			counter[28] = WAVESEPERATION;
			counter[29] = WAVESEPERATION;
			currentWave++;
		}else if(currentWave == 1){
			for(int i = 0; i < numberOfBullets; i++){
				tempVec.fX = (WAVERADIUS*cos(angle))+playerPos.fX+32; // Add circle center
				tempVec.fY = -(WAVERADIUS*sin(angle))+playerPos.fY;

				tempBullet = new Bullet(tempVec,"WaveBullet",( ( (BASEDAMAGEPERSECOND*(WAVECOOLDOWN/60.0)) / (numberOfBullets/4) )* DAMAGEMULTPERRANK * DAMAGEMULTPERRANK ) / 3);
				tempVec.fX = WAVE_SPEED4*cos(angle);
				tempVec.fY = -WAVE_SPEED4*sin(angle);
				angle += (2*WAVETHETA)/(numberOfBullets-1);
				orxObject_SetSpeed(tempBullet->bullet, &tempVec);
				
			}
			counter[24] = WAVESEPERATION;
			counter[25] = WAVESEPERATION;
			counter[26] = WAVESEPERATION;
			counter[27] = WAVESEPERATION;
			counter[28] = WAVESEPERATION;
			counter[29] = WAVESEPERATION;

			currentWave++;
		}else if(currentWave == 2){
			for(int i = 0; i < numberOfBullets; i++){
				tempVec.fX = (WAVERADIUS*cos(angle))+playerPos.fX+32; // Add circle center
				tempVec.fY = -(WAVERADIUS*sin(angle))+playerPos.fY;

				tempBullet = new Bullet(tempVec,"WaveBullet",( ( (BASEDAMAGEPERSECOND*(WAVECOOLDOWN/60.0)) / (numberOfBullets/4) )* DAMAGEMULTPERRANK * DAMAGEMULTPERRANK ) / 3);
				tempVec.fX = WAVE_SPEED5*cos(angle);
				tempVec.fY = -WAVE_SPEED5*sin(angle);
				angle += (2*WAVETHETA)/(numberOfBullets-1);
				orxObject_SetSpeed(tempBullet->bullet, &tempVec);
				
			}
			counter[24] = WAVECOOLDOWN;
			counter[25] = WAVECOOLDOWN;
			counter[26] = WAVECOOLDOWN;
			counter[27] = WAVECOOLDOWN;
			counter[28] = WAVECOOLDOWN;
			counter[29] = WAVECOOLDOWN;

			currentWave = 0;
		}else{
			currentWave = 0;
		}
		break;
	case 29: // Wave: Rank 3 - Strong
		numberOfBullets = 50; // For two waves
		angle = (3*3.14159265)/2 -  WAVETHETA;
		if(currentWave == 0){
			for(int i = 0; i < numberOfBullets; i++){
				tempVec.fX = (WAVERADIUS*cos(angle))+playerPos.fX+32; // Add circle center
				tempVec.fY = -(WAVERADIUS*sin(angle))+playerPos.fY;

				tempBullet = new Bullet(tempVec,"WaveBullet",( ( (BASEDAMAGEPERSECOND*(WAVECOOLDOWN/60.0)) / (numberOfBullets/4) )* DAMAGEMULTPERRANK * DAMAGEMULTPERRANK * DAMAGEMULTSTRONG ) / 4);
				tempVec.fX = WAVE_SPEED2*cos(angle);
				tempVec.fY = -WAVE_SPEED2*sin(angle);
				angle += (2*WAVETHETA)/(numberOfBullets-1);
				orxObject_SetSpeed(tempBullet->bullet, &tempVec);
				
				
			}
			counter[24] = WAVESEPERATION;
			counter[25] = WAVESEPERATION;
			counter[26] = WAVESEPERATION;
			counter[27] = WAVESEPERATION;
			counter[28] = WAVESEPERATION;
			counter[29] = WAVESEPERATION;
			currentWave++;
		}else if(currentWave == 1){
			for(int i = 0; i < numberOfBullets; i++){
				tempVec.fX = (WAVERADIUS*cos(angle))+playerPos.fX+32; // Add circle center
				tempVec.fY = -(WAVERADIUS*sin(angle))+playerPos.fY;

				tempBullet = new Bullet(tempVec,"WaveBullet",( ( (BASEDAMAGEPERSECOND*(WAVECOOLDOWN/60.0)) / (numberOfBullets/4) )* DAMAGEMULTPERRANK * DAMAGEMULTPERRANK * DAMAGEMULTSTRONG) / 4);
				tempVec.fX = WAVE_SPEED3*cos(angle);
				tempVec.fY = -WAVE_SPEED3*sin(angle);
				angle += (2*WAVETHETA)/(numberOfBullets-1);
				orxObject_SetSpeed(tempBullet->bullet, &tempVec);
				
			}
			counter[24] = WAVESEPERATION;
			counter[25] = WAVESEPERATION;
			counter[26] = WAVESEPERATION;
			counter[27] = WAVESEPERATION;
			counter[28] = WAVESEPERATION;
			counter[29] = WAVESEPERATION;

			currentWave++;
		}else if(currentWave == 2){
			for(int i = 0; i < numberOfBullets; i++){
				tempVec.fX = (WAVERADIUS*cos(angle))+playerPos.fX+32; // Add circle center
				tempVec.fY = -(WAVERADIUS*sin(angle))+playerPos.fY;

				tempBullet = new Bullet(tempVec,"WaveBullet",( ( (BASEDAMAGEPERSECOND*(WAVECOOLDOWN/60.0)) / (numberOfBullets/4) )* DAMAGEMULTPERRANK * DAMAGEMULTPERRANK * DAMAGEMULTSTRONG) / 4);
				tempVec.fX = WAVE_SPEED4*cos(angle);
				tempVec.fY = -WAVE_SPEED4*sin(angle);
				angle += (2*WAVETHETA)/(numberOfBullets-1);
				orxObject_SetSpeed(tempBullet->bullet, &tempVec);
				
			}
			counter[24] = WAVESEPERATION;
			counter[25] = WAVESEPERATION;
			counter[26] = WAVESEPERATION;
			counter[27] = WAVESEPERATION;
			counter[28] = WAVESEPERATION;
			counter[29] = WAVESEPERATION;

			currentWave++;
		}else if(currentWave == 3){
			for(int i = 0; i < numberOfBullets; i++){
				tempVec.fX = (WAVERADIUS*cos(angle))+playerPos.fX+32; // Add circle center
				tempVec.fY = -(WAVERADIUS*sin(angle))+playerPos.fY;

				tempBullet = new Bullet(tempVec,"WaveBullet",( ( (BASEDAMAGEPERSECOND*(WAVECOOLDOWN/60.0)) / (numberOfBullets/4) )* DAMAGEMULTPERRANK * DAMAGEMULTPERRANK * DAMAGEMULTSTRONG) / 4);
				tempVec.fX = WAVE_SPEED5*cos(angle);
				tempVec.fY = -WAVE_SPEED5*sin(angle);
				angle += (2*WAVETHETA)/(numberOfBullets-1);
				orxObject_SetSpeed(tempBullet->bullet, &tempVec);
				
			}
			counter[24] = WAVECOOLDOWN;
			counter[25] = WAVECOOLDOWN;
			counter[26] = WAVECOOLDOWN;
			counter[27] = WAVECOOLDOWN;
			counter[28] = WAVECOOLDOWN;
			counter[29] = WAVECOOLDOWN;

			currentWave = 0;
		}
		break;
	}

}



float PlayerWeapon::randFloat(int start, int end){
	return end - (float)rand()/((float)RAND_MAX/(end - start));
}

void PlayerWeapon::saveInventory(){
	for(int i = 0; i < NUMBER_OF_PLAYER_WEAPONS; i++){
		savedUnlocks[i] = unlocks[i];
	}
}

void PlayerWeapon::resetInventory(){
	for(int i = 0; i < NUMBER_OF_PLAYER_WEAPONS; i++){
		unlocks[i] = savedUnlocks[i];
	}
}

void PlayerWeapon::changeWeapon(int newWeapon){
	if(newWeapon == CHANGE_NEXT_WEAPON){
		newWeapon=(currentWeapon+1)%NUMBER_OF_PLAYER_WEAPONS;
		if(newWeapon == 1)
			newWeapon = 2;
		while(unlocks[newWeapon]!=true){
			newWeapon=(newWeapon+1)%NUMBER_OF_PLAYER_WEAPONS;
		}
		currentWeapon = (player_weapon_index)newWeapon;
	}
	else if(newWeapon == CHANGE_PREV_WEAPON){
		newWeapon=currentWeapon-1;
		if(newWeapon == 1)
			newWeapon = 0;
		if(newWeapon<0){
			newWeapon=NUMBER_OF_PLAYER_WEAPONS-1;
		}
		while(unlocks[newWeapon]!=true){
			newWeapon=newWeapon-1;
			if(newWeapon<0){
				newWeapon=NUMBER_OF_PLAYER_WEAPONS-1;
			}
		}
		currentWeapon = (player_weapon_index)newWeapon;
	}
	else if(newWeapon<NUMBER_OF_PLAYER_WEAPONS && unlocks[newWeapon]){
		currentWeapon = (player_weapon_index)newWeapon;
	}
	
}

void PlayerWeapon::unlockWeapon(int weapon){
	unlocks[weapon] = true;
}

void PlayerWeapon::upgradeWeapon(int ranks){
	rank+=ranks;
	if(rank<0){
		rank=0;
		currentCrystals=0;
	}
	else if(rank>=MAX_WEAPON_RANK){
		rank=MAX_WEAPON_RANK;
		currentCrystals=0;
	}
	ss.str("");
	ss<<(rank+1);
	orxObject_SetTextString(playerWeaponRankText, ss.str().c_str());
	moveCrystalUI();
}

void PlayerWeapon::changeCrystalCount(int crystals){
	if(rank<MAX_WEAPON_RANK || crystals<0){
		currentCrystals+=crystals;
	}
	if(currentCrystals>=CRYSTALS_TO_RANK){
		upgradeWeapon(currentCrystals/CRYSTALS_TO_RANK);
		currentCrystals=currentCrystals%CRYSTALS_TO_RANK;
	}
	else if(currentCrystals<0){
		upgradeWeapon((currentCrystals/CRYSTALS_TO_RANK)-1);
		if(currentCrystals!=0){
			currentCrystals=currentCrystals+((currentCrystals/CRYSTALS_TO_RANK)-1)*(-CRYSTALS_TO_RANK);
		}
	}
	moveCrystalUI();
}

void PlayerWeapon::moveCrystalUI(){
	//finds the position to put the bar at
	orxVECTOR pos;
	orxObject_GetPosition(uiCrystalBar, &pos);
	pos.fX=((LENGTH_OF_UI_CRYSTAL_BAR*1.0)/(CRYSTALS_TO_RANK))*currentCrystals+UI_CRYSTAL_BAR_START_COOR;

	orxObject_SetPosition(uiCrystalBar,&pos);

	ss.str("");
	ss<<currentCrystals;
	orxObject_SetTextString(playerCrystalTotalText,ss.str().c_str());
}

int PlayerWeapon::getRank(){
	return rank;
}

bool PlayerWeapon::isUnlocked(int num){
	return unlocks[num];
}

player_weapon_index PlayerWeapon::getCurrentWeapon(){
	return currentWeapon;
}