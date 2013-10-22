#include "Pattern.h"
#include "Bullet.h"
#include "Player.h"
#include <iostream>

using namespace std;

//Handles firing weapons

Pattern::Pattern()
{
	pat = 0; // If a pattern is not specified later, there is no firing
}

orxSTATUS Pattern::setPat(int name){ //get an enum
	pat = name;
	return orxSTATUS_SUCCESS;
}

orxSTATUS Pattern::fire(orxVECTOR pos, orxVECTOR playerPos){
	if(!GameSystem::getFireFlag()){
		return orxSTATUS_SUCCESS;
	}
	
	switch (pat){
		orxVECTOR t;
		orxVECTOR angle;
		Bullet *temp;
		double step;

	case 10:
		//straight down
		t = pos;
		temp = new Bullet(t, "blue",1);
		//t.fX = sin((double)r*(rand()%100))*200;
		t.fX =0;
		t.fY = 100;
		temp->SetSpeed(t);
		break;

	case 11: //aimed shots
		t = pos;
		temp = new Bullet(t,"blue",1);
		//angle = playerPos - t;
		playerPos.fX+=32;
		playerPos.fY+=32;
		orxVector_Sub(&angle,&playerPos,&t);
		orxVector_Normalize(&angle,&angle);
		orxVector_Mulf(&angle,&angle,200);
		temp->SetSpeed(angle);
		break;

	case 12: //aimed shots only within a certain range
		t = pos;
		playerPos.fX+=32;
		playerPos.fY+=32;
		if(orxVector_GetDistance(&playerPos,&t)<500){
			temp = new Bullet(t,"purple",1);			
			orxVector_Sub(&angle,&playerPos,&t);
			orxVector_Normalize(&angle,&angle);
			orxVector_Mulf(&angle,&angle,500);
			temp->SetSpeed(angle);
		}
		break;

	case 13: //badly aimed shots
		t = pos;
		temp = new Bullet(t,"purple",1);
		playerPos.fX+=32;
		playerPos.fY+=32;
		//add jitter to x and y before subtraction
		
		orxVector_Sub(&angle,&playerPos,&t);
		orxVector_Normalize(&angle,&angle);
		if(rand()%100<50){
			angle.fX+=sin((float)(rand()%100)/500.0);
		}else{
			angle.fX-=sin((float)(rand()%100)/500.0);
		}
		if(rand()%100<50){
			angle.fY+=sin((float)(rand()%100)/500.0);
		}else{
			angle.fY-=sin((float)(rand()%100)/500.0);
		}
		orxVector_Mulf(&angle,&angle,250);
		temp->SetSpeed(angle);

		break;

	case 14: //lasers? 
		t = pos;
		if(abs(playerPos.fX-t.fX)<50||abs(playerPos.fX+t.fX)<50){ //only shoot when player below
		
			temp = new Bullet(t, "laserb",1);
			angle = pos;
			angle.fX -= 16;
			angle.fY -= 350;
			temp->SetPos(angle);
			t.fX =0;
			t.fY = 1500;
			temp->SetSpeed(t);
		}

		break;

	case 15://trishot - triangle \|/
		t = pos;
		t.fY +=10;
		//t.fX +=32;
		temp = new Bullet(t, "green",1);
		t.fX = -50;
		t.fY = 250;
		//temp->Spin(-4.0f);
		//temp->SetRotation(rand()%360);
		temp->SetSpeed(t);

		t = pos;
		t.fY +=10;
		//t.fX +=32;
		temp = new Bullet(t, "green",1);
		t.fX = 0;
		t.fY = 250;
		//temp->Spin(-4.0f);
		//temp->SetRotation(rand()%360);
		temp->SetSpeed(t);

		t = pos;
		t.fY +=10;
		//t.fX +=32;
		temp = new Bullet(t, "green",1);
		t.fX = 50;
		t.fY = 250;
		//temp->Spin(-4.0f);
		//temp->SetRotation(rand()%360);
		temp->SetSpeed(t);
		break;

	case 16:
		//shoot in a big circle at once
		//{cos(x),sin(x),0} for circle
		step = 0;
		t = pos;
		angle.fZ = 0;

		while(step<=(1.1*3.14159)){
			
			temp = new Bullet(t,"green",1);

			angle.fY = sin(step); //the math steps for unit circle
			angle.fX = cos(step);
			step+=(2*3.14159)/8.0;

			orxVector_Mulf(&angle,&angle,300);
			temp->SetSpeed(angle);
			
		}
		break;

	case 17://homing bullets- make a unit that follows player

		break;

		case 18: //badly aimed shots
		t = pos;
		temp = new Bullet(t,"purple",1);
		playerPos.fX+=32;
		playerPos.fY+=32;
		//add jitter to x and y before subtraction
		
		orxVector_Sub(&angle,&playerPos,&t);
		orxVector_Normalize(&angle,&angle);
		if(rand()%100<50){
			angle.fX+=sin((float)(rand()%200)/300.0);
		}else{
			angle.fX-=sin((float)(rand()%200)/300.0);
		}
		if(rand()%100<50){
			angle.fY+=sin((float)(rand()%200)/300.0);
		}else{
			angle.fY-=sin((float)(rand()%200)/300.0);
		}
		orxVector_Mulf(&angle,&angle,150);
		temp->SetSpeed(angle);

		break;


	default:
		
		break;
	}

	return orxSTATUS_SUCCESS;
}

