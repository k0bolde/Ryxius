#include "LevelSystem.h"
#include <iostream>
using namespace std;
#include <fstream>

orxOBJECT *score;  //SCORE
orxOBJECT* LevelName; // SCORE
string levelNameForHS;
orxOBJECT* multText;

orxSOUND* moleskinsoft;
orxSOUND* currentMusic;
orxSOUND* nextMusic;
orxSOUND* spazz;
orxSOUND* ave;
orxSOUND *crimes, *sapper, *dvd, *fff, *air, *faena, *quick, *scan;

//@ mpEV- This vector of map events should be ordered in the order in which the events occur, with the smallest
//        frame condition being first (frame 60 -> frame 64 -> frame 102 -> ect.). The specifications for the
//        events are below. All events must specify a time in variable "time".
//
//        CHANGEMAPSPEED (Event 0)
//        Changes the scrolling of the map to the speed specified in iValue1 (lowest background), iValue2 (mid background),
//        and iValue3 (closest or highest background). This event should ideally be the first event in a level to enable
//        scrolling the background (otherwise the background will not move).
//
//        iValue1 - Scrolling speed of Lowest/Farthest background
//        iValue2 - Scrolling speed of Mid-range background
//        iValue3 - Scrolling speed of Highest/Closest background
//
//        CREATEENEMY (Event 1)
//        Creates an enemy with the specifications given in the value fields.
//        
//        Temporary Values
//        iValue1 - X position
//        iValue2 - Y position
//        iValue3 - HP of Enemy
//        iValue4 - Score Value of Enemy
//        sValue  - Name of enemy to spawn
//		  sValue2 - path of enemy
//
//        ENDLEVEL (Event 2)
//
//        Ends the current level and loads the next level, if any. This can also be done by calling the EndLevel method.
//        
//@ mpE - This vector of strings holds the name of the elements (specified in the .ini's) that refer to the
//        background element. The vector should be ordered in order of displaying (having the first background
//        at the 0th element in the vector).
LevelSystem::LevelSystem(GameSystem* m){
	offscreen = new orxVECTOR();
	offscreen->fX = -1501;
	offscreen->fY = 0;
	offscreen->fZ = 1.0;
	mapLoaded = false;
	m_Instance = m;
	//===========================
	currentLevel = 2;
	//=============================
	enemiesCreated=0;
	enemiesKilled=0;
	scoreAtStart=0;
	endLevelHeaderObj=NULL;
	endLevelEnemiesDestroyedObj=NULL;
	endLevelScoreEarnedObj=NULL;
	endLevelTotalScoreObj=NULL;
	endLevelContinueTextObj=NULL;
	waitNextLevelFlag=false;

	//Create UI =========================================
	orxObject_CreateFromConfig("ui");
	orxVECTOR asdf = {-15,325,0};
	score = orxObject_CreateFromConfig("score");
	hiscore = 0;
	//orxObject_SetPosition(score, &asdf);
	orxCOLOR black;
	orxVECTOR bv = {0,0,0};
	black.vRGB = bv;
	black.fAlpha = 1.0;
	orxObject_SetColor(score, &black);
	LevelName = orxObject_CreateFromConfig("LevelName");
	levelNameForHS = "";
	orxObject_SetColor(LevelName, &black);
	orxObject_CreateFromConfig("uiCrystalBarBackground");
	//End Create UI =====================================

	//Create multiplier text
	multText = orxObject_CreateFromConfig("Points");
	orxVECTOR multPos;
	//orxObject_GetPosition(score, &multPos);
	//cout<<multPos.fX<<" "<<multPos.fY<<" "<<multPos.fZ<<" "<<endl;
	multPos.fY = 326;
	multPos.fX = -90;
	multPos.fZ = -.6;
	orxCOLOR white;
	orxVECTOR w = {0,0,0};
	white.vRGB = w;
	white.fAlpha = 1;
	orxObject_SetColor(multText, &white);
	orxObject_SetPosition(multText, &multPos);

	//Create Music Objects ==============================
	
	moleskinsoft = orxSound_CreateFromConfig("Moleskin");
	spazz = orxSound_CreateFromConfig("spazz");
	ave = orxSound_CreateFromConfig("ave");
	crimes = orxSound_CreateFromConfig("crimes");
	sapper =  orxSound_CreateFromConfig("sapper");
	dvd =  orxSound_CreateFromConfig("dvd");
	fff =  orxSound_CreateFromConfig("fff");
	faena =  orxSound_CreateFromConfig("faena");
	air = orxSound_CreateFromConfig("air");
	scan = orxSound_CreateFromConfig("scan"); //3-2 
	quick = orxSound_CreateFromConfig("quick"); //replace moleskin

	// EMD Create Music Objects =========================

	// Create hotbar
	orxOBJECT* gatlingHot = orxObject_CreateFromConfig("GatlingHot");
	orxOBJECT* missileHot = orxObject_CreateFromConfig("MissileHot");
	orxOBJECT* laserHot = orxObject_CreateFromConfig("LaserHot");
	orxOBJECT* shotgunHot = orxObject_CreateFromConfig("ShotgunHot");
	orxOBJECT* waveHot = orxObject_CreateFromConfig("WaveHot");
	bronzeback = orxObject_CreateFromConfig("HotBarBackground1");
	silverback = orxObject_CreateFromConfig("HotBarBackground2");
	goldback = orxObject_CreateFromConfig("HotBarBackground3");
	currentback = bronzeback;
	orxVECTOR vect;
	orxObject_GetPosition(currentback, &vect);
	vect.fZ = -.59;
	orxVECTOR zero;
	zero.fZ = 0;
	orxObject_SetPosition(currentback, &vect);
	
	//store for deletion
	hotbar.push_back(gatlingHot);
	hotbar.push_back(laserHot);
	hotbar.push_back(shotgunHot);
	hotbar.push_back(missileHot);
	hotbar.push_back(waveHot);
	hotbar.push_back(bronzeback); //5
	hotbar.push_back(silverback); //6
	hotbar.push_back(goldback); //7
	
	setHotbar(0);

	// end create hotbar

	highLoadLevel();
}

void LevelSystem::setHotbar(int wep){
	if(m_Instance->getWeaponUnlock(0)){
		orxObject_Delete(hotbar.at(0));
		if(wep == 0)
			hotbar.at(0) = orxObject_CreateFromConfig("GatlingSelect");
		else
			hotbar.at(0) = orxObject_CreateFromConfig("GatlingHotColor");
	}
	/*if(m_Instance->getWeaponUnlock(1)){ // No laser support
		orxObject_Delete(hotbar.at(1));
		hotbar.at(1) = orxObject_CreateFromConfig("LaserHotColor");
	}*/
	if(m_Instance->getWeaponUnlock(2)){
		orxObject_Delete(hotbar.at(2));
		if(wep == 2)
			hotbar.at(2) = orxObject_CreateFromConfig("ShotgunSelect");
		else
			hotbar.at(2) = orxObject_CreateFromConfig("ShotgunHotColor");
	}
	if(m_Instance->getWeaponUnlock(3)){
		orxObject_Delete(hotbar.at(3));
		if(wep == 3)
			hotbar.at(3) = orxObject_CreateFromConfig("MissileSelect");
		else
			hotbar.at(3) = orxObject_CreateFromConfig("MissileHotColor");
	}
	if(m_Instance->getWeaponUnlock(4)){
		orxObject_Delete(hotbar.at(4));
		if(wep == 4)
			hotbar.at(4) = orxObject_CreateFromConfig("WaveSelect");
		else
			hotbar.at(4) = orxObject_CreateFromConfig("WaveHotColor");
	}
}

LevelSystem::~LevelSystem(){

	//Stop whatever music is playing
	orxSound_Stop(currentMusic);
	
	

	//Clear and Reset ======================================================
	m_Instance->setFireFlag(false);
	if(!mapLoaded){
		mapElements = new vector<orxOBJECT*>;
	}else{
		for(int i = 0; i < mapElements->size(); i++){
			orxObject_Delete(mapElements->at(i));
		}
		mapElements->clear();
		m_Instance->clearUnits();
		m_Instance->clearPowerups();
		m_Instance->clearBullets();
	}

	//clearing out the hotbar
	for(int i = 0; i < hotbar.size(); i++){
		orxObject_Delete(hotbar.at(i));
	}
	hotbar.clear();

	//m_Instance->clearPlayer(); //This also works for if the player dies
	currentFrame = 0;
	mapSpeed = 0;
	eventPosition = 0;
	enemiesCreated=0;
	//END Clear and Reset ==================================================

	//TODO Delete the #$^% out of the UI
	waitNextLevelFlag=false;
	endOfLevel = false; //hii
	
	//Create UI =========================================
	//orxObject_CreateFromConfig("ui");
	//cout<<"Deleting ui"<<endl;
	if (m_Instance->GetObjectByName("ui") != orxNULL)
		orxObject_Delete(m_Instance->GetObjectByName("ui"));
	//score = orxObject_CreateFromConfig("score");
	//cout<<"Deleting score"<<endl;
	orxObject_Delete(score);
	//hiscore = 0;
	//orxObject_SetPosition(score, &asdf);
	//orxCOLOR black;
	//orxVECTOR bv = {0,0,0};
	//black.vRGB = bv;
	//black.fAlpha = 1.0;
	//orxObject_SetColor(score, &black);
	//LevelName = orxObject_CreateFromConfig("LevelName");
	//cout<<"Deleting lvlName"<<endl;
	orxObject_Delete(LevelName);
	//orxObject_SetColor(LevelName, &black);
	//orxObject_CreateFromConfig("uiCrystalBarBackground");
	//cout<<"Deleting uicrystalbarbackground"<<endl;
	if (m_Instance->GetObjectByName("uiCrystalBarBackground") != orxNULL)
		orxObject_Delete(m_Instance->GetObjectByName("uiCrystalBarBackground"));
	//End Create UI =====================================

	//Create multiplier text
	//cout<<"Deleting points"<<endl;
	//multText = orxObject_CreateFromConfig("Points");
	orxObject_Delete(multText);
	//orxVECTOR multPos;
	//orxObject_GetPosition(score, &multPos);
	//cout<<multPos.fX<<" "<<multPos.fY<<" "<<multPos.fZ<<" "<<endl;
	//multPos.fY = 326;
	//multPos.fX = -90;
	//multPos.fZ = -.6;
	//orxCOLOR white;
	//orxVECTOR w = {0,0,0};
	//white.vRGB = w;
	//white.fAlpha = 1;
	//orxObject_SetColor(multText, &white);
	//orxObject_SetPosition(multText, &multPos);

	//Create Music Objects ==============================

	deleteEndLevelText();


	//moleskinsoft = orxSound_CreateFromConfig("Music");



	// EMD Create Music Objects =========================



	//highLoadLevel();
	
}


string LevelSystem::getLevelName(){
	return levelNameForHS;
}

//Loads a level and replaces the current level if any is loaded..
void LevelSystem::loadLevel(vector<MapEvent>* mpEV, vector<string>* mpE){
	//Clear and Reset ======================================================
	m_Instance->setFireFlag(true);
	mapEvents = mpEV;
	if(!mapLoaded){
		mapElements = new vector<orxOBJECT*>;
	}else{
		for(int i = 0; i < mapElements->size(); i++){
			orxObject_Delete(mapElements->at(i));
		}
		mapElements->clear();
		m_Instance->clearUnits();
		m_Instance->clearPowerups();
		m_Instance->clearBullets();
	}

	m_Instance->restartPlayer(); //This also works for if the player dies
	currentFrame = 0;
	mapSpeed = 0;
	eventPosition = 0;
	enemiesCreated=0;
	if(currentMusic != 0){
		orxSound_Stop(currentMusic);
	}
	//END Clear and Reset ==================================================


	for(int i = 0; i < mpE->size(); i++){
		mapElements->push_back(orxObject_CreateFromConfig( mpE->at(i).c_str() ));
		if(i > 1){//Set the 3rd and any backgrounds after to be put offscreen
			orxObject_SetPosition(mapElements->at(i),offscreen);
		}
	}


	EventsCompleted = false;
	if(mpEV->empty()){
		EventsCompleted = true;
	}

	orxVECTOR asdf = {0,mapSpeed,0};
	if(mapElements->size() > 1){
		leading = mapElements->at(0);
		trailing = mapElements->at(1);
		emptyMap = false;
	}else if (mapElements->size() == 1){
		leading = mapElements->at(0);
		trailing = mapElements->at(0);
		emptyMap = false;
	}else{
		emptyMap = true;
		leading = 0;
		trailing = 0;
	}
	if(!emptyMap){
		orxVECTOR pos = {-500,-1650,1.0};
		orxObject_SetPosition(trailing,&pos);
		pos.fY = -650;
		orxObject_SetPosition(leading,&pos);
		orxObject_SetSpeed(leading,&asdf);
		orxObject_SetSpeed(trailing,&asdf);
	}
	currentBackground = 1;
	
	currentMusic = nextMusic;
	orxSound_Play(currentMusic);
	mapLoaded = true;
}

void LevelSystem::update(){
	if(waitNextLevelFlag){//if we are waiting for the level to be over check to see if the player has signaled it to end
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_SPACE)){
			waitNextLevelFlag=false;
			endOfLevel = false; //hii
			goToNextLevel();
		}
	}
	else{
		if(mapLoaded){//Only do update if a map is loaded
			//UI Update ========================================================
			ss.str("");
			ss<<hiscore;
			orxObject_SetTextString(score, ss.str().c_str());
			//END UI Update ====================================================

			if(endOfLevel && !m_Instance->getPlayer()->warpAnim){// After warp anim, show stats
				displayEndLevelText();
				waitNextLevelFlag=true;
			}

			//3-3 The End Hack =================================================

			if(currentLevel == 8 && mapElements != 0 ){
				if(leading == mapElements->at(9)){
					orxVECTOR pos;
					orxObject_GetPosition(leading, &pos);
					if(pos.fY >= -550){
						pos.fX = 0;
						pos.fY = 0;
						pos.fZ = 0;
						orxObject_SetSpeed(leading,&pos);
						orxObject_SetSpeed(trailing,&pos);

					}
				}
			}

			//END 3-3 The End Hack =============================================


			//Scrolling Background Update ======================================
			if(!emptyMap){
				orxVECTOR pos;
				orxObject_GetPosition(leading, &pos);

				if(pos.fY > 351.0){ //If leading map goes offscreen...
					orxVECTOR asdf2 = {0,0,0};
					orxObject_SetPosition(leading,offscreen);//Put the map that went offscreen into the "offscreen" position and have it stop
					orxObject_SetSpeed(leading,&asdf2);
					leading = trailing;
					if(currentBackground+1 < mapElements->size()){
						currentBackground++;//Increment background position
					}else{
						currentBackground = 0;//Unless that is the end of the backgrounds, so then loop.
					}
					orxVECTOR asdf = {0,mapSpeed,0};
					trailing = mapElements->at(currentBackground);
					pos.fY -= 2000;
					orxObject_SetPosition(trailing,&pos);
					orxObject_SetSpeed(trailing,&asdf);

				}
			}
			//END Scrolling Background Update ==================================

			//Event Handler ====================================================
			if(!EventsCompleted){ // If there are still events to be done
				bool timeout = true;
				while(timeout){
					if(mapEvents->at(eventPosition).time <= currentFrame){ // If the event time is up, execute the event
						//cout << "Executing Event "<< eventPosition<< endl;
						if(mapEvents->at(eventPosition).command == 0){ // Change map speed event
							mapSpeed = mapEvents->at(eventPosition).iValue1;
							orxVECTOR asdf = {0,mapSpeed,0};
							orxObject_SetSpeed(trailing,&asdf);
							orxObject_SetSpeed(leading,&asdf);

						}
						else if(mapEvents->at(eventPosition).command == 1){ // Create Enemy event
							orxVECTOR asdf = {mapEvents->at(eventPosition).iValue1,mapEvents->at(eventPosition).iValue2};
							orxSTRING orc = (orxCHAR*)mapEvents->at(eventPosition).sValue1.c_str();
							Unit *what = new Unit(asdf,orc,mapEvents->at(eventPosition).iValue3,ENEMY_POINT_VAL,mapEvents->at(eventPosition).weapUnlock);
							enemiesCreated++;
							if(!(mapEvents->at(eventPosition).sValue2==0)){
								pathInfo* path = mapEvents->at(eventPosition).sValue2; //this may cause crash
								what->SetPath(path);
							}
							m_Instance->addUnit(what);
						}else if(mapEvents->at(eventPosition).command == 2){ // End Level event
							if(!GameSystem::getPlayerDeathFlag() && !endOfLevel){
								endLevel();
								break;
							}
						}

						if(eventPosition+1 >= mapEvents->size()){
							EventsCompleted = true;
							timeout = false;
						}else{
							eventPosition++;
						}
					}else{
						timeout = false;
					}
				}
			}
			//END Event Handler ================================================

			currentFrame++;

		}//END MAPLOADED IF
	}
}
//Completes the current level and advances the player 
void LevelSystem::endLevel(){
	//Show some sort of stat screen here or exit anim
	m_Instance->getPlayer()->warpAnim = true;
	
	endOfLevel = true;
	//cout<<"Clearing Units"<<endl;
	m_Instance->clearUnits();
	//cout<<"Units cleared, clearing powerups"<<endl;
	m_Instance->clearPowerups();
	//cout<<"Powerups cleared, saving inventory"<<endl;
	//CLEAR BULLETS HERE
	


	m_Instance->savePlayerInventory();
	//cout<<"Inventory saved"<<endl;
	
}

void LevelSystem::displayEndLevelText(){
	//cout<<"Made it to endleveltext"<<endl;
	//stops the background from moving
	orxVECTOR stopSpeed = {0,0,0};
	if(trailing!=0)	orxObject_SetSpeed(trailing,&stopSpeed);
	if(leading!=0) orxObject_SetSpeed(leading,&stopSpeed);
	//cout<<"Starting endlevel text"<<endl;
	endLevelHeaderObj=orxObject_CreateFromConfig("EndLevelHeader");
	//cout<<"Made header"<<endl;
	//display the number of enemies killed
	endLevelEnemiesDestroyedObj=orxObject_CreateFromConfig("EnemiesDestroyedText");
	//cout<<"Made enemies desroyed text"<<endl;
	ss.str("");
	ss<<ENEMIES_DESTROYED_TEXT<<enemiesKilled<<"/"<<enemiesCreated;
	orxObject_SetTextString(endLevelEnemiesDestroyedObj, ss.str().c_str());

	//display the number of points the player earned this round
	endLevelScoreEarnedObj=orxObject_CreateFromConfig("ScoreEarnedText");
	ss.str("");
	ss<<POINTS_EARNED_TEXT<<(hiscore-scoreAtStart);
	orxObject_SetTextString(endLevelScoreEarnedObj, ss.str().c_str());

	//display the total number of points the player earned
	endLevelTotalScoreObj=orxObject_CreateFromConfig("TotalScoreText");
	ss.str("");
	ss<<TOTAL_POINTS_TEXT<<hiscore;
	orxObject_SetTextString(endLevelTotalScoreObj, ss.str().c_str());
	
	//display text to say how to go to the next level
	endLevelContinueTextObj=orxObject_CreateFromConfig("KeyToContinueText");

	//Edit Global Variable
	//TODO Pass to gamesystem
	//scoreAtStart;
	//TODO pass level to gamesystem
	//LevelName;

	scoreAtStart = hiscore;
}

void LevelSystem::deleteEndLevelText(){
	if(endLevelHeaderObj!=NULL){
		orxObject_Delete(endLevelHeaderObj);
		endLevelHeaderObj=NULL;
	}
	if(endLevelEnemiesDestroyedObj!=NULL){
		orxObject_Delete(endLevelEnemiesDestroyedObj);
		endLevelEnemiesDestroyedObj=NULL;
	}
	if(endLevelScoreEarnedObj!=NULL){
		orxObject_Delete(endLevelScoreEarnedObj);
		endLevelScoreEarnedObj=NULL;
	}
	if(endLevelTotalScoreObj!=NULL){
		orxObject_Delete(endLevelTotalScoreObj);
		endLevelTotalScoreObj=NULL;
	}
	if(endLevelContinueTextObj!=NULL){
		orxObject_Delete(endLevelContinueTextObj);
		endLevelContinueTextObj=NULL;
	}
}

void LevelSystem::modScore(int mod){
	hiscore+=mod;
}

void LevelSystem::enemyKilled(){
	enemiesKilled++;
}

void LevelSystem::goToNextLevel(){
	currentLevel++;
	highLoadLevel();
}

void LevelSystem::setRankBadge(int rank){
	orxVECTOR vect;
	orxObject_GetPosition(bronzeback, &vect);
	vect.fZ = -.59;
	handleRank();

	switch(rank){ 
	case 0: 
		orxObject_SetPosition(bronzeback, &vect);
		break;
	case 1: 
		orxObject_SetPosition(silverback, &vect);
		break;
	case 2: 
		orxObject_SetPosition(goldback, &vect);
	}
}

void LevelSystem::handleRank(){
	orxVECTOR zero;
	orxObject_GetPosition(bronzeback, &zero); //get current pos
	//make 0 zorder vect
	zero.fZ = 0;
	orxObject_SetPosition(bronzeback, &zero); // show back at the right position
	orxObject_SetPosition(silverback, &zero); //hide curr
	orxObject_SetPosition(goldback, &zero);
}

//returns true if the game is waiting for the player to signal to go to the next level
bool LevelSystem::waitingForNextLevel(){
	return waitNextLevelFlag;
}