orxSTATUS Pattern::fire(orxVECTOR pos, orxVECTOR playerPos, int time){
	if(!GameSystem::getFireFlag()){
		return orxSTATUS_SUCCESS;
	}
	switch (pat){
		orxVECTOR t;
		orxVECTOR angle;
		Bullet *temp;
		double step;

	case 10:
		{
		//straight down
		t = pos;
		temp = new Bullet(t, "blue",1);
		//t.fX = sin((double)r*(rand()%100))*200;
		t.fX =0;
		t.fY = 100;
		temp->SetSpeed(t);
		}
		break;

	case 11: //aimed shots
		t = pos;
		temp = new Bullet(t,"blue",1);
		//angle = playerPos - t;
		playerPos.fX+=32;
		playerPos.fY+=32;
		orxVector_Sub(&angle,&playerPos,&t);
		orxVector_Normalize(&angle,&angle);
		orxVector_Mulf(&angle,&angle,200);
		temp->SetSpeed(angle);
		break;

	case 12: //aimed shots only within a certain range
		t = pos;
		playerPos.fX+=32;
		playerPos.fY+=32;
		if(orxVector_GetDistance(&playerPos,&t)<500){
			temp = new Bullet(t,"purple",1);			
			orxVector_Sub(&angle,&playerPos,&t);
			orxVector_Normalize(&angle,&angle);
			orxVector_Mulf(&angle,&angle,500);
			temp->SetSpeed(angle);
		}
		break;

	case 13: //badly aimed shots
		t = pos;
		temp = new Bullet(t,"purple",1);
		playerPos.fX+=32;
		playerPos.fY+=32;
		//add jitter to x and y before subtraction
		
		orxVector_Sub(&angle,&playerPos,&t);
		orxVector_Normalize(&angle,&angle);
		if(rand()%100<50){
			angle.fX+=sin((float)(rand()%100)/500.0);
		}else{
			angle.fX-=sin((float)(rand()%100)/500.0);
		}
		if(rand()%100<50){
			angle.fY+=sin((float)(rand()%100)/500.0);
		}else{
			angle.fY-=sin((float)(rand()%100)/500.0);
		}
		orxVector_Mulf(&angle,&angle,250);
		temp->SetSpeed(angle);

		break;

	case 14: //lasers? 
		t = pos;
		if(abs(playerPos.fX-t.fX)<50||abs(playerPos.fX+t.fX)<50){ //only shoot when player below
		
			temp = new Bullet(t, "laserb",1);
			angle = pos;
			angle.fX -= 16;
			angle.fY -= 350;
			temp->SetPos(angle);
			t.fX =0;
			t.fY = 1500;
			temp->SetSpeed(t);
		}

		break;

	case 15://trishot - triangle \|/
		t = pos;
		t.fY +=10;
		//t.fX +=32;
		temp = new Bullet(t, "green",1);
		t.fX = -50;
		t.fY = 250;
		//temp->Spin(-4.0f);
		//temp->SetRotation(rand()%360);
		temp->SetSpeed(t);

		t = pos;
		t.fY +=10;
		//t.fX +=32;
		temp = new Bullet(t, "green",1);
		t.fX = 0;
		t.fY = 250;
		//temp->Spin(-4.0f);
		//temp->SetRotation(rand()%360);
		temp->SetSpeed(t);

		t = pos;
		t.fY +=10;
		//t.fX +=32;
		temp = new Bullet(t, "green",1);
		t.fX = 50;
		t.fY = 250;
		//temp->Spin(-4.0f);
		//temp->SetRotation(rand()%360);
		temp->SetSpeed(t);
		break;

	case 16:
		//shoot in a big circle at once
		//{cos(x),sin(x),0} for circle
		step = 0;
		t = pos;
		angle.fZ = 0;

		while(step<=(2*3.14159)){
			
			temp = new Bullet(t,"green",1);

			angle.fY = sin(step); //the math steps for unit circle
			angle.fX = cos(step);
			step+=(2*3.14159)/20;

			orxVector_Mulf(&angle,&angle,200);
			temp->SetSpeed(angle);
			
		}
		break;

	case 17://spiral - need to use time
		step = time; //this isn't going to like ints very well
		t = pos;
		step = (double) step*0.05;
		angle.fZ = 0;
		temp = new Bullet(t,"green",1);
		angle.fY = sin(step); //the math steps for unit circle
		angle.fX = cos(step);
		//step+=(2*3.14159)/20;
		orxVector_Mulf(&angle,&angle,200);
		temp->SetSpeed(angle);

		//shoot opposite direction too
		temp = new Bullet(t,"green",1);
		angle.fY = -1*sin(step); //the math steps for unit circle
		angle.fX = -1*cos(step);
		//step+=(2*3.14159)/20;
		orxVector_Mulf(&angle,&angle,200);
		temp->SetSpeed(angle);

		break;

	case 18: //badly aimed shots
		t = pos;
		temp = new Bullet(t,"purple",1);
		playerPos.fX+=32;
		playerPos.fY+=32;
		//add jitter to x and y before subtraction
		
		orxVector_Sub(&angle,&playerPos,&t);
		orxVector_Normalize(&angle,&angle);
		if(rand()%100<50){
			angle.fX+=sin((float)(rand()%100)/500.0);
		}else{
			angle.fX-=sin((float)(rand()%100)/500.0);
		}
		if(rand()%100<50){
			angle.fY+=sin((float)(rand()%100)/500.0);
		}else{
			angle.fY-=sin((float)(rand()%100)/500.0);
		}
		orxVector_Mulf(&angle,&angle,150);
		temp->SetSpeed(angle);

		break;
	default:
		
		break;
	}

	return orxSTATUS_SUCCESS;
}