//Contains information for all levels and has the levelsystem create a level based
//on what currentLevel is.
void LevelSystem::highLoadLevel(){
	enemiesCreated=0;
	enemiesKilled=0;
	hiscore=scoreAtStart;

	deleteEndLevelText();

	switch(currentLevel){

	case 0://Level 1-1
		{
			nextMusic = fff;




			orxObject_SetTextString(LevelName, "1-1 Hopeful Beginnings"); 
			levelNameForHS = "STAGE 1-1";
			vector<string>* tempBackgroundVec = new vector<string>();

			tempBackgroundVec->push_back("filler1");
			tempBackgroundVec->push_back("filler2");
			tempBackgroundVec->push_back("filler5");
			tempBackgroundVec->push_back("filler3");
			tempBackgroundVec->push_back("fillerred");
			tempBackgroundVec->push_back("filler4");
			tempBackgroundVec->push_back("filler2");
			tempBackgroundVec->push_back("filler1");
			tempBackgroundVec->push_back("fillerred");
			tempBackgroundVec->push_back("filler2");
			tempBackgroundVec->push_back("filler4");
			tempBackgroundVec->push_back("filler5");
			tempBackgroundVec->push_back("filler3");
			tempBackgroundVec->push_back("filler1");
			tempBackgroundVec->push_back("fillerred");

			vector<MapEvent>* tempMapEvents = new vector<MapEvent>();
			MapEvent arg0 = {0,0,80,0,0,0,"",0,NO_WEAPON}; //Map Movement Speed
			tempMapEvents->push_back(arg0);
			
			int t =0;
			t+=60;

			for(int i = 0; i < 12; i +=1){
				pathInfo* testPath = genSweep(true);
				MapEvent pathtest = {1,t+i*30,-500,-400,4,20,"peace",testPath,NO_WEAPON};
				tempMapEvents->push_back(pathtest);

			}
			t+=12*30;
			t+=120;

			for(int i = 0; i < 12; i +=1){
				pathInfo* testPath = genSweep(false);
				MapEvent pathtest = {1,t+i*30,500,-400,4,20,"peace",testPath,NO_WEAPON};
				tempMapEvents->push_back(pathtest);

			}
			t+=12*30;
			t+=120;

			for(int i = 0; i < 16; i +=1){
				bool right = true;
				int xPos = -500;
				if(i%2==0){
					right = false;
					xPos*=-1;
				}
				pathInfo* testPath = genSweep(right);
				MapEvent pathtest = {1,t+i*35,xPos,-400,4,20,"peace",testPath,NO_WEAPON};
				tempMapEvents->push_back(pathtest);

			}
			t+=35*16;
			t+=120;

			for(int i = 0; i < 16; i +=1){
				bool right = true;
				int xPos = -500;
				if(rand()%2==0){
					right = false;
					xPos*=-1;
				}
				pathInfo* testPath = genSweep(right);
				MapEvent pathtest = {1,t+i*35,xPos,-400,4,20,"peace",testPath,NO_WEAPON};
				tempMapEvents->push_back(pathtest);

			}
			t+=35*16;
			t+=120;
			
			//invader pattern - not the best...
			{
			pathInfo* testPath = genInvaders(-450,0);
			MapEvent pathtest = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
			testPath = genInvaders(-400+10,0);
			MapEvent pathtest1 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest1);
			testPath = genInvaders(-350+20,0);
			MapEvent pathtest2 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest2);
			testPath = genInvaders(-300+30,0);
			MapEvent pathtest3 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest3);
			}
			//MapEvent endLevelTest = {1,t+120,0,-400,4,20,"topHoard",0,NO_WEAPON};
			t+=70;
			{
			pathInfo* testPath = genInvaders(-450,0);
			MapEvent pathtest = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
			testPath = genInvaders(-400+10,0);
			MapEvent pathtest1 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest1);
			testPath = genInvaders(-350+20,0);
			MapEvent pathtest2 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest2);
			testPath = genInvaders(-300+30,0);
			MapEvent pathtest3 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest3);
			}
			t+=70;
			
			//actual invader formation
			{
			pathInfo* testPath = genInvaders(-450,0);
			MapEvent pathtest = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
			testPath = genInvaders(-400+10,0);
			MapEvent pathtest1 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest1);
			testPath = genInvaders(-350+20,0);
			MapEvent pathtest2 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest2);
			testPath = genInvaders(-300+30,0);
			MapEvent pathtest3 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest3);
			}
			{
			pathInfo* testPath = genInvaders(-450,60);
			MapEvent pathtest = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
			testPath = genInvaders(-400+10,60);
			MapEvent pathtest1 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest1);
			testPath = genInvaders(-350+20,60);
			MapEvent pathtest2 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest2);
			testPath = genInvaders(-300+30,60);
			MapEvent pathtest3 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest3);
			}
			{
			pathInfo* testPath = genInvaders(-450,-60);
			MapEvent pathtest = {1,t,-400,-400,8,20,"peace",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
			testPath = genInvaders(-400+10,-60);
			MapEvent pathtest1 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest1);
			testPath = genInvaders(-350+20,-60);
			MapEvent pathtest2 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest2);
			testPath = genInvaders(-300+30,-60);
			MapEvent pathtest3 = {1,t,-400,-400,8,20,"peace",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest3);
			}
			t+=480;

			{
			pathInfo* testPath = genInvaders(-450,0);
			MapEvent pathtest = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
			testPath = genInvaders(-400+10,0);
			MapEvent pathtest1 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest1);
			testPath = genInvaders(-350+20,0);
			MapEvent pathtest2 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest2);
			testPath = genInvaders(-300+30,0);
			MapEvent pathtest3 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest3);
			}
			{
			pathInfo* testPath = genInvaders(-450,60);
			MapEvent pathtest = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
			testPath = genInvaders(-400+10,60);
			MapEvent pathtest1 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest1);
			testPath = genInvaders(-350+20,60);
			MapEvent pathtest2 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest2);
			testPath = genInvaders(-300+30,60);
			MapEvent pathtest3 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest3);
			}
			{
			pathInfo* testPath = genInvaders(-450,-60);
			MapEvent pathtest = {1,t,-400,-400,8,20,"peace",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
			testPath = genInvaders(-400+10,-60);
			MapEvent pathtest1 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest1);
			testPath = genInvaders(-350+20,-60);
			MapEvent pathtest2 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest2);
			testPath = genInvaders(-300+30,-60);
			MapEvent pathtest3 = {1,t,-400,-400,8,20,"peace",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest3);
			}
			t+=480;
			for(int i =0; i < 40; i +=1){ //cool stream sort of thingy
				pathInfo* testPath = genInvaders(-400,-1*rand()%60);
				MapEvent pathtest = {1,t+i*30,-400,-400,4,20,"hope",testPath,NO_WEAPON};
				tempMapEvents->push_back(pathtest);
			}
			t+=40*30;
			t+=120;
			for(int i =0; i < 20; i +=1){
				MapEvent a = {1,t+i*45,-600,-400,4,20,"love",0,NO_WEAPON};
				tempMapEvents->push_back(a);
				
			}
			t+=20*45;
			t+=120;

			for(int i =0; i < 40; i +=1){ //cool stream sort of thingy
				if(rand()%2==0){
					pathInfo* testPath = genInvaders(-400,-1*rand()%60);
					MapEvent pathtest = {1,t+i*35,-400,-400,4,20,"hope",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest);
				}else{
					bool right = true;
					int xPos = -500;
					if(rand()%2==0){
						right = false;
						xPos*=-1;
					}
					pathInfo* testPath = genSweep(right);
					MapEvent pathtest = {1,t+i*35,xPos,-400,4,20,"peace",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest);
				}
			}
			t+=40*35;

			{
			pathInfo* testPath = genInvaders(-450,0);
			MapEvent pathtest = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
			testPath = genInvaders(-400+10,0);
			MapEvent pathtest1 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest1);
			testPath = genInvaders(-350+20,0);
			MapEvent pathtest2 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest2);
			testPath = genInvaders(-300+30,0);
			MapEvent pathtest3 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest3);
			}
			{
			pathInfo* testPath = genInvaders(-450,60);
			MapEvent pathtest = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
			testPath = genInvaders(-400+10,60);
			MapEvent pathtest1 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest1);
			testPath = genInvaders(-350+20,60);
			MapEvent pathtest2 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest2);
			testPath = genInvaders(-300+30,60);
			MapEvent pathtest3 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest3);
			}
			{
			pathInfo* testPath = genInvaders(-450,-60);
			MapEvent pathtest = {1,t,-400,-400,8,20,"peace",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
			testPath = genInvaders(-400+10,-60);
			MapEvent pathtest1 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest1);
			testPath = genInvaders(-350+20,-60);
			MapEvent pathtest2 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest2);
			testPath = genInvaders(-300+30,-60);
			MapEvent pathtest3 = {1,t,-400,-400,8,20,"peace",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest3);
			}
			t+=480;

			

			//random side guys + invaders
			{
			pathInfo* testPath = genInvaders(-450,0);
			MapEvent pathtest = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
			testPath = genInvaders(-400+10,0);
			MapEvent pathtest1 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest1);
			testPath = genInvaders(-350+20,0);
			MapEvent pathtest2 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest2);
			testPath = genInvaders(-300+30,0);
			MapEvent pathtest3 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest3);
			}
			{
			pathInfo* testPath = genInvaders(-450,60);
			MapEvent pathtest = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
			testPath = genInvaders(-400+10,60);
			MapEvent pathtest1 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest1);
			testPath = genInvaders(-350+20,60);
			MapEvent pathtest2 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest2);
			testPath = genInvaders(-300+30,60);
			MapEvent pathtest3 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest3);
			}
			{
			pathInfo* testPath = genInvaders(-450,-60);
			MapEvent pathtest = {1,t,-400,-400,8,20,"peace",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
			testPath = genInvaders(-400+10,-60);
			MapEvent pathtest1 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest1);
			testPath = genInvaders(-350+20,-60);
			MapEvent pathtest2 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest2);
			testPath = genInvaders(-300+30,-60);
			MapEvent pathtest3 = {1,t,-400,-400,8,20,"peace",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest3);
			}
			for(int i =0; i < 20; i +=1){
				MapEvent a = {1,t+i*45,-600,-400,4,20,"love",0,NO_WEAPON};
				tempMapEvents->push_back(a);
				
			}
			t+=20*45;

			for(int i =0; i < 4; i +=1){
				{
					pathInfo* testPath = genInvaders(-450,0);
					MapEvent pathtest = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest);
					testPath = genInvaders(-400+10,0);
					MapEvent pathtest1 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest1);
					testPath = genInvaders(-350+20,0);
					MapEvent pathtest2 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest2);
					testPath = genInvaders(-300+30,0);
					MapEvent pathtest3 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest3);
				}
				{
					pathInfo* testPath = genInvaders(-450,60);
					MapEvent pathtest = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest);
					testPath = genInvaders(-400+10,60);
					MapEvent pathtest1 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest1);
					testPath = genInvaders(-350+20,60);
					MapEvent pathtest2 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest2);
					testPath = genInvaders(-300+30,60);
					MapEvent pathtest3 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest3);
				}
				{
					pathInfo* testPath = genInvaders(-450,-60);
					MapEvent pathtest = {1,t,-400,-400,16,20,"peace",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest);
					testPath = genInvaders(-400+10,-60);
					MapEvent pathtest1 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest1);
					testPath = genInvaders(-350+20,-60);
					MapEvent pathtest2 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest2);
					testPath = genInvaders(-300+30,-60);
					MapEvent pathtest3 = {1,t,-400,-400,16,20,"peace",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest3);
				}
				t+=240;
			}

			t+=480;
			MapEvent arg19 = {2,t+120,0,0,0,0,"",0,NO_WEAPON};//End Level
			tempMapEvents->push_back(arg19);




			loadLevel(tempMapEvents,tempBackgroundVec);

			break;
		}
	
		case 1:{
			
			nextMusic = dvd;
			



			orxObject_SetTextString(LevelName, "1-2 Love in Death"); 
			levelNameForHS = "STAGE 1-2";
			vector<string>* tempBackgroundVec = new vector<string>();
			tempBackgroundVec->push_back("filler3");
			tempBackgroundVec->push_back("fillerred");
			tempBackgroundVec->push_back("filler4");
			tempBackgroundVec->push_back("filler2");
			tempBackgroundVec->push_back("filler1");
			tempBackgroundVec->push_back("filler4");
			tempBackgroundVec->push_back("filler3");
			tempBackgroundVec->push_back("fillerred");
			tempBackgroundVec->push_back("filler2");
			tempBackgroundVec->push_back("filler3");
			tempBackgroundVec->push_back("filler1");
			tempBackgroundVec->push_back("filler5");
			tempBackgroundVec->push_back("fillerred");
			tempBackgroundVec->push_back("filler2");
			tempBackgroundVec->push_back("filler4");

			vector<MapEvent>* tempMapEvents = new vector<MapEvent>();
			MapEvent arg0 = {0,0,80,0,0,0,"",0,NO_WEAPON}; //Map Movement Speed
			tempMapEvents->push_back(arg0);
			int t = 0;
			//start with a staggered block
			t+=60;
			{
			pathInfo* testPath = genInvaders(-450,0);
			MapEvent pathtest = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
			testPath = genInvaders(-400+10,0);
			MapEvent pathtest1 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest1);
			testPath = genInvaders(-350+20,0);
			MapEvent pathtest2 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest2);
			testPath = genInvaders(-300+30,0);
			MapEvent pathtest3 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest3);
			}
			t+=70;
			{
			pathInfo* testPath = genInvaders(-450,60);
			MapEvent pathtest = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
			testPath = genInvaders(-400+10,60);
			MapEvent pathtest1 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest1);
			testPath = genInvaders(-350+20,60);
			MapEvent pathtest2 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest2);
			testPath = genInvaders(-300+30,60);
			MapEvent pathtest3 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest3);
			}
			t+=70;
			{
			pathInfo* testPath = genInvaders(-450,-60);
			MapEvent pathtest = {1,t,-400,-400,8,20,"peace",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
			testPath = genInvaders(-400+10,-60);
			MapEvent pathtest1 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest1);
			testPath = genInvaders(-350+20,-60);
			MapEvent pathtest2 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest2);
			testPath = genInvaders(-300+30,-60);
			MapEvent pathtest3 = {1,t,-400,-400,8,20,"peace",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest3);
			}
			t+=140;
			//double block
			for(int i =0; i < 2; i +=1){
				{
					pathInfo* testPath = genInvaders(-450,0);
					MapEvent pathtest = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest);
					testPath = genInvaders(-400+10,0);
					MapEvent pathtest1 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest1);
					testPath = genInvaders(-350+20,0);
					MapEvent pathtest2 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest2);
					testPath = genInvaders(-300+30,0);
					MapEvent pathtest3 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest3);
				}
				{
					pathInfo* testPath = genInvaders(-450,60);
					MapEvent pathtest = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest);
					testPath = genInvaders(-400+10,60);
					MapEvent pathtest1 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest1);
					testPath = genInvaders(-350+20,60);
					MapEvent pathtest2 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest2);
					testPath = genInvaders(-300+30,60);
					MapEvent pathtest3 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest3);
				}
				{
					pathInfo* testPath = genInvaders(-450,-60);
					MapEvent pathtest = {1,t,-400,-400,8,20,"peace",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest);
					testPath = genInvaders(-400+10,-60);
					MapEvent pathtest1 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest1);
					testPath = genInvaders(-350+20,-60);
					MapEvent pathtest2 = {1,t,-400,-400,4,20,"hope",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest2);
					testPath = genInvaders(-300+30,-60);
					MapEvent pathtest3 = {1,t,-400,-400,8,20,"peace",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest3);
				}
				t+=240;
			}
			t+=240;
			//random side guys
			for(int i =0; i < 20; i +=1){
				MapEvent a = {1,t+i*45,-600,-400,4,20,"love",0,NO_WEAPON};
				tempMapEvents->push_back(a);
				
			}
			t+=20*45;
			t+=120;
			//sweep guys
			for(int i = 0; i < 16; i +=1){
				bool right = true;
				int xPos = -500;
				if(i%2==0){
					right = false;
					xPos*=-1;
				}
				pathInfo* testPath = genSweep(right);
				MapEvent pathtest = {1,t+i*30,xPos,-400,4,20,"peace",testPath,NO_WEAPON};
				tempMapEvents->push_back(pathtest);

			}
			t+=30*16;
			t+=120;
			//switch it up - sweep aimed bullet guys
			for(int i = 0; i < 16; i +=1){
				bool right = true;
				int xPos = -500;
				if(i%2==0){
					right = false;
					xPos*=-1;
				}
				pathInfo* testPath = genSweep(right);
				MapEvent pathtest = {1,t+i*30,xPos,-400,4,20,"hope",testPath,NO_WEAPON};
				tempMapEvents->push_back(pathtest);

			}
			t+=30*16;
			t+=120;
			for(int i =0; i < 40; i +=1){ //cool stream sort of thingy
				if(rand()%2==0){
					MapEvent a = {1,t+i*45,-600,-400,4,20,"love",0,NO_WEAPON};
					tempMapEvents->push_back(a);
				}else{
					bool right = true;
					int xPos = -500;
					if(rand()%2==0){
						right = false;
						xPos*=-1;
					}
					pathInfo* testPath = genSweep(right);
					MapEvent pathtest = {1,t+i*35,xPos,-400,4,20,"peace",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest);
				}
			}
			t+=40*35;
			t+=120;
			for(int i =0; i < 40; i +=1){ //cool stream sort of thingy
				if(rand()%2==0){
					MapEvent a = {1,t+i*45,-600,-400,4,20,"love",0,NO_WEAPON};
					tempMapEvents->push_back(a);
				}else{
					pathInfo* testPath = genInvaders(-400,-1*rand()%60);
					MapEvent pathtest = {1,t+i*35,-400,-400,4,20,"hope",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest);
				}
			}
			t+=40*35;
			t+=120;
			for(int i = 0; i < 16; i +=1){
				
				bool right = true;
				int xPos = -500;
				if(rand()%2==0){
					right = false;
					xPos*=-1;
				}
				pathInfo* testPath = genSweep(right);
				if(rand()%2==0){
				MapEvent pathtest = {1,t+i*35,xPos,-400,4,20,"peace",testPath,NO_WEAPON}; tempMapEvents->push_back(pathtest);
				}else{ MapEvent pathtest = {1,t+i*35,xPos,-400,4,20,"hope",testPath,NO_WEAPON}; tempMapEvents->push_back(pathtest);}
				

			}
			t+=35*16;
			t+=120;
			{
				MapEvent a = {1,t,0,0,50,20,"hopeElite",0,NO_WEAPON}; //end when this dies
				tempMapEvents->push_back(a);
			}



			MapEvent arg19 = {2,t+120,0,0,0,0,"",0,NO_WEAPON};//End Level

			tempMapEvents->push_back(arg19);
			loadLevel(tempMapEvents,tempBackgroundVec); //tempBackgroundVec
			break;
			   }
		case 2:{
			
			nextMusic = sapper;
			



			orxObject_SetTextString(LevelName, "1-3 Rest in Peace"); 
			levelNameForHS = "STAGE 1-3";
			vector<string>* tempBackgroundVec = new vector<string>();
			tempBackgroundVec->push_back("filler5");
			tempBackgroundVec->push_back("filler2");
			tempBackgroundVec->push_back("filler3");
			tempBackgroundVec->push_back("filler5");
			tempBackgroundVec->push_back("filler1");
			tempBackgroundVec->push_back("fillerred");
			tempBackgroundVec->push_back("filler5");
			tempBackgroundVec->push_back("filler4");
			tempBackgroundVec->push_back("filler1");
			tempBackgroundVec->push_back("filler2");
			tempBackgroundVec->push_back("filler3");
			tempBackgroundVec->push_back("fillerred");
			tempBackgroundVec->push_back("filler5");
			tempBackgroundVec->push_back("filler1");
			tempBackgroundVec->push_back("filler2");

			vector<MapEvent>* tempMapEvents = new vector<MapEvent>();
			MapEvent arg0 = {0,0,80,0,0,0,"",0,NO_WEAPON}; //Map Movement Speed
			tempMapEvents->push_back(arg0);
			int t = 0;
			
			
			//random side guys
			for(int i =0; i < 40; i +=1){
				MapEvent a = {1,t+i*30,-600,-400,4,20,"love",0,NO_WEAPON};
				tempMapEvents->push_back(a);
				
			}
			t+=40*30;
			t+=120;

			//solid stream of sweepers!
			for(int i = 0; i < 40; i +=1){
				bool right = true;
				int xPos = -500;
				if(i%2==0){
					right = false;
					xPos*=-1;
				}
				pathInfo* testPath = genSweep(right);
				if(i%4>=2){
					MapEvent pathtest = {1,t+i*15,xPos,-400,4,20,"peace",testPath,NO_WEAPON}; 
					tempMapEvents->push_back(pathtest);
				}else{ 
					MapEvent pathtest = {1,t+i*15,xPos,-400,4,20,"hope",testPath,NO_WEAPON}; 
					tempMapEvents->push_back(pathtest);
				}
				
			}
			t+=40*15;
			t+=120;

			//more enemy blocks! all peaces!
			for(int i =0; i < 2; i +=1){
				{
					pathInfo* testPath = genInvaders(-450,0);
					MapEvent pathtest = {1,t,-400,-400,4,20,"peace",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest);
					testPath = genInvaders(-400+10,0);
					MapEvent pathtest1 = {1,t,-400,-400,4,20,"peace",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest1);
					testPath = genInvaders(-350+20,0);
					MapEvent pathtest2 = {1,t,-400,-400,4,20,"peace",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest2);
					testPath = genInvaders(-300+30,0);
					MapEvent pathtest3 = {1,t,-400,-400,4,20,"peace",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest3);
				}
				{
					pathInfo* testPath = genInvaders(-450,60);
					MapEvent pathtest = {1,t,-400,-400,4,20,"peace",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest);
					testPath = genInvaders(-400+10,60);
					MapEvent pathtest1 = {1,t,-400,-400,4,20,"peace",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest1);
					testPath = genInvaders(-350+20,60);
					MapEvent pathtest2 = {1,t,-400,-400,4,20,"peace",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest2);
					testPath = genInvaders(-300+30,60);
					MapEvent pathtest3 = {1,t,-400,-400,4,20,"peace",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest3);
				}
				{
					pathInfo* testPath = genInvaders(-450,-60);
					MapEvent pathtest = {1,t,-400,-400,16,20,"hope",testPath,NO_WEAPON}; //MAKE ELITE
					tempMapEvents->push_back(pathtest);
					testPath = genInvaders(-400+10,-60);
					MapEvent pathtest1 = {1,t,-400,-400,4,20,"peace",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest1);
					testPath = genInvaders(-350+20,-60);
					MapEvent pathtest2 = {1,t,-400,-400,4,20,"peace",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest2);
					testPath = genInvaders(-300+30,-60);
					MapEvent pathtest3 = {1,t,-400,-400,8,20,"hope",testPath,NO_WEAPON};
					tempMapEvents->push_back(pathtest3);
				}
				t+=360;
			}
			t+=60;

			//sweeps - random and loves
			for(int i = 0; i < 40; i +=1){
				if(rand()%2==0){
					bool right = true;
					int xPos = -500;
					if(rand()%2==0){
						right = false;
						xPos*=-1;
					}
					pathInfo* testPath = genSweep(right);
					if(rand()%2==0){
						MapEvent pathtest = {1,t+i*35,xPos,-400,4,20,"peace",testPath,NO_WEAPON}; 
						tempMapEvents->push_back(pathtest);
					}else{ 
						MapEvent pathtest = {1,t+i*35,xPos,-400,4,20,"hope",testPath,NO_WEAPON};
						tempMapEvents->push_back(pathtest);
					}
				}else{
					MapEvent a = {1,t+i*35,-600,-400,4,20,"love",0,NO_WEAPON};
					tempMapEvents->push_back(a);
				}

			}
			t+=35*40;
			t+=120;

			MapEvent arg19 = {2,t+120,0,0,0,0,"",0,NO_WEAPON};//End Level

			tempMapEvents->push_back(arg19);
			loadLevel(tempMapEvents,tempBackgroundVec); //tempBackgroundVec
			break;
			   }

		case 3:{
			
			nextMusic = crimes;
			


			//Bugs 2-2
			orxObject_SetTextString(LevelName, "2-1 What? Bugs?"); 
			levelNameForHS = "STAGE 2-1";
			vector<string>* tempBackgroundVec = new vector<string>();
			tempBackgroundVec->push_back("filler1");
			tempBackgroundVec->push_back("filler4");
			tempBackgroundVec->push_back("filler5");
			tempBackgroundVec->push_back("special1");
			tempBackgroundVec->push_back("filler3");
			tempBackgroundVec->push_back("filler2");
			tempBackgroundVec->push_back("fillergreen");
			tempBackgroundVec->push_back("filler4");
			tempBackgroundVec->push_back("filler3");
			tempBackgroundVec->push_back("filler5");
			tempBackgroundVec->push_back("filler1");
			tempBackgroundVec->push_back("fillergreen");
			tempBackgroundVec->push_back("filler3");
			tempBackgroundVec->push_back("filler4");
			tempBackgroundVec->push_back("filler1");

			vector<MapEvent>* tempMapEvents = new vector<MapEvent>();
			MapEvent arg0 = {0,0,80,0,0,0,"",0,NO_WEAPON}; //Map Movement Speed
			tempMapEvents->push_back(arg0);
			//start with random again, but for longer
			for(int i = 0; i < 2; i +=1){
				pathInfo* testPath = genRand(12,20);
				MapEvent pathtest = {1,i,0,-350,4,20,"stop",testPath,NO_WEAPON};
				tempMapEvents->push_back(pathtest);
			}

			for(int i = 0; i < 20; i +=1){
				pathInfo* testPath = genRand(12,10);
				MapEvent pathtest = {1,10+i*30,0,-350,4,20,"stop",testPath,NO_WEAPON};
				tempMapEvents->push_back(pathtest);
			}
			//1010 - mantis rush guys
			for(int i =0; i < 4; i +=1){
			int hun = 100;
			if(rand()%2==0) hun*=-1;
			pathInfo* testPath = genMantisRush((rand()%3)*hun,20);
			MapEvent pathtest = {1,1100+i,i*hun,-350,4,20,"stop",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
		}

		for(int i =0; i < 8; i +=1){
			int hun = 100;
			if(rand()%2==0) hun*=-1;
			pathInfo* testPath = genMantisRush((rand()%3)*hun,20);
			MapEvent pathtest = {1,1200+i*30,i*hun,-350,4,20,"stop",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
		}

		for(int i =0; i < 16; i +=1){
			int hun = 100;
			if(rand()%2==0) hun*=-1;
			pathInfo* testPath = genMantisRush((rand()%3)*hun,20);
			MapEvent pathtest = {1,1400+i*40,i*hun,-350,4,20,"stop",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
		}

		//2000 lets do that zipper, but slowly
		for(int i = 0; i < 20; i +=1){
			bool snake = false;
			int xPos;
			pathInfo* testPath;
			if(rand()%2==0) snake = true;
			if(snake){
				bool huh = false;
				if(i%2==0) huh = true;
				testPath = genSnake(huh,30);
				xPos = -500;
				if(huh) xPos=500;
			}else{
				bool right = false;
				if(i%2==0) right = true;
				testPath = genSwarm(30,right);
				xPos = 500;
				if(right) xPos = -500;
			}
			MapEvent pathtest = {1,2000+i*60,xPos,-350,4,20,"stag",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
		}

		//3200 - add a top swarm that we replenish throughout the rest
		for(int i =0; i < 20; i +=1){
			MapEvent pathtest = {1,3240+i,0,-400,4,20,"topHoard",0,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
		}

		//3300 - remember - we just added a bunch of guys at the top
		for(int i = 0; i < 10; i +=1){
			bool huh = false;
			pathInfo* testPath = genSnake(huh,30);
			int xpos = -500;
			MapEvent pathtest = {1,3300+i*2,xpos,-350,4,20,"stag",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
		}
		for(int i = 0; i < 10; i +=1){
			pathInfo* testPath = genSwarm(30,true);
			MapEvent pathtest = {1,3320+i,-500,-350,4,20,"stag",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
		}

		for(int i = 0; i < 10; i +=1){
			bool huh = true;
			pathInfo* testPath = genSnake(huh,20);
			int xpos = 500;
			MapEvent pathtest = {1,3500+i*2,xpos,-350,4,20,"stag",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
		}
		for(int i = 0; i < 10; i +=1){
			pathInfo* testPath = genSwarm(20,false);
			MapEvent pathtest = {1,3520+i,500,-350,4,20,"stag",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
		}

		for(int i =0; i < 20; i +=1){
			MapEvent pathtest = {1,3640+i,0,-400,4,20,"topHoard",0,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
		}

		for(int i = 0; i < 10; i +=1){
			pathInfo* testPath = genRand(12,10);
			MapEvent pathtest = {1,3800+i*60,0,-400,4,20,"stop",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
		}

		for(int i =0; i < 10; i +=1){
			MapEvent pathtest = {1,4400+i,0,-400,4,20,"topHoard",0,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
		}

		for(int i =0; i < 10; i +=1){
			MapEvent pathtest = {1,5000+i,0,-400,4,20,"topHoard",0,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
		}

		//let them clear the top hoard 5000 - GIANT SWARM
		for(int i = 0; i < 80; i +=1){
				bool right = false;
				if(i%2==0) right = true;
				pathInfo* testPath = genSwarm(20,right);
				int xPos = 500;
				if(right) xPos = -500;
				MapEvent pathtest = {1,5020+i*2,xPos,-350,4,20,"stag",testPath,NO_WEAPON};
				tempMapEvents->push_back(pathtest);
		}
		MapEvent arg19 = {2,6000,0,0,0,0,"",0,NO_WEAPON};//End Level
		tempMapEvents->push_back(arg19);
		loadLevel(tempMapEvents,tempBackgroundVec);
		break;
		}

		case 4:
		{
			
			nextMusic = air;
			

			//Blank Level
			orxObject_SetTextString(LevelName, "2-2 Ugh... Bugs"); 
			levelNameForHS = "STAGE 2-2";
			vector<string>* tempBackgroundVec = new vector<string>();
			tempBackgroundVec->push_back("filler5");
			tempBackgroundVec->push_back("filler4");
			tempBackgroundVec->push_back("filler2");
			tempBackgroundVec->push_back("filler1");
			tempBackgroundVec->push_back("fillergreen");
			tempBackgroundVec->push_back("filler3");
			tempBackgroundVec->push_back("filler5");
			tempBackgroundVec->push_back("filler2");
			tempBackgroundVec->push_back("special1");
			tempBackgroundVec->push_back("filler3");
			tempBackgroundVec->push_back("filler2");
			tempBackgroundVec->push_back("filler4");
			tempBackgroundVec->push_back("filler1");
			tempBackgroundVec->push_back("fillergreen");
			tempBackgroundVec->push_back("filler1");
			vector<MapEvent>* tempMapEvents = new vector<MapEvent>();
			MapEvent arg0 = {0,0,80,0,0,0,"",0,NO_WEAPON}; //Map Movement Speed
			tempMapEvents->push_back(arg0);

		for(int i = 0; i < 5; i +=1){
				pathInfo* testPath = genRand(12,30);
				MapEvent pathtest = {1,i*30,0,-350,20,20,"stop",testPath,NO_WEAPON};
				tempMapEvents->push_back(pathtest);
		}
		for(int i = 0; i < 12; i +=1){
				pathInfo* testPath = genSwarm(40,false);
				MapEvent pathtest = {1,300+i*6,500,-350,4,20,"stag",testPath,NO_WEAPON};
				tempMapEvents->push_back(pathtest);
		}
		for(int i = 0; i < 12; i +=1){
				pathInfo* testPath = genSwarm(40,true);
				MapEvent pathtest = {1,600+i*6,-500,-350,4,20,"stag",testPath,NO_WEAPON};
				tempMapEvents->push_back(pathtest);
		}
		for(int i = 0; i < 10; i +=1){
				pathInfo* testPath = genSwarm(30,false);
				MapEvent pathtest = {1,900+i*10,500,-350,4,20,"stag",testPath,NO_WEAPON};
				tempMapEvents->push_back(pathtest);
		}
		for(int i = 0; i < 10; i +=1){
				pathInfo* testPath = genSwarm(30,true);
				MapEvent pathtest = {1,1000+i*10,-500,-350,4,20,"stag",testPath,NO_WEAPON};
				tempMapEvents->push_back(pathtest);
		}


		//2/6th
		for(int i = 0; i < 40; i +=1){
				bool right = false;
				if(i%2==0) right = true;
				pathInfo* testPath = genSwarm(20,right);
				int xPos = 500;
				if(right) xPos = -500;
				MapEvent pathtest = {1,1300+i*25,xPos,-350,4,20,"stag",testPath,NO_WEAPON};
				tempMapEvents->push_back(pathtest);
		}
		for(int i = 0; i < 10; i +=1){
				bool huh = false;
				if(rand()%2==0) huh = true;
				pathInfo* testPath = genSnake(huh,40);
				int xpos = -500;
				if(huh) xpos*=-1;
				MapEvent pathtest = {1,2000+i*30,xpos,-350,4,20,"stag",testPath,NO_WEAPON};
				tempMapEvents->push_back(pathtest);
		}
		for(int i = 0; i < 15; i +=1){
				bool huh = false;
				if(rand()%2==0) huh = true;
				pathInfo* testPath = genSnake(huh,30);
				int xpos = -500;
				if(huh) xpos*=-1;
				MapEvent pathtest = {1,2300+i*20,xpos,-350,4,20,"stag",testPath,NO_WEAPON};
				tempMapEvents->push_back(pathtest);
		}
		for(int i = 0; i < 20; i +=1){
				bool huh = false;
				if(rand()%2==0) huh = true;
				pathInfo* testPath = genSnake(huh,20);
				int xpos = -500;
				if(huh) xpos*=-1;
				MapEvent pathtest = {1,2600+i*10,xpos,-350,4,20,"stag",testPath,NO_WEAPON};
				tempMapEvents->push_back(pathtest);
		}

		//2800 - hammer with both for a bit?
		for(int i = 0; i < 10; i +=1){
			bool huh = false;
			pathInfo* testPath = genSnake(huh,30);
			int xpos = -500;
			MapEvent pathtest = {1,2900+i*2,xpos,-350,4,20,"stag",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
		}
		for(int i = 0; i < 10; i +=1){
			pathInfo* testPath = genSwarm(30,true);
			MapEvent pathtest = {1,2920+i,-500,-350,4,20,"stag",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
		}

		for(int i = 0; i < 10; i +=1){
			bool huh = true;
			pathInfo* testPath = genSnake(huh,20);
			int xpos = 500;
			MapEvent pathtest = {1,3000+i*2,xpos,-350,4,20,"stag",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
		}
		for(int i = 0; i < 10; i +=1){
			pathInfo* testPath = genSwarm(20,false);
			MapEvent pathtest = {1,3020+i,500,-350,4,20,"stag",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
		}

		//3030 HALFWAY! ziper both for 1000? every 60 ticks
		for(int i = 0; i < 20; i +=1){
			bool snake = false;
			int xPos;
			pathInfo* testPath;
			if(rand()%2==0) snake = true;
			if(snake){
				bool huh = false;
				if(i%2==0) huh = true;
				testPath = genSnake(huh,20);
				xPos = -500;
				if(huh) xPos=500;
			}else{
				bool right = false;
				if(i%2==0) right = true;
				testPath = genSwarm(20,right);
				xPos = 500;
				if(right) xPos = -500;
			}
			MapEvent pathtest = {1,3100+i*60,xPos,-350,4,20,"stag",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
		}

		//4300 - need a break from swarm, getting boring. rush
		for(int i =0; i < 4; i +=1){
			int hun = 100;
			if(rand()%2==0) hun*=-1;
			pathInfo* testPath = genMantisRush((rand()%3)*hun,20);
			MapEvent pathtest = {1,4300+i,i*hun,-350,4,20,"stop",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
		}

		for(int i =0; i < 8; i +=1){
			int hun = 100;
			if(rand()%2==0) hun*=-1;
			pathInfo* testPath = genMantisRush((rand()%3)*hun,20);
			MapEvent pathtest = {1,4400+i*30,i*hun,-350,4,20,"stop",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
		}

		for(int i =0; i < 16; i +=1){
			int hun = 100;
			if(rand()%2==0) hun*=-1;
			pathInfo* testPath = genMantisRush((rand()%3)*hun,20);
			MapEvent pathtest = {1,4500+i*40,i*hun,-350,4,20,"stop",testPath,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
		}

		//4500+640 = 5140. Introduce the swarm for the next 1000 ticks
		for(int i =0; i < 10; i +=1){
			MapEvent pathtest = {1,5140+i,0,-400,4,20,"topHoard",0,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
		}
		for(int i =0; i < 10; i +=1){
			MapEvent pathtest = {1,5300+i,0,-400,4,20,"topHoard",0,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
		}
		for(int i =0; i < 30; i +=1){
			MapEvent pathtest = {1,5800+i,0,-400,4,20,"topHoard",0,NO_WEAPON};
			tempMapEvents->push_back(pathtest);
		}
		
		MapEvent arg19 = {2,6500,0,0,0,0,"",0,NO_WEAPON};//End Level
		tempMapEvents->push_back(arg19);

		loadLevel(tempMapEvents, tempBackgroundVec);
		break;
		}

		case 5:{ 
			nextMusic = ave;
			

			int t = 0; //keep track of what time we are at

			orxObject_SetTextString(LevelName, "2-3 I Don't Like Them"); 
			levelNameForHS = "STAGE 2-3";
			vector<string>* tempBackgroundVec = new vector<string>();
			tempBackgroundVec->push_back("filler2");
			tempBackgroundVec->push_back("fillergreen");
			tempBackgroundVec->push_back("filler4");
			tempBackgroundVec->push_back("filler1");
			tempBackgroundVec->push_back("filler5");
			tempBackgroundVec->push_back("special1");
			tempBackgroundVec->push_back("filler3");
			tempBackgroundVec->push_back("filler5");
			tempBackgroundVec->push_back("filler2");
			tempBackgroundVec->push_back("fillergreen");
			tempBackgroundVec->push_back("filler3");
			tempBackgroundVec->push_back("filler1");
			tempBackgroundVec->push_back("filler4");
			tempBackgroundVec->push_back("fillergreen");
			tempBackgroundVec->push_back("filler1");
			vector<MapEvent>* tempMapEvents = new vector<MapEvent>();
			MapEvent arg0 = {0,0,80,0,0,0,"",0,NO_WEAPON}; //Map Movement Speed
			tempMapEvents->push_back(arg0);

			
			for(int i =0; i < 40; i +=1){
				MapEvent pathtest = {1,t+i,0,-400,4,20,"topHoard",0,NO_WEAPON};
				tempMapEvents->push_back(pathtest);
			}
			t+=120; //40
			for(int i =0; i < 16; i +=1){
				int hun = 100;
				if(rand()%2==0) hun*=-1;
				pathInfo* testPath = genMantisRush((rand()%3)*hun,20);
				MapEvent pathtest = {1,t+i*60,i*hun,-350,4,20,"stop",testPath,NO_WEAPON};
				tempMapEvents->push_back(pathtest);
			}
			t+=60*16; //1080
			//t+=480; //give them 4 secs to fight hoard
			for(int i = 0; i < 80; i +=1){
				bool right = false;
				if(i%2==0) right = true;
				pathInfo* testPath = genSwarm(20,right);
				int xPos = 500;
				if(right) xPos = -500;
				MapEvent pathtest = {1,t+i*20,xPos,-350,4,20,"stag",testPath,NO_WEAPON};
				tempMapEvents->push_back(pathtest);
			}
			t+=80*20;
			//t+=480; //time to fight hoard
			for(int i = 0; i < 10; i +=1){
				pathInfo* testPath = genRand(12,10);
				MapEvent pathtest = {1,t+i*60,0,-400,4,20,"stop",testPath,NO_WEAPON};
				tempMapEvents->push_back(pathtest);
			}
			t+=10*60; //2560
			//t+=480; //fight the hoard
			for(int i =0; i < 40; i +=1){ //refresh hoard
				MapEvent pathtest = {1,t+i,0,-400,4,20,"topHoard",0,NO_WEAPON};
				tempMapEvents->push_back(pathtest);
			}
			t+=40;
			t+=840; //gotta fight that damn hoard, 14 seconds
			//3920
			for(int i =0; i < 40; i +=1){ //refresh hoard
				MapEvent pathtest = {1,t+i,0,-400,4,20,"topHoard",0,NO_WEAPON};
				tempMapEvents->push_back(pathtest);
			}
			t+=40;
			//all at once? all at once...

			for(int i = 0; i < 20; i +=1){
				bool right = false;
				if(i%2==0) right = true;
				pathInfo* testPath = genSwarm(20,right);
				int xPos = 500;
				if(right) xPos = -500;
				MapEvent pathtest = {1,t+i*5,xPos,-350,4,20,"stag",testPath,NO_WEAPON};
				tempMapEvents->push_back(pathtest);
			}
			t+=20*5;
			for(int i = 0; i < 10; i +=1){
				pathInfo* testPath = genRand(12,10);
				MapEvent pathtest = {1,t+i*5,0,-400,4,20,"stop",testPath,NO_WEAPON};
				tempMapEvents->push_back(pathtest);
			}
			t+=5*10;
			for(int i =0; i < 10; i +=1){
				int hun = 100;
				if(rand()%2==0) hun*=-1;
				pathInfo* testPath = genMantisRush((rand()%3)*hun,20);
				MapEvent pathtest = {1,t+i*5,i*hun,-350,4,20,"stop",testPath,NO_WEAPON};
				tempMapEvents->push_back(pathtest);
			}
			t+=5*10; //4160
			//it ain't over - kill the final hoard!
			for(int i =0; i < 40; i +=1){ //refresh hoard
				MapEvent pathtest = {1,t+i,0,-400,4,20,"topHoard",0,NO_WEAPON};
				tempMapEvents->push_back(pathtest);
			}
			t+=40;
			for(int i =0; i < 1; i +=1){ //refresh hoard
				MapEvent pathtest = {1,t+i,0,-400,20,20,"topHoard",0,NO_WEAPON};
				tempMapEvents->push_back(pathtest);
			}
			t+=480;
			for(int i =0; i < 40; i +=1){ //refresh hoard
				MapEvent pathtest = {1,t+i,0,-400,4,20,"topHoard",0,NO_WEAPON};
				tempMapEvents->push_back(pathtest);
			}
			t+=40;
			//t+=480;
			for(int i = 0; i < 40; i +=1){
				bool huh = false;
				if(i%2==0) huh = true;
				pathInfo* testPath = genSnake(huh,20);
				int xpos = -500;
				if(huh) xpos*=-1;
				MapEvent pathtest = {1,t+i*30,xpos,-350,4,20,"stag",testPath,NO_WEAPON};
				tempMapEvents->push_back(pathtest);
			}
			t+=30*40;
			t+=120;
			

			MapEvent arg19 = {2,t+120,0,0,0,0,"",0,NO_WEAPON};//End Level
			tempMapEvents->push_back(arg19);
			loadLevel(tempMapEvents,tempBackgroundVec);
			break;
			}

			case 6:
				{
					
					nextMusic = quick;
					

					orxObject_SetTextString(LevelName, "3-1 Bullets > Bugs?"); 
					levelNameForHS = "STAGE 3-1";
					vector<string>* tempBackgroundVec = new vector<string>();
					tempBackgroundVec->push_back("filler3");
					tempBackgroundVec->push_back("special3");
					tempBackgroundVec->push_back("filler5");
					tempBackgroundVec->push_back("filler3");
					tempBackgroundVec->push_back("filler2");
					tempBackgroundVec->push_back("filler1");
					tempBackgroundVec->push_back("filler4");
					tempBackgroundVec->push_back("special2");
					tempBackgroundVec->push_back("filler3");
					tempBackgroundVec->push_back("filler5");
					tempBackgroundVec->push_back("filler2");
					tempBackgroundVec->push_back("filler1");
					tempBackgroundVec->push_back("special2");
					tempBackgroundVec->push_back("filler4");
					tempBackgroundVec->push_back("filler5");
					vector<MapEvent>* tempMapEvents = new vector<MapEvent>();
					MapEvent arg0 = {0,0,80,0,0,0,"",0,NO_WEAPON}; //Map Movement Speed
					tempMapEvents->push_back(arg0);

					int t = 0;
					t+=60;
					//4 waves of down
					for(int i = 0; i < 3; i+=1){
						int j = i * 200;
						MapEvent pathtest = {1,t+i,j,-400,4,20,"down",0,NO_WEAPON};
						tempMapEvents->push_back(pathtest);
					}
					t+=60;
					for(int i = 0; i < 6; i+=1){
						int j = -1*i * 100;
						MapEvent pathtest = {1,t+i,j,-400,4,20,"down",0,NO_WEAPON};
						tempMapEvents->push_back(pathtest);
					}
					t+=120;
					for(int i = 0; i < 3; i+=1){
						int j = -1*i * 175;
						MapEvent pathtest = {1,t+i,j,-400,4,20,"down",0,NO_WEAPON};
						tempMapEvents->push_back(pathtest);
					}
					t+=60;
					for(int i = 0; i < 6; i+=1){
						int j = i * 75;
						MapEvent pathtest = {1,t+i,j,-400,4,20,"down",0,NO_WEAPON};
						tempMapEvents->push_back(pathtest);
					}
					t+=120;

					//7 seconds in, wiggles
					for(int i = 0; i < 1; i +=1){
						int j = 0;
						MapEvent pathtest = {1,t+i,j,-400,4,20,"b2",0,NO_WEAPON};
						tempMapEvents->push_back(pathtest);
					} 
					t+= 60;
					for(int i = 0; i < 3; i +=1){
						int j = i*175;
						MapEvent pathtest = {1,t+i,j,-400,4,20,"b2",0,NO_WEAPON};
						tempMapEvents->push_back(pathtest);
					} 
					t+=120;
					for(int i = 0; i < 3; i +=1){
						int j = -1*i*175;
						MapEvent pathtest = {1,t+i,j,-400,4,20,"b2",0,NO_WEAPON};
						tempMapEvents->push_back(pathtest);
					} 
					t+=120;

					//12 seconds, shoot both at once
					for(int i = 0; i < 1; i +=1){
						int j = 0;
						MapEvent pathtest = {1,t,j,-400,4,20,"down",0,NO_WEAPON};
						tempMapEvents->push_back(pathtest);
					} 
					t+=30;
					for(int i = 0; i < 1; i +=1){
						int j = 0;
						MapEvent pathtest = {1,t+i,j,-400,4,20,"b2",0,NO_WEAPON};
						tempMapEvents->push_back(pathtest);
					} 
					t+=90;

					//14 seconds - send two
					for(int i = 0; i < 2; i +=1){
						int j = 0;
						if(i==0) j = -200;
						if(i==1) j = 200;
						MapEvent pathtest = {1,t,j,-400,4,20,"down",0,NO_WEAPON};
						tempMapEvents->push_back(pathtest);
					} 
					t+=30;
					for(int i = 0; i < 2; i +=1){
						int j = 0;
						if(i==0) j = -200;
						if(i==1) j = 200;
						MapEvent pathtest = {1,t+i,j,-400,4,20,"b2",0,NO_WEAPON};
						tempMapEvents->push_back(pathtest);
					} 
					t+=90;

					//16 seconds, wait a bit, then side ships
					for(int i = 0; i < 2; i +=1){
						int j = 0;
						if(i==0) j = -600;
						if(i==1) j = 600;
						MapEvent pathtest = {1,t,j,-400,4,20,"side",0,NO_WEAPON};
						tempMapEvents->push_back(pathtest);
					} 
					t+=60;

					//17 seconds - send more side ships
					for(int i = 0; i < 20; i +=1){
						int j = 600;
						if(rand()%2==0) j*=-1;
						int y = -1*(rand()%500+250);
						MapEvent pathtest = {1,t+i*30,j,y,4,20,"side",0,NO_WEAPON};
						tempMapEvents->push_back(pathtest);
					} 
					t+=20*30;
					t+=60;//wait a sec

					//~28 seconds - downs across the screen, duck shape V
					for(int i =0; i < 10; i+=1){
						int j = i*50;
						if(i%2==0){ 
							j *=-1;
						}
						MapEvent pathtest = {1,t+i*60,j,-400,4,20,"down",0,NO_WEAPON};
						tempMapEvents->push_back(pathtest);
					}
					t+=60*10;
					t+=60; //wait a sec

					//39 seconds
					for(int i =0; i < 10; i+=1){
						int j = 500-i*50;
						if(i%2==0){ 
							j *=-1;
						}
						MapEvent pathtest = {1,t+i*60,j,-400,4,20,"down",0,NO_WEAPON};
						tempMapEvents->push_back(pathtest);
					}
					t+=60*10;
					t+=60;

					//50 seconds - sides and wiggles, random?
					for(int i = 0; i < 20; i +=1){
						if(rand()%2==0){
							int j = rand()%500;
							if(rand()%2==0) j*=-1;
							MapEvent pathtest = {1,t+i*30,j,-400,4,20,"b2",0,NO_WEAPON};
							tempMapEvents->push_back(pathtest);
						}else{
							int j = 600;
							if(rand()%2==0) j*= -1;
							int y = -1*(rand()%500+250);
							MapEvent pathtest = {1,t+i*30,j,y,4,20,"side",0,NO_WEAPON};
							tempMapEvents->push_back(pathtest);
						}
					} 
					t+=20*30;
					t+=60; //wait a sec

					//70 seconds - sides and down
					for(int i = 0; i < 20; i +=1){
						if(rand()%2==0){
							int j = rand()%500;
							if(rand()%2==0) j*=-1;
							MapEvent pathtest = {1,t+i*30,j,-400,4,20,"down",0,NO_WEAPON};
							tempMapEvents->push_back(pathtest);
						}else{
							int j = 600;
							if(rand()%2==0) j*=-1;
							int y = -1*(rand()%500+250);
							MapEvent pathtest = {1,t+i*30,j,y,4,20,"side",0,NO_WEAPON};
							tempMapEvents->push_back(pathtest);
						}
					} 
					t+=20*30;
					t+=60; //wait a sec

					//81 seconds - all three!
					for(int i = 0; i < 30; i +=1){
						int r = rand()%3;
						if(r==0){
							int j = rand()%500;
							if(rand()%2==0) j*=-1;
							MapEvent pathtest = {1,t+i*30,j,-400,4,20,"down",0,NO_WEAPON};
							tempMapEvents->push_back(pathtest);
						}else if(r==1){
							int j = 600;
							if(rand()%2==0) j*=-1;
							int y = -1*(rand()%500+250);
							MapEvent pathtest = {1,t+i*30,j,y,4,20,"side",0,NO_WEAPON};
							tempMapEvents->push_back(pathtest);
						}else{
							int j = rand()%500;
							if(rand()%2==0) j*=-1;
							MapEvent pathtest = {1,t+i*30,j,-400,4,20,"b2",0,NO_WEAPON};
							tempMapEvents->push_back(pathtest);
						}
					} 
					t+=30*30;

					//96 seconds






					MapEvent arg19 = {2,t+120,0,0,0,0,"",0,NO_WEAPON};//End Level
					tempMapEvents->push_back(arg19);
					loadLevel(tempMapEvents,tempBackgroundVec);
					break;
				}

				case 7:{
					
					nextMusic = scan;
					
					orxObject_SetTextString(LevelName, "3-2 Warp Speed!"); 
					levelNameForHS = "STAGE 3-2";
					vector<string>* tempBackgroundVec = new vector<string>();
					tempBackgroundVec->push_back("filler3");
					tempBackgroundVec->push_back("filler2");
					tempBackgroundVec->push_back("filler5");
					tempBackgroundVec->push_back("filler3");
					tempBackgroundVec->push_back("filler2");
					tempBackgroundVec->push_back("filler1");
					tempBackgroundVec->push_back("filler4");
					tempBackgroundVec->push_back("filler5");
					tempBackgroundVec->push_back("filler3");
					tempBackgroundVec->push_back("filler5");
					tempBackgroundVec->push_back("filler2");
					tempBackgroundVec->push_back("filler1");
					tempBackgroundVec->push_back("filler3");
					tempBackgroundVec->push_back("filler4");
					tempBackgroundVec->push_back("filler5");
					vector<MapEvent>* tempMapEvents = new vector<MapEvent>();
					MapEvent arg0 = {0,0,500,0,0,0,"",0,NO_WEAPON}; //Map Movement Speed
					tempMapEvents->push_back(arg0);

					int t = 0;
					t+=60;
					//1 second start with 2 streams
					for(int i =0; i < 2; i+=1){
						int j = 400;
						if(i%2==0) j*=-1;
						MapEvent pathtest = {1,t+i,j,-400,4,20,"stream",0,NO_WEAPON};
						tempMapEvents->push_back(pathtest);
					}
					t+=180; //give them 3 seconds to kill the streams
					for(int i =0; i < 3; i+=1){
						int j = 400;
						if(i%2==0) j*=-1;
						if(i==2) j = 0;
						MapEvent pathtest = {1,t+i*60,j,-400,4,20,"stream",0,NO_WEAPON};
						tempMapEvents->push_back(pathtest);
					}
					t+=60*3;
					t+=180; //give them 3 seconds to kill the streams
					//9 seconds in - spawn twirlys for a while
					for(int i =0; i < 6; i+=1){
						int j = rand()%450;
						if(rand()%2==0) j*=-1;
						MapEvent pathtest = {1,t+i*120,j,-400,4,20,"stream",0,NO_WEAPON};
						tempMapEvents->push_back(pathtest);
					}
					t+=120*6;
					t+=120;

					//35 seconds in - spawn one with loads of health
					for(int i =0; i < 1; i+=1){
						int j = 0;
						MapEvent pathtest = {1,t+i*120,j,-400,100,20,"stream",0,NO_WEAPON}; //10X as much health
						tempMapEvents->push_back(pathtest);
					}
					t+=60;
					for(int i =0; i < 10; i+=1){
						int j = i*50;
						if(i%2==0){ 
							j *=-1;
						}
						MapEvent pathtest = {1,t+i*60,j,-400,4,20,"down",0,NO_WEAPON};
						tempMapEvents->push_back(pathtest);
					}
					t+=60*10;
					for(int i = 0; i < 10; i +=1){
							int j = rand()%500;
							if(rand()%2==0) j*=-1;
							MapEvent pathtest = {1,t+i*60,j,-400,4,20,"b2",0,NO_WEAPON};
							tempMapEvents->push_back(pathtest);
					}
					t+=10*60;
					t+=60;
					for(int i = 0; i < 10; i +=1){
						int j = 600;
						if(rand()%2==0) j*=-1;
						int y = -1*(rand()%500+250);
						MapEvent pathtest = {1,t+i*60,j,y,4,20,"side",0,NO_WEAPON};
						tempMapEvents->push_back(pathtest);
					} 
					t+=10*60;
					t+=60;
					//spawn 2 high helath guys
					for(int i =0; i < 2; i+=1){
						int j = 400;
						if(i==1) j*=-1;
						MapEvent pathtest = {1,t+i*120,j,-400,100,20,"stream",0,NO_WEAPON}; //10X as much health
						tempMapEvents->push_back(pathtest);
					}
					t+=120;
					for(int i = 0; i < 30; i +=1){
						int r = rand()%3;
						if(r==0){
							int j = rand()%500;
							if(rand()%2==0) j*=-1;
							MapEvent pathtest = {1,t+i*30,j,-400,4,20,"down",0,NO_WEAPON};
							tempMapEvents->push_back(pathtest);
						}else if(r==1){
							int j = 600;
							if(rand()%2==0) j*=-1;
							int y = -1*(rand()%500+250);
							MapEvent pathtest = {1,t+i*30,j,y,4,20,"side",0,NO_WEAPON};
							tempMapEvents->push_back(pathtest);
						}else{
							int j = rand()%500;
							if(rand()%2==0) j*=-1;
							MapEvent pathtest = {1,t+i*30,j,-400,4,20,"b2",0,NO_WEAPON};
							tempMapEvents->push_back(pathtest);
						}
					} 
					t+=30*30;
					//cout<<"Made it"<<endl;

					MapEvent arg19 = {2,t+120,0,0,0,0,"",0,NO_WEAPON};//End Level
					tempMapEvents->push_back(arg19);
					loadLevel(tempMapEvents,tempBackgroundVec);
					break;
					}
				case 8: { //only the last level can have no background
					
					nextMusic = faena; //spazz for lulz
					
					

					orxObject_SetTextString(LevelName, "3-3 The End"); 
					levelNameForHS = "STAGE 3-3";
					vector<string>* tempBackgroundVec = new vector<string>();
					tempBackgroundVec->push_back("background1");
					tempBackgroundVec->push_back("background2");
					tempBackgroundVec->push_back("background3");
					tempBackgroundVec->push_back("background4");
					tempBackgroundVec->push_back("background5");
					tempBackgroundVec->push_back("background6");
					tempBackgroundVec->push_back("background7");
					tempBackgroundVec->push_back("background8");
					tempBackgroundVec->push_back("background9");
					tempBackgroundVec->push_back("background10");
					vector<MapEvent>* tempMapEvents = new vector<MapEvent>();
					MapEvent arg0 = {0,0,50,0,0,0,"",0,NO_WEAPON}; //Map Movement Speed
					tempMapEvents->push_back(arg0);
					int t = 0;
					t+=60;
					//blend levels 2 and 3?
					//make twirlys!

					for(int i =0; i < 2; i+=1){
						int j = 400;
						if(i%2==0) j*=-1;
							MapEvent pathtest = {1,t+i,j,-400,4,20,"stream",0,NO_WEAPON};
							tempMapEvents->push_back(pathtest);
						}
						t+=120; //2seconds to kill //180
						//send in a little swarm
						for(int i = 0; i < 10; i +=1){
							pathInfo* testPath = genSwarm(30,true);
							MapEvent pathtest = {1,t+i,-500,-350,4,20,"stag",testPath,NO_WEAPON};
							tempMapEvents->push_back(pathtest);
						}
						t+=120; //300
						for(int i = 0; i < 10; i +=1){
							pathInfo* testPath = genSwarm(30,false);
							MapEvent pathtest = {1,t+i,500,-350,4,20,"stag",testPath,NO_WEAPON};
							tempMapEvents->push_back(pathtest);
						}
						t+=240; //420

						//wiggle and down pair 
						for(int i = 0; i < 2; i +=1){
							int j = 0;
							if(i==0) j = -200;
							if(i==1) j = 200;
							MapEvent pathtest = {1,t,j,-400,4,20,"down",0,NO_WEAPON};
							tempMapEvents->push_back(pathtest);
						} 
						t+=30; //450
						for(int i = 0; i < 2; i +=1){
							int j = 0;
							if(i==0) j = -200;
							if(i==1) j = 200;
							MapEvent pathtest = {1,t+i,j,-400,4,20,"b2",0,NO_WEAPON};
							tempMapEvents->push_back(pathtest);
						} 
						t+=120; //540

						//side ships
						for(int i = 0; i < 2; i +=1){
							int j = 0;
							if(i==0) j = -600;
							if(i==1) j = 600;
							MapEvent pathtest = {1,t,j,-400,4,20,"side",0,NO_WEAPON};
							tempMapEvents->push_back(pathtest);
						} 
						t+=60; //600

						//snake stags
						for(int i = 0; i < 20; i +=1){
							bool huh = false;
							if(rand()%2==0) huh = true;
							pathInfo* testPath = genSnake(huh,30);
							int xpos = -500;
							if(huh) xpos*=-1;
							MapEvent pathtest = {1,t+i*30,xpos,-350,4,20,"stag",testPath,NO_WEAPON};
							tempMapEvents->push_back(pathtest);
						}
						t+=30*20; //1200
						t+=240;
						//twirls AND swarm!
						for(int i =0; i < 60; i +=1){
							MapEvent pathtest = {1,t+i,0,-400,4,20,"topHoard",0,NO_WEAPON};
							tempMapEvents->push_back(pathtest);
						}
						t+=30; //1230
						for(int i =0; i < 6; i+=1){
							int j = rand()%450;
							if(rand()%2==0) j*=-1;
							MapEvent pathtest = {1,t+i*120,j,-400,8,20,"stream",0,NO_WEAPON};
							tempMapEvents->push_back(pathtest);
						}
						t+=120*6; //1950
						t+=180; //2070
						//need more wiggles, downs, and sides
						//start some swarms first
						for(int i = 0; i < 30; i +=1){
							bool right = false;
							if(i%2==0) right = true;
							pathInfo* testPath = genSwarm(20,right);
							int xPos = 500;
							if(right) xPos = -500;
							MapEvent pathtest = {1,t+i*25,xPos,-350,4,20,"stag",testPath,NO_WEAPON};
							tempMapEvents->push_back(pathtest);
						}
						t+=30*25;
						t+=120;
						for(int i =0; i < 60; i +=1){
							MapEvent pathtest = {1,t+i,0,-400,4,20,"topHoard",0,NO_WEAPON};
							tempMapEvents->push_back(pathtest);
						}
						t+=30; 
						for(int i = 0; i < 30; i +=1){
							int r = rand()%4;
							if(r==0){
								int j = rand()%500;
								if(rand()%2==0) j*=-1;
								MapEvent pathtest = {1,t+i*30,j,-400,4,20,"down",0,NO_WEAPON};
								tempMapEvents->push_back(pathtest);
							}else if(r==1){ //these suck too much for this section
								int j = 600;
								if(rand()%2==0) j*=-1;
								int y = -1*(rand()%500+250);
								//MapEvent pathtest = {1,t+i*30,j,y,4,20,"side",0,NO_WEAPON};
								//tempMapEvents->push_back(pathtest);
							}else if(r==3){
								int j = rand()%500;
								if(rand()%2==0) j*=-1;
								MapEvent pathtest = {1,t+i*30,j,-400,4,20,"b2",0,NO_WEAPON};
								tempMapEvents->push_back(pathtest);
							}else{
								int j = rand()%450;
								if(rand()%2==0) j*=-1;
								MapEvent pathtest = {1,t+i*30,j,-400,4,20,"stream",0,NO_WEAPON};
								tempMapEvents->push_back(pathtest);
							}
						} 
						t+=30*30;
						t+=120;
						for(int i = 0; i < 20; i +=1){
							int j = rand()%500;
							if(rand()%2==0) j*=-1;
							MapEvent pathtest = {1,t+i*30,j,-400,4,20,"b2",0,NO_WEAPON};
							tempMapEvents->push_back(pathtest);
						}
						t+=20*30;

						for(int i = 0; i < 120; i +=1){
							int r = rand()%4;
							if(r==0){
								int j = rand()%500;
								if(rand()%2==0) j*=-1;
								MapEvent pathtest = {1,t+i*30,j,-400,4,20,"down",0,NO_WEAPON};
								tempMapEvents->push_back(pathtest);
							}else if(r==1){ //these suck too much for this section
								int j = 600;
								if(rand()%2==0) j*=-1;
								int y = -1*(rand()%500+250);
								MapEvent pathtest = {1,t+i*30,j,y,4,20,"side",0,NO_WEAPON};
								tempMapEvents->push_back(pathtest);
							}else if(r==3){
								int j = rand()%500;
								if(rand()%2==0) j*=-1;
								MapEvent pathtest = {1,t+i*30,j,-400,4,20,"b2",0,NO_WEAPON};
								tempMapEvents->push_back(pathtest);
							}else{
								int j = rand()%450;
								if(rand()%2==0) j*=-1;
								MapEvent pathtest = {1,t+i*30,j,-400,4,20,"stream",0,NO_WEAPON};
								tempMapEvents->push_back(pathtest);
							}
						}

						t+=120*30;
						MapEvent arg19 = {2,t+120,0,0,0,0,"",0,NO_WEAPON};//End Level //don't end level
						tempMapEvents->push_back(arg19);
						loadLevel(tempMapEvents,tempBackgroundVec);
						break;
						}

						case 9:{
							vector<MapEvent>* tempMapEvents = new vector<MapEvent>();
							loadLevel(tempMapEvents,new vector<string>);
							m_Instance->completeGame();
							
							break;
						}
	}
}
pathInfo* LevelSystem::genSweep(bool right){
	double* p = (double*) malloc(sizeof(double)*4*2);
	p[0] = 600;
	p[1] = -400;

	p[2] = 500;
	p[3] = -300;

	p[4] = -2000;
	p[5] = -200;

	p[6] = 1400;
	p[7] = 400;

	if(right){
		for(int i = 0; i < 4; i+=1){
			p[i*2] *=-1;
		}
	}

	return genBezier(p,4,24,10);
}
pathInfo* LevelSystem::genInvaders(int xPos, int yOffset){
	double* p = (double*) malloc(sizeof(double)*10*2);
	p[0] = xPos;
	p[1] = -400+yOffset;

	p[2] = xPos;
	p[3] = -200+yOffset;

	p[4] = xPos+375;
	p[5] = -200+yOffset; 

	p[6] = xPos+750; //goto the right side of the screen
	p[7] = -200+yOffset;

	p[8] = xPos+750;
	p[9] = 0+yOffset;

	p[10] = xPos+375;
	p[11] = 0+yOffset;

	p[12] = xPos;
	p[13] = 0+yOffset;

	p[14] = xPos;
	p[15] = 200+yOffset;

	p[16] = xPos+750;
	p[17] = 200+yOffset;

	p[18] = xPos+750;
	p[19] = 450+yOffset;

	return genPathConstTime(p,10,90);

}

pathInfo* LevelSystem::genRand(int numPoints, int constTime){ //always set numPoints to 12 or less - goes to 0,0 at 20
	double* p = (double*) malloc(sizeof(double)*numPoints*2);
	for(int i =0; i < numPoints; i +=1){
		p[i*2] = 500 - rand()%1000;
		p[i*2+1] = 350 - rand()%700;
		p[i*2]*=2;
		p[i*2+1]*=2;
	}
	p[numPoints*2-1] = 1000;

	return genBezier(p,numPoints,numPoints*4,constTime);
}

pathInfo* LevelSystem::genSnake(bool left, int constTime){

	int mul = 1;
	double* p = (double*) malloc(sizeof(double)*8*2);
	p[0] = -500;
	p[1] = -400;

	p[2] = 500*2;
	p[3] = 0;
	p[4] = 500;
	p[5] = 0;
	p[6] = 500*2;
	p[7] = 0;

	p[8] = -500*2;
	p[9] = 450;
	p[10] = -500;
	p[11] = 450;
	p[12] = -500*2;
	p[13] = 450;

	p[14] = 500*6;
	p[15] = 450;

	if(left){
		for(int i = 0; i < 8; i+=1){
			p[i*2] *=-1;
		}
	}

	addJitter(p,8,150);

	return genBezier(p,8,25,constTime);

}
pathInfo* LevelSystem::genMantisRush(int x, int constTime){ //doesn't go down far enough?
	double* p = (double*) malloc(sizeof(double)*6*2);
	double jit = randFloat(0,350);
	if(rand()%2==0) jit*=-1;
	p[0] = x+jit;
	p[1] = -400;

	p[2] = x-jit;
	p[3] = 100;

	p[4] = x+jit;
	p[5] = -400;

	p[6] = x-jit;
	p[7] = 350;

	p[8] = x+jit;
	p[9] = -350;

	p[10] = (x-jit)*3;
	p[11] = 1200;

	return genPathConstTime(p,6,constTime);
}

pathInfo* LevelSystem::genSwarm(int constTime, bool goingRight){
	double* p = (double*) malloc(sizeof(double)*7*2);
	//int yInterval = abs(yEnd-yStart)/2; //use 400 for yEnd for testing
	p[0] = 500;//yStart;
	p[1] = -350;

	p[2] = 500*2;//yStart+yInterval; //200
	p[3] = 0;

	p[4] = -500*2;//yStart+yEnd; //400
	p[5] = 350*2;

	p[6] = -500*2;//yStart+yInterval;
	p[7] = 0;

	p[8] = 500*2;//yStart;
	p[9] = -350*2;

	p[10] = 500*2;
	p[11] = 0;

	p[12] = -500*3;
	p[13] = 1100;

	if(goingRight){
		for(int i = 0; i < 7; i+=1){
			p[i*2] *=-1;
		}
	}

	addJitter(p,7,300);

	return genBezier(p,7,25, constTime);
}

//give this your array, the number points in your input array, the number of points you want on the curve, and the amount of time between each point.
pathInfo* LevelSystem::genBezier(double* p, int numPoints, int numReturnPoints, int constTime){
	double* beez = (double*) malloc(sizeof(double)*2*numReturnPoints);
	Bezier2D(p,numPoints,numReturnPoints,beez);
	free(p);
	return genPathConstTime(beez,numReturnPoints,constTime);
}

pathInfo* LevelSystem::genBezier(double* p, int numPoints, int numReturnPoints){
	double* beez = (double*) malloc(sizeof(double)*2*numReturnPoints);
	//cout<<"trying Bezier2D"<<endl;
	Bezier2D(p,numPoints,numReturnPoints,beez);
	free(p);
	//cout<<"trying generatePath"<<endl;
	//run through and check beez
	/*for(int i = 0; i < numReturnPoints*2-1; i+=2){
	//cout<<i/2<<": "<<"x "<<beez[i]<<" y "<<beez[i+1]<<endl;
	}*/
	return genPathConstTime(beez,numReturnPoints,10);
}

pathInfo* LevelSystem::genPathConstTime(double* p,int numPoints,int t){
	orxVECTOR* path = (orxVECTOR*) malloc(sizeof(orxVECTOR)*numPoints);
	int* time = (int*) malloc(sizeof(int)*numPoints);
	for(int i = 0; i < numPoints;i+=1){
		//path[i] = new orxVECTOR();
		path[i].fX = p[i*2];
		path[i].fY = p[i*2+1];
		//cout<<"Got x = "<<path[i].fX<<" y = "<<p[i+1]<<endl;
		path[i].fZ=0;
		time[i] = t;


	}
	//cout<<"finished "<<numPoints<<endl;
	free(p);
	pathInfo* info = new pathInfo();
	info->numPoints = numPoints;
	info->path = path;
	info->times = time;
	return info;
}
pathInfo* LevelSystem::generatePath(double* p, int numPoints){ //give it an array with x,y,time values and the number of points and it gives you a pathInfo pointer
	orxVECTOR* path = (orxVECTOR*) malloc(sizeof(orxVECTOR)*numPoints);
	int* time = (int*) malloc(sizeof(int)*numPoints);
	for(int i = 0; i < numPoints-3;i+=3){
		path[i].fX = p[i*2];
		path[i].fY = p[i*2+1];
		time[i] = p[i*2+2];
	}

	pathInfo* info = new pathInfo();
	info->numPoints = numPoints;
	info->path = path;
	info->times = time;
	return info;
}
//Code for Bezier calculations=======================================================
double LevelSystem::randFloat(int start, int end){
	return end - (double)rand()/((double)RAND_MAX/(end - start));
}

void LevelSystem::addJitter(double* p, int numPairs, double jitter){ //adds jitter to an array
	for(int i =0; i < numPairs; i +=1){
		double xJit = randFloat(0,jitter);//add or subtract a random amount of jitter
		double yJit = randFloat(0,jitter);
		if(rand()%2==1){
			xJit*=-1;
		}
		if(rand()%2==1){
			yJit*=-1;
		}
		p[i*2] += xJit;
		p[i*2+1] += yJit;
	}
}



int factorial (int a)
{
	if (a > 1)
		return (a * factorial (a-1));
	else
		return (1);
}

double Ni(int n, int i)
{
	double ni;
	//cout<<"attempting factorials"<<endl;
	double a1 = factorial(n);
	double a2 = factorial(i);
	double a3 = factorial(n - i);
	//cout<<"got factorials"<<endl;
	ni =  a1 / (a2 * a3);
	//cout<<"did math"<<endl;
	return ni;
}

// Calculate the Bernstein Advantage
double Bernstein(int n, int i, double t)
{
	double basis;
	double ti; /* t^i */
	double tni; /* (1 - t)^i */

	/* Prevent problems with pow */
	//cout<<"Initialized berstein"<<endl;
	if (t == 0.0 && i == 0) 
		ti = 1.0; 
	else 
		ti = pow(t, i);

	if (n == i && t == 1.0) 
		tni = 1.0; 
	else 
		tni = pow((1 - t), (n - i));

	//Bernstein basis
	//cout<<"Attempting basis"<<endl;
	basis = Ni(n, i) * ti * tni; 
	//cout<<"Got basis"<<endl;
	return basis;
}

//give this an array of points, the number of points in array, number of points to calculate, the array to store the results in
void LevelSystem::Bezier2D(double* b, int npts, int cpts, double* p)
{
	//int npts = (b.Length) / 2;
	int icount, jcount;
	double step, t;

	// Calculate points on curve

	icount = 0;
	t = 0;
	step = (double)1.0 / (cpts - 1);
	//cout<<"initialized Bezier2d"<<endl;
	for (int i1 = 0; i1 != cpts; i1++)
	{ 
		//cout<<"running "<<i1<<" times"<<endl;
		if ((1.0 - t) < 5e-6) 
			t = 1.0;

		jcount = 0;
		p[icount] = 0.0;
		p[icount + 1] = 0.0;
		for (int i = 0; i != npts; i++)
		{
			//cout<<"attempting bernstein"<<endl;
			double basis = Bernstein(npts - 1, i, t);
			//cout<<"did berstein"<<endl;
			p[icount] += basis * b[jcount];
			p[icount + 1] += basis * b[jcount + 1];
			jcount = jcount +2;
		}

		icount += 2;
		t += step;
	}
}
//End Bezier Code======================================================================

void LevelSystem::updateMultText(string text){
	orxObject_SetTextString(multText, text.c_str());
	//cout<<"UpdatedMultText called"<<endl;
}