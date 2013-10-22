#include <Windows.h>
#include "GameSystem.h"
#include "Unit.h" //holds our units
#include "Player.h"
#include "Bullet.h"
#include "Parser.h"
#include "HighScores.h"
#include <vector>
#include <iostream>
#include <string>

//using std::vector;

using namespace std;

GameSystem* GameSystem::m_Instance = NULL;
Boss* GameSystem::boss = NULL;
vector<Powerup*> GameSystem::powerups;
int GameSystem::tempI;

//Combo variables
int enemyComboMultiplier = 1;
#define COMBO_DEFAULT 30
int comboCounter = COMBO_DEFAULT;
int survivalMultiplier = 1;
int crystalComboMultiplier = 1;
#define CRYSTAL_COMBO_DEFAULT 60;
int crystalComboCounter = CRYSTAL_COMBO_DEFAULT;
vector<orxOBJECT*> points; // Vector containing visible points objects on screen
#define SURVIVAL_KILLS_PER_MULT_UP 5
short currKills;
//end combo vars

int qwer = 0;
int time = 0;
int lastHit = 0;
bool invuln = false;
bool fireButton = false; //Dont start off pressing these
bool fullScreen = false; //Dont start off pressing these
bool gameStarted = false; //game starts off not started
orxSOUND* pstMusic;
orxSOUND* annoying;
orxSOUND* eyecandy;
orxSOUND* enemyHit;
orxSOUND* airWin;
orxOBJECT* startBack;
orxOBJECT* startButt;
orxOBJECT* scoresButt;
orxOBJECT* scoresBack;
orxOBJECT* exitButt;
orxOBJECT* backButt;
orxOBJECT* scoreDisplay;
orxOBJECT* oScoreDisplay;
orxOBJECT* hallOfFameDisplay;
orxOBJECT* headersLevel;
orxOBJECT* headersName;
orxOBJECT* headersScore;
orxOBJECT* nameDisplay;
orxOBJECT* title;
//Unit *testing;
vector<Unit*> currentUnits;

string scoreString;
string oScoreString;
bool inGame,atStartScreen,nameEntry;
bool pushin[27];
string name;

Player* player;
float pSpeed = 350;
bool playerDeathFlag; // Marks if player is currently dead and waiting to respawn (see below)
int playerDeathCounter; // Marks time for waiting when player dies (time from when a player dies and level restarts)
bool fireFlag; //Players, enemies, and anything else can only fire if this flag is on

bool gameComplete;

GameSystem* GameSystem::Instance (){
	if( m_Instance != NULL )
	{
		return m_Instance;
	}

	m_Instance = new GameSystem();
	return m_Instance;
}

bool GameSystem::getFireFlag(){
	return fireFlag;
}

bool GameSystem::getPlayerDeathFlag(){
	return playerDeathFlag;
}

void GameSystem::setFireFlag(bool b){
	fireFlag = b;
}

/*
Default Constructer method
*/
GameSystem::GameSystem(){
	//THIS IS NEVER CALLED by us
}

orxSTATUS orxFASTCALL GameSystem::Run()
{
	return orxSTATUS_SUCCESS;
}


void orxFASTCALL GameSystem::Exit()
{

	//exit(0);

	return;
}

void GameSystem::addUnit(Unit* u){
	currentUnits.push_back(u);
}

Player* GameSystem::getPlayer(){
	return player;
}

void GameSystem::removeUnit(Unit* u){

	for(int i = 0; i < currentUnits.size(); i++){

		if(currentUnits.at(i) == u){
			delete currentUnits[i];

			currentUnits.erase(currentUnits.begin()+i);
			break;
		}
	}
}

void GameSystem::removePowerup(Powerup* p){
	for(int i = 0; i < powerups.size(); i++){

		if(powerups.at(i) == p){
			delete powerups[i];
			powerups.erase(powerups.begin()+i);

			break;
		}
	}
}

void GameSystem::clearUnits(){

	for(int i = 0; i < currentUnits.size(); i++){
		delete currentUnits[i];
	}

	currentUnits.clear();
}

void GameSystem::clearPowerups(){
	for(int i = 0; i < powerups.size(); i++){
		delete powerups[i];
	}

	powerups.clear();
}

bool GameSystem::getWeaponUnlock(int num){
	return player->isWeaponUnlocked(num);
}

orxSTATUS orxFASTCALL GameSystem::Init()
{

	// Our automatically loaded configuration files include the data to make our camera
	//   and our viewport, we need to initialise those here.
	// The viewport will in turn create the camera, so we only need the one line of code.
	view = orxViewport_CreateFromConfig( "Viewport" );
	orxConfig_Load( "Pattern.ini" );
	orxConfig_Load( "Unit.ini" );
	orxConfig_Load("Backgrounds.ini");
	orxConfig_Load("Clock.ini");
	orxConfig_Load("sound.ini");
	orxConfig_Load("UI.ini");
	orxConfig_Load("Powerup.ini");
	orxConfig_Load("PlayerBullets.ini");
	orxConfig_Load("EndLevelScreen.ini");
	orxConfig_Load("Hotbar.ini");
	//	orxConfig_Load("MainMenu.ini");

	pstMusic = orxSound_CreateFromConfig("Hello");
	eyecandy = orxSound_CreateFromConfig("Eyecandy");
	airWin = orxSound_CreateFromConfig("air");
	enemyHit = orxSound_CreateFromConfig("enemyHit");
	orxSound_Play(eyecandy);
	//make a menu here?

	//Initialize score display string
	scoreString = "";

	//Initialize boolean for starting game name entry
	nameEntry = false;
	name = "";
	for (int i = 0; i < 27; i++){
		pushin[i] = false;
	}

	//This is needed for exit key
	inGame = false;
	atStartScreen = true;


	//make the start background
	startBack = orxObject_CreateFromConfig("StartBackground");

	//make the buttons
	startButt = orxObject_CreateFromConfig("StartButton");
	scoresButt = orxObject_CreateFromConfig("ScoresButton");
	exitButt = orxObject_CreateFromConfig("ExitButton");

	//make the ui background
	title = orxObject_CreateFromConfig("title");

	orxCLOCK* gameClock = orxClock_CreateFromConfig("GameClock");
	orxClock_Register( gameClock, Update, 0, orxMODULE_ID_MAIN, orxCLOCK_PRIORITY_HIGHEST );


	orxEvent_AddHandler( orxEVENT_TYPE_ANIM, GameSystem::EventHandler );

	orxConfig_Load( "PlayerInput.ini" );

	orxInput_Load( orxSTRING_EMPTY );

	orxEvent_AddHandler( orxEVENT_TYPE_INPUT, GameSystem::EventHandler );

	orxConfig_Load( "Interaction.ini" );

	orxEvent_AddHandler( orxEVENT_TYPE_PHYSICS, GameSystem::EventHandler );

	//HighScores::submitHighScore("BEARORX",100000000,"STAGE 9-7");
	//HighScores::submitHighScore("CHRIS",80000000,"STAGE 3-3");
	//HighScores::submitHighScore("JACOB",70000000,"STAGE 3-2");
	//HighScores::submitHighScore("JON",60000000,"STAGE 3-1");
	//HighScores::submitHighScore("KELLY",50000000,"STAGE 2-3");
	//HighScores::submitHighScore("MARIA",40000000,"STAGE 2-2");
	//HighScores::submitHighScore("MARK",30000000,"STAGE 2-1");
	//HighScores::submitHighScore("MIKE",20000000,"STAGE 1-3");
	//HighScores::submitHighScore("LASER",10000000,"STAGE 1-2");
	//HighScores::submitHighScore("JACOBART",500000,"STAGE 1-1");


	playerDeathFlag = false;
	playerDeathCounter = 0;
	fireFlag = true;
	endOfLevel = false;
	gameComplete = false;
	return orxSTATUS_SUCCESS;
}

void GameSystem::clearGame(){
	std::string TempName;
	orxOBJECT* TempObject;

	TempObject = orxOBJECT( orxStructure_GetFirst( orxSTRUCTURE_ID_OBJECT ) );
	while(TempObject!=orxNULL){
		orxOBJECT* next = orxOBJECT( orxStructure_GetNext( TempObject ) ) ;
		// Get the current object's name,
		TempName = orxObject_GetName( TempObject );
		// Compare that to our bullet names

		// and delete the bullet
		//orxOBJECT* tmp = TempObject;
		orxObject_Delete(TempObject);

		TempObject = next;
	}
}

orxOBJECT* GameSystem::GetObjectByName( std::string objectName ){
	// This goes through the linked-list of all objects that have been loaded, and compares
	//   their name with whatever string we pass in, returning the first match,
	//   or a NULL if no match is found.
	std::string TempName;
	orxOBJECT* TempObject;

	for( // Grab the first object in our linked list, which is an orxOBJECT,
		TempObject = orxOBJECT( orxStructure_GetFirst( orxSTRUCTURE_ID_OBJECT ) );
		// Until we get one that's NULL,
		TempObject != orxNULL;
	// Continue onto the next object,
	TempObject = orxOBJECT( orxStructure_GetNext( TempObject ) ) )
	{
		// Get the current object's name,
		TempName = orxObject_GetName( TempObject );
		// Compare that to our input string (objectName),
		if( TempName.compare( objectName ) == 0 )
			// and return that object if they're the same.
			return TempObject;
	}

	// or return a NULL instead.
	return orxNULL;
}

void GameSystem::clearBullets(){
	std::string TempName;
	orxOBJECT* TempObject;

	TempObject = orxOBJECT( orxStructure_GetFirst( orxSTRUCTURE_ID_OBJECT ) );
	while(TempObject!=orxNULL){
		orxOBJECT* next = orxOBJECT( orxStructure_GetNext( TempObject ) ) ;
		// Get the current object's name,
		TempName = orxObject_GetName( TempObject );
		// Compare that to our bullet names
		if( TempName.compare( "blue" ) == 0 ||TempName.compare( "green" ) == 0||TempName.compare( "purple" ) == 0||TempName.compare( "blueOvalBullet" ) == 0||TempName.compare( "Missile1" ) == 0||TempName.compare( "Missile2" ) == 0||TempName.compare( "Missile3" ) == 0||TempName.compare( "Missile1Strong" ) == 0||TempName.compare( "Missile2Strong" ) == 0||TempName.compare( "Missile3Strong" ) == 0||TempName.compare( "shotBullet" ) == 0||TempName.compare( "MissileFragment" ) == 0 || TempName.compare("WaveBullet") == 0 ){
			// and delete the bullet
			//orxOBJECT* tmp = TempObject;
			orxObject_Delete(TempObject);
		}
		TempObject = next;
	}
}

orxSTATUS GameSystem::Setup(){


	return orxSTATUS_SUCCESS;
}

void GameSystem::clearPlayer(){
	player->~Player();
	//m_Instance->clearBullets();
	survivalMultiplier = 1;
	enemyComboMultiplier = 1;
	crystalComboMultiplier = 1;
	currKills = 0;
}

void GameSystem::restartPlayer(){
	player->restart();
	m_Instance->clearBullets();
	survivalMultiplier = 1;
	enemyComboMultiplier = 1;
	crystalComboMultiplier = 1;
	currKills = 0;
}

//returns true if an object at the given position and is a circle of radius size is off the screen
bool GameSystem::isOffScreen(orxVECTOR* pos, float size){
	if(pos->fX+size>SCREEN_MAX_X || pos->fX-size<SCREEN_MIN_X || pos->fY+size>SCREEN_MAX_Y || pos->fY-size<SCREEN_MIN_Y){
		return true;
	}
	return false;
}

void orxFASTCALL GameSystem::Update( const orxCLOCK_INFO* clockInfo, void* context ){
	if(nameEntry){
		nameEntrySequence();

		//Update String text
		orxObject_SetTextString(nameDisplay,("Name Entry: " + name).c_str());
	}

	if(gameStarted){
		//int i = 0;
		//int tempI; //how to remove from update loop? get non-static var from static stuff errors
		time++;

		//update current weapon in hotbar
		int wep = player->getCurrentWeapon();
		lvlSys->setHotbar(wep);

		//Wait and Reload Level ============================================
		if(playerDeathFlag){

			playerDeathCounter++;

			if(playerDeathCounter >= 240){
				playerDeathCounter = 0;

				playerDeathFlag = false;
				lvlSys->highLoadLevel();
				player->resetInventory();
				fireFlag = true;
			}
		}
		//END Wait and Reload Level ========================================

		lvlSys->update();

		if(!gameComplete){

			//update all of the points objects on screen
			for(tempI = 0; tempI < points.size(); tempI++){
				orxCOLOR alphaL;
				orxObject_GetColor(points[tempI], &alphaL);
				if(alphaL.fAlpha > 0){
					alphaL.fAlpha-=.015;
					orxObject_SetColor(points[tempI], &alphaL); //sets dimmer alpha level
				}else{
					orxOBJECT* p = points.at(tempI);
					points.erase(points.begin()+tempI);
					orxObject_Delete(p);
					//removes it from the vector and deletes the object itself
				}
			}

			//handle the enemy combo multiplier
			comboCounter--; //decrement it every update
			if(comboCounter == 0){
				if(enemyComboMultiplier > 1){
					enemyComboMultiplier--;
				}
				comboCounter = COMBO_DEFAULT;
			}

			//handle the crystal combo multiplier
			crystalComboCounter--;
			if(crystalComboCounter == 0){
				if(crystalComboMultiplier > 1){
					crystalComboMultiplier--;
				}
				crystalComboCounter = CRYSTAL_COMBO_DEFAULT;
			}	

			//update every unit
			for(tempI = 0; tempI < currentUnits.size(); tempI++){
				currentUnits[tempI]->Update(player->GetPos());
			}

			//kill everything past bottom of screen
			for(tempI =0;tempI<currentUnits.size();tempI++){
				if(currentUnits[tempI]->GetPos().fY>375){
					m_Instance->removeUnit(currentUnits[tempI]);
				}
			}
			for(tempI=0;tempI<powerups.size();tempI++){
				orxVECTOR* pupPos=powerups.at(tempI)->getPosition();

				if(m_Instance->isOffScreen(powerups.at(tempI)->getPosition(),POWERUP_SIZE) ){
					m_Instance->removePowerup(powerups.at(tempI));
				}
			}


			//hit cooldown code
			if(invuln&&lastHit==0){
				lastHit=time;
				player->SetInvuln(invuln);
			} else if(time-lastHit>90&&lastHit!=0){
				lastHit = 0;
				invuln = false;
				player->SetInvuln(invuln);
			}
			player->Update(fireFlag);
		}else{
			//Game complete goes here
			m_Instance->escapeGame();
			m_Instance->goToScoreScreen();
		}
	}
}

orxVECTOR orxFASTCALL GameSystem::GetMouseWorldPosition(){
	orxVECTOR MouseWorldPosition = { 0.0, 0.0, 0.0 };
	orxVECTOR MouseScreenPosition = { 0.0, 0.0, 0.0 };

	orxRender_GetWorldPosition( orxMouse_GetPosition( &MouseScreenPosition ), &MouseWorldPosition );

	return MouseWorldPosition;
}

void GameSystem::nameEntrySequence(){
	if(gameComplete){
		name = "GODMODE";
		gameComplete = false;
	}
	if(name.length() < MAXNAMELENGTH){
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_A) && !pushin[0]){
			name += "A";
			pushin[0] = true;
		}
		else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_A) && pushin[0]) {
			pushin[0] = false;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_B) && !pushin[1]){
			name += "B";
			pushin[1] = true;
		}
		else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_B) && pushin[1]) {
			pushin[1] = false;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_C) && !pushin[2]){
			name += "C";
			pushin[2] = true;
		}
		else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_C) && pushin[2]) {
			pushin[2] = false;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_D) && !pushin[3]){
			name += "D";
			pushin[3] = true;
		}
		else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_D) && pushin[3]) {
			pushin[3] = false;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_E) && !pushin[4]){
			name += "E";
			pushin[4] = true;
		}
		else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_E) && pushin[4]) {
			pushin[4] = false;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_F) && !pushin[5]){
			name += "F";
			pushin[5] = true;
		}
		else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_F) && pushin[5]) {
			pushin[5] = false;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_G) && !pushin[6]){
			name += "G";
			pushin[6] = true;
		}
		else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_G) && pushin[6]) {
			pushin[6] = false;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_H) && !pushin[7]){
			name += "H";
			pushin[7] = true;
		}
		else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_H) && pushin[7]) {
			pushin[7] = false;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_I) && !pushin[8]){
			name += "I";
			pushin[8] = true;
		}
		else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_I) && pushin[8]) {
			pushin[8] = false;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_J) && !pushin[9]){
			name += "J";
			pushin[9] = true;
		}
		else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_J) && pushin[9]) {
			pushin[9] = false;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_K) && !pushin[10]){
			name += "K";
			pushin[10] = true;
		}
		else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_K) && pushin[10]) {
			pushin[10] = false;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_L) && !pushin[11]){
			name += "L";
			pushin[11] = true;
		}
		else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_L) && pushin[11]) {
			pushin[11] = false;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_M) && !pushin[12]){
			name += "M";
			pushin[12] = true;
		}
		else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_M) && pushin[12]) {
			pushin[12] = false;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_N) && !pushin[13]){
			name += "N";
			pushin[13] = true;
		}
		else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_N) && pushin[13]) {
			pushin[13] = false;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_O) && !pushin[14]){
			name += "O";
			pushin[14] = true;
		}
		else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_O) && pushin[14]) {
			pushin[14] = false;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_P) && !pushin[15]){
			name += "P";
			pushin[15] = true;
		}
		else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_P) && pushin[15]) {
			pushin[15] = false;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_Q) && !pushin[16]){
			name += "Q";
			pushin[16] = true;
		}
		else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_Q) && pushin[16]) {
			pushin[16] = false;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_R) && !pushin[17]){
			name += "R";
			pushin[17] = true;
		}
		else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_R) && pushin[17]) {
			pushin[17] = false;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_S) && !pushin[18]){
			name += "S";
			pushin[18] = true;
		}
		else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_S) && pushin[18]) {
			pushin[18] = false;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_T) && !pushin[19]){
			name += "T";
			pushin[19] = true;
		}
		else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_T) && pushin[19]) {
			pushin[19] = false;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_U) && !pushin[20]){
			name += "U";
			pushin[20] = true;
		}
		else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_U) && pushin[20]) {
			pushin[20] = false;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_V) && !pushin[21]){
			name += "V";
			pushin[21] = true;
		}
		else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_V) && pushin[21]) {
			pushin[21] = false;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_W) && !pushin[22]){
			name += "W";
			pushin[22] = true;
		}
		else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_W) && pushin[22]) {
			pushin[22] = false;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_X) && !pushin[23]){
			name += "X";
			pushin[23] = true;
		}
		else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_X) && pushin[23]) {
			pushin[23] = false;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_Y) && !pushin[24]){
			name += "Y";
			pushin[24] = true;
		}
		else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_Y) && pushin[24]) {
			pushin[24] = false;
		}
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_Z) && !pushin[25]){
			name += "Z";
			pushin[25] = true;
		}
		else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_Z) && pushin[25]) {
			pushin[25] = false;
		}
	}

	if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_BACKSPACE) && !pushin[26]){
		name = name.substr(0,name.length()-1);
		pushin[26] = true;
	}
	else if (!orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_BACKSPACE) && pushin[26]) {
		pushin[26] = false;
	}

	//If they hit enter, that's it! @$#*(%$(%)#$*)% them, time to start the game, their fault
	if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_NUMPAD_RETURN) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_RETURN) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_F12)){
		//TODO Erase name entry field
		nameEntry = false;
		orxObject_Delete(nameDisplay);
		startGame();
		gameComplete = false;
		//name += "A";
		//pushin[0] = true;
	}
}

void GameSystem::startGame(){
	//Getting into the game
	inGame = true;
	atStartScreen = false;

	player = new Player(m_Instance);
	player->YouAreAGod(name);
	orxObject_Delete(startButt);
	orxObject_Delete(scoresButt);
	orxObject_Delete(exitButt);
	orxObject_Delete(startBack);
	orxObject_Delete(title);
	gameStarted = true;

	//Turn off mouse for game
	orxMouse_ShowCursor(false);

	lvlSys = new LevelSystem(m_Instance);
	orxSound_Stop(eyecandy);
	orxSound_Stop(airWin);

	//Display multiplier for the first time
	lvlSys->updateMultText("X1");
}

orxSTATUS orxFASTCALL GameSystem::EventHandler( const orxEVENT* currentEvent ){

	if (!nameEntry) {
		if(orxInput_IsActive("ARROW_LEFT") && player != NULL){
			player->setLeft(pSpeed);
		}else if(player != NULL){
			player->setLeft(0);
		}

		if(orxInput_IsActive("ARROW_RIGHT") && player != NULL){
			player->setRight(pSpeed);
		}else if(player != NULL){
			//player->setRight(0);
		}

		if(orxInput_IsActive("ARROW_UP") && player != NULL){
			player->setUp(pSpeed);
		}else if(player != NULL){
			player->setUp(0);
		}

		if(orxInput_IsActive("ARROW_DOWN") && player != NULL){
			player->setDown(pSpeed);
		}else if(player != NULL){
			//player->setDown(0);
		}
		if(orxInput_IsActive("startGame")){
			//if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_N)){
			// Here we grab the mouse position,
			orxVECTOR MouseWorldPosition = GetMouseWorldPosition();
			//test .fx and .fy to be in a range where the button is, then start game
			orxOBJECT* PickedObject = orxObject_Pick( &MouseWorldPosition );

			//if(!gameStarted){
			if(PickedObject == startButt){

				nameDisplay = orxObject_CreateFromConfig("NameEntryDisplay");


				nameEntry = true;
			}
			else if (PickedObject == scoresButt){
				m_Instance->goToScoreScreen();

			}
			else if (PickedObject == exitButt){
				TerminateProcess(GetCurrentProcess(), 0); // MURDER WITH WINDOWS FUNCTIONS!
			}
			else if (PickedObject == backButt){
				atStartScreen = true;
				//Delete Stuff that is in the high score screen
				orxObject_Delete(scoresBack);
				orxObject_Delete(scoreDisplay);
				orxObject_Delete(oScoreDisplay);
				orxObject_Delete(headersLevel);
				orxObject_Delete(headersName);
				orxObject_Delete(headersScore);
				orxObject_Delete(hallOfFameDisplay);
				orxObject_Delete(backButt);

				//remake the start background
				startBack = orxObject_CreateFromConfig("StartBackground");
				title = orxObject_CreateFromConfig("title");

				//remake the buttons
				startButt = orxObject_CreateFromConfig("StartButton");
				scoresButt = orxObject_CreateFromConfig("ScoresButton");
				exitButt = orxObject_CreateFromConfig("ExitButton");

			}
		}
		//Flips firing on and off on key press
		if((orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_SPACE) || (orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_LCTRL))) && player != NULL){
			if (!fireButton){ //If pressed, but not held down
				fireButton = true; //Woo, it's being held down now
				player->setFiring(!player->getFiring()); //This will not fast toggle, I can prove by contradiction if you need one
			}
		}
		else
			fireButton = false; //No longer pressing

		if(orxInput_IsActive("EXIT")){
			//Exit Game
			//Parser *p = new Parser();
			//p->Write();

			m_Instance->escapeGame();
		}

		//Fullscreen full screen key set
		/*if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_TILDE)){
		//orxFLOAT w = 1000;
		//orxFLOAT h = 700;
		//cout<<w<<"  "<<h<<endl;
		//orxDisplay_GetScreenSize(&w,&h);
		//cout<<w<<"  "<<h<<endl;

		orxDisplay_SetFullScreen(!orxDisplay_IsFullScreen());
		//orxDisplay_SetHeight(700);
		}*/
		if(orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_F4)){
			if (!fullScreen){ //If pressed, but not held down
				fullScreen = true; //Woo, it's being held down now, and we can detect that!
				if(!orxDisplay_IsFullScreen()){
					orxViewport_Delete(view);
					orxDisplay_SetFullScreen(!orxDisplay_IsFullScreen()); //This will not fast toggle, I can prove by contradiction if you need one
					view = orxViewport_CreateFromConfig( "Viewport" );
				}else{
					orxDisplay_SetFullScreen(!orxDisplay_IsFullScreen());
					orxDISPLAY_VIDEO_MODE dvm = orxDISPLAY_VIDEO_MODE();
					dvm.u32Width = 1000;
					dvm.u32Height = 700;
					dvm.u32Depth = 0;
					orxDisplay_SetVideoMode(&dvm);
					orxViewport_Delete(view);
					view = orxViewport_CreateFromConfig( "Viewport" );
				}
				orxMouse_ShowCursor(true);
			}
		}
		else
			fullScreen = false; //No longer pressing



		//if(orxInput_IsActive("SLOW")){
		//	pSpeed = 75;
		//}else{//y u no werk?
		//	pSpeed = 300;
		//}

	}

	switch( currentEvent->eType ){
		// This is for animation events, the only ones we handle in tutorial 3.
	case orxEVENT_TYPE_ANIM:{
		// Okay so we know we're dealing with an animation, lets grab the 'payload' out of the event.
		orxANIM_EVENT_PAYLOAD* EventPayload = (orxANIM_EVENT_PAYLOAD*)currentEvent->pstPayload;

		// We want the name of the object who called this event.
		//   (In tutorial 3, this will always be 'SoldierGraphics'.)
		const orxCHAR* ObjectName = orxObject_GetName( orxOBJECT( currentEvent->hRecipient ) );

		// And finally the action which is this event.
		const orxCHAR* EventAct;

		switch( currentEvent->eID ){
		case orxANIM_EVENT_START: EventAct = "started"; break;
		case orxANIM_EVENT_STOP: EventAct = "stopped"; break;
		case orxANIM_EVENT_CUT: EventAct = "been cut"; break;
			//case orxANIM_EVENT_LOOP: EventAct = "looped"; break; // We comment this line out to avoid some hefty console spam.
		default: return orxSTATUS_SUCCESS; // Here we return early to avoid trying to access something we've not specifically set or loaded.
		}

		// Now we'll output this nicely to the console, so we can see exactly what animations
		//   are being called, on what objects, and what they're doing.
		orxLOG("Animation <%s>@<%s> has %s!", EventPayload->zAnimName, ObjectName, EventAct );
		break;


							}
							// Here we add event handling for tutorial 4, Input!
	case orxEVENT_TYPE_INPUT:{
		orxINPUT_EVENT_PAYLOAD* EventPayload = (orxINPUT_EVENT_PAYLOAD*)currentEvent->pstPayload;

		const orxCHAR* EventAct;

		switch( currentEvent->eID ){
		case orxINPUT_EVENT_ON:{
			EventAct = "pressed down";
			break;
							   }

		case orxINPUT_EVENT_OFF: EventAct = "released"; break;
		default: return orxSTATUS_SUCCESS; // Remember to 'early return' here, as there are more than just the two events we've specifically handled right now.
		}


		break;
							 }
							 // And physics events too!
	case orxEVENT_TYPE_PHYSICS:
		{
			ObjectInfo *sender=(ObjectInfo*)orxObject_GetUserData(orxOBJECT(currentEvent->hSender));
			ObjectInfo *recipient=(ObjectInfo*)orxObject_GetUserData(orxOBJECT(currentEvent->hRecipient));

			m_Instance->HandleCollision(sender,recipient);
			break;
		}

	}

	return orxSTATUS_SUCCESS;
	//}
	//}
}

void GameSystem::escapeGame(){
	if (inGame == true) { //We are in the game

		if (fireFlag){

			//TODO Input High Score, because you are sooooooo high
			if (lvlSys->scoreAtStart > 0 && HighScores::isHighScore(lvlSys->scoreAtStart) && !player->YouAreAGod(name)){
				HighScores::submitHighScore(name, lvlSys->scoreAtStart, lvlSys->getLevelName());
			}

			//cout<<"Clearandresetgo"<<endl;
			delete lvlSys;
			lvlSys = NULL;
			gameStarted = false;
			//cout<<"STOOOOOOOOP"<<endl;
			inGame = false; //No Longer in game

			delete player;

			for (int i = 0; i < points.size(); i++){
				orxObject_Delete(points.at(i));
			}

			//Restart main menu music
			if(!gameComplete){
				orxSound_Play(eyecandy);
			}else{
				orxSound_Play(airWin);
			}


			name = "";

			if (!atStartScreen) { //Dont want to recreate multiple menus
				//remake the start background
				startBack = orxObject_CreateFromConfig("StartBackground");

				//remake the buttons
				startButt = orxObject_CreateFromConfig("StartButton");
				scoresButt = orxObject_CreateFromConfig("ScoresButton");
				exitButt = orxObject_CreateFromConfig("ExitButton");

				orxMouse_ShowCursor(true);

				//remake anything else
				title  = orxObject_CreateFromConfig("title");
				atStartScreen = true;
			}

		}
	}else {

		name = "";

		if (GetObjectByName("BackButton") != orxNULL){ //If we are in the score screen
			atStartScreen = true;
			//Delete Stuff that is in the high score screen
			orxObject_Delete(scoresBack);
			orxObject_Delete(scoreDisplay);
			orxObject_Delete(oScoreDisplay);
			orxObject_Delete(backButt);
			orxObject_Delete(headersLevel);
			orxObject_Delete(headersName);
			orxObject_Delete(headersScore);
			orxObject_Delete(hallOfFameDisplay);

			//remake the start background
			startBack = orxObject_CreateFromConfig("StartBackground");

			//remake the buttons
			startButt = orxObject_CreateFromConfig("StartButton");
			scoresButt = orxObject_CreateFromConfig("ScoresButton");
			exitButt = orxObject_CreateFromConfig("ExitButton");

			//remake anything else
			title  = orxObject_CreateFromConfig("title");
		}
		else if (!atStartScreen) { //Dont want to recreate multiple menus

			//remake the start background
			startBack = orxObject_CreateFromConfig("StartBackground");

			//remake the buttons
			startButt = orxObject_CreateFromConfig("StartButton");
			scoresButt = orxObject_CreateFromConfig("ScoresButton");
			exitButt = orxObject_CreateFromConfig("ExitButton");

			orxMouse_ShowCursor(true);

			//remake anything else
			title  = orxObject_CreateFromConfig("title");
			atStartScreen = true;
		}
	}
}

/*
* Handle collisions between 2 objects.
*/
void GameSystem::HandleCollision(ObjectInfo* obj1, ObjectInfo* obj2){
	if(obj1!=NULL && obj2!=NULL){

		//Player bullet and enemy unit collide
		if(obj1->type==UNIT && obj2->type==BULLET){
			string bulletName(orxObject_GetName( ((Bullet*)obj2->object)->bullet ));
			if(bulletName.compare("Missile1")==0){
				missileExplode((Bullet*)obj2->object,0);
			}else if(bulletName.compare("Missile2")==0){
				missileExplode((Bullet*)obj2->object,2);
			}else if(bulletName.compare("Missile3")==0){
				missileExplode((Bullet*)obj2->object,4);
			}else if(bulletName.compare("Missile1Strong")==0){
				missileExplode((Bullet*)obj2->object,1);
			}else if(bulletName.compare("Missile2Strong")==0){
				missileExplode((Bullet*)obj2->object,3);
			}else if(bulletName.compare("Missile3Strong")==0){
				missileExplode((Bullet*)obj2->object,5);
			}

			orxObject_Delete(((Bullet*)obj2->object)->bullet);

			UnitHit((Unit*)obj1->object,((Bullet*)obj2->object)->getDamage());
		}
		else if(obj2->type==UNIT && obj1->type==BULLET){
			string bulletName(orxObject_GetName( ((Bullet*)obj1->object)->bullet ));
			if(bulletName.compare("Missile1")==0){
				missileExplode((Bullet*)obj1->object,0);
			}else if(bulletName.compare("Missile2")==0){
				missileExplode((Bullet*)obj1->object,2);
			}else if(bulletName.compare("Missile3")==0){
				missileExplode((Bullet*)obj1->object,4);
			}else if(bulletName.compare("Missile1Strong")==0){
				missileExplode((Bullet*)obj1->object,1);
			}else if(bulletName.compare("Missile2Strong")==0){
				missileExplode((Bullet*)obj1->object,3);
			}else if(bulletName.compare("Missile3Strong")==0){
				missileExplode((Bullet*)obj1->object,5);
			}

			orxObject_Delete(((Bullet*)obj1->object)->bullet);

			UnitHit((Unit*)obj2->object,((Bullet*)obj1->object)->getDamage());

		}

		//Enemy bullet and player collide
		else if(obj1->type==BULLET && obj2->type==PLAYER){
			//handle enemy bullet and player ship collisions
			if(!invuln){
				if(!playerDeathFlag){orxSound_Play(pstMusic);}
				orxObject_Delete(((Bullet*)obj1->object)->bullet);
				if(fireFlag){
					lvlSys->modScore(-POINTS_LOST_ON_HIT);
				}
				invuln = true;
				if(((Player*)obj2->object)->Hit(1)==0){
					((Player*)obj2->object)->Die();
					playerDeathFlag = true;
					fireFlag = false;
				}
				((Player*)obj2->object)->changeCrystalCount(CRYSTALS_LOST_ON_HIT);
				lvlSys->setRankBadge(((Player*)obj2->object)->getWeaponRank());

				//manage multiplier stuff
				currKills = 0;
				survivalMultiplier = 1;
				lvlSys->updateMultText("X1");
			}
		}
		else if((obj2->type==BULLET && obj1->type==PLAYER)){

			if(!invuln){
				if(!playerDeathFlag){orxSound_Play(pstMusic);}
				orxObject_Delete(((Bullet*)obj2->object)->bullet);
				if(fireFlag){
					lvlSys->modScore(-POINTS_LOST_ON_HIT);
				}
				invuln = true;
				if(((Player*)obj1->object)->Hit(1)==0){
					((Player*)obj1->object)->Die();
					playerDeathFlag = true;
					fireFlag = false;
				}
				((Player*)obj1->object)->changeCrystalCount(CRYSTALS_LOST_ON_HIT);
				lvlSys->setRankBadge(((Player*)obj1->object)->getWeaponRank());

				//manage multiplier stuff
				currKills = 0;
				survivalMultiplier = 1;
				lvlSys->updateMultText("X1");
			}
		}

		//enemy unit collides with player
		else if((obj2->type==UNIT && obj1->type==PLAYER)){
			if(!invuln){
				//orxObject_Delete(((Unit*)obj2->object)->unit);
				if(!playerDeathFlag){orxSound_Play(pstMusic);}
				if(fireFlag){
					lvlSys->modScore(-POINTS_LOST_ON_HIT);
				}
				invuln = true;
				if(((Player*)obj1->object)->Hit(1)==0){
					((Player*)obj1->object)->Die();
					playerDeathFlag = true;
					fireFlag = false;
				}
				((Player*)obj1->object)->changeCrystalCount(CRYSTALS_LOST_ON_HIT);
				lvlSys->setRankBadge(((Player*)obj1->object)->getWeaponRank());

				//manage multiplier stuff
				currKills = 0;
				survivalMultiplier = 1;
				lvlSys->updateMultText("X1");
			}
		}
		else if((obj2->type==PLAYER && obj1->type==UNIT)){

			if(!invuln){
				//orxObject_Delete(((Unit*)obj2->object)->unit);
				if(!playerDeathFlag){orxSound_Play(pstMusic);}
				if(fireFlag){
					lvlSys->modScore(-POINTS_LOST_ON_HIT);
				}
				invuln = true;
				if(((Player*)obj2->object)->Hit(1)==0){
					((Player*)obj2->object)->Die();
					playerDeathFlag = true;
					fireFlag = false;
				}
				((Player*)obj2->object)->changeCrystalCount(CRYSTALS_LOST_ON_HIT);
				lvlSys->setRankBadge(((Player*)obj2->object)->getWeaponRank());

				//manage multiplier stuff
				currKills = 0;
				survivalMultiplier = 1;
				lvlSys->updateMultText("X1");
			}
		}
		else if((obj1->type==PLAYER && obj2->type==POWERUP)){
			if(((Powerup*)obj2->object)->getType()==WEAPON_UNLOCK){
				((Player*)obj1->object)->unlockWeapon(((Powerup*)obj2->object)->getValue());

				lvlSys->modScore(WEAPON_UNLOCK_POINT_VAL); //add weapon unlock points to score

				//handle points objects
				points.push_back(((Powerup*)obj2->object)->spawnPointObj(WEAPON_UNLOCK_POINT_VAL));
				//end handle points objects

				removePowerup((Powerup*)obj2->object);
			}
			else if(((Powerup*)obj2->object)->getType()==WEAPON_RANK_UP){
				((Player*)obj1->object)->upgradeWeapon(((Powerup*)obj2->object)->getValue());

				lvlSys->modScore(WEAPON_RANK_UP_POINT_VAL); //add weapon rank up points to score

				//handle points objects
				points.push_back(((Powerup*)obj2->object)->spawnPointObj(WEAPON_RANK_UP_POINT_VAL));
				//end handle points objects

				removePowerup((Powerup*)obj2->object);
			}
			else if(((Powerup*)obj2->object)->getType()==CRYSTAL){
				((Player*)obj1->object)->changeCrystalCount(((Powerup*)obj2->object)->getValue());
				//check rank and change the rank badge
				lvlSys->setRankBadge(((Player*)obj1->object)->getWeaponRank());

				lvlSys->modScore(CRYSTAL_POINT_VAL*crystalComboMultiplier*survivalMultiplier); //add crystal points to score

				//handle points objects
				points.push_back(((Powerup*)obj2->object)->spawnPointObj(CRYSTAL_POINT_VAL*crystalComboMultiplier*survivalMultiplier));
				//end handle points objects
				removePowerup((Powerup*)obj2->object);

				//increment crystal combo multiplier
				crystalComboMultiplier++;
			}
			else if(((Powerup*)obj2->object)->getType()==HEALTH){
				((Player*)obj1->object)->heal(((Powerup*)obj2->object)->getValue());
				removePowerup((Powerup*)obj2->object);
			}
		}
		else if((obj2->type==PLAYER && obj1->type==POWERUP)){
			if(((Powerup*)obj1->object)->getType()==WEAPON_UNLOCK){
				((Player*)obj2->object)->unlockWeapon(((Powerup*)obj1->object)->getValue());

				lvlSys->modScore(WEAPON_UNLOCK_POINT_VAL); //add weapon unlock points to score

				//handle points objects
				points.push_back(((Powerup*)obj1->object)->spawnPointObj(WEAPON_UNLOCK_POINT_VAL));
				//end handle points objects

				removePowerup((Powerup*)obj1->object);
			}
			else if(((Powerup*)obj1->object)->getType()==WEAPON_RANK_UP){
				((Player*)obj2->object)->upgradeWeapon(((Powerup*)obj1->object)->getValue());

				lvlSys->modScore(WEAPON_RANK_UP_POINT_VAL); //add weapon rank up points to score

				//handle points objects
				points.push_back(((Powerup*)obj1->object)->spawnPointObj(WEAPON_RANK_UP_POINT_VAL));
				//end handle points objects

				removePowerup((Powerup*)obj1->object);
			}
			else if(((Powerup*)obj1->object)->getType()==CRYSTAL){
				((Player*)obj2->object)->changeCrystalCount(((Powerup*)obj1->object)->getValue());
				//check rank and change the rank badge
				lvlSys->setRankBadge(((Player*)obj2->object)->getWeaponRank());

				lvlSys->modScore(CRYSTAL_POINT_VAL*crystalComboMultiplier*survivalMultiplier); //add crystal points to score

				//handle points objects
				points.push_back(((Powerup*)obj1->object)->spawnPointObj(CRYSTAL_POINT_VAL*crystalComboMultiplier*survivalMultiplier));
				//end handle points objects
				removePowerup((Powerup*)obj1->object);

				//increment crystal combo multiplier
				crystalComboMultiplier++;
			}
			else if(((Powerup*)obj1->object)->getType()==HEALTH){
				((Player*)obj2->object)->heal(((Powerup*)obj1->object)->getValue());
				removePowerup((Powerup*)obj1->object);
			}
		}
	}
}

/* unit is the unit being hit
* damage is the damage being done to the unit
*/
void GameSystem::UnitHit(Unit* unit, double damage){
	//orxSound_Play(annoying);
	orxSound_Play(enemyHit);

	//check to see if the unit is a boss part
	if(unit->getPartIndex()>=0){
		if(boss->Hit( unit , damage)==0){
			//TODO do cool stuff when the boss dies
			delete boss;
		}
	}
	else if(unit->Hit(damage)==0){
		//spawn a powerup if necessary
		if(unit->getWeaponUnlocked()>=0){
			m_Instance->spawnRandomPowerup(unit->GetPos(),WEAPON_UNLOCK,unit->getWeaponUnlocked());
		}
		//randomly spawn a powerup
		else if(rand()%100<POWERUP_PROBABILITY){
			m_Instance->spawnRandomPowerup(unit->GetPos(),CRYSTAL);//TODO allow other powerups to spawn
		}

		//handle points objects
		points.push_back(unit->spawnPointObj(enemyComboMultiplier*survivalMultiplier));
		//end handle points objects

		lvlSys->modScore(unit->getScoreValue()*enemyComboMultiplier*survivalMultiplier);
		lvlSys->enemyKilled();
		removeUnit(unit);

		currKills++;
		if(currKills%SURVIVAL_KILLS_PER_MULT_UP == 0){
			currKills = 0;
			survivalMultiplier++;
			stringstream updateText;
			updateText << "X" << survivalMultiplier;
			lvlSys->updateMultText(updateText.str());
			//cout<<"SurvivalMultiplier: "<<survivalMultiplier<<endl;
		}
		enemyComboMultiplier++; //increment enemy combo multiplier
		//cout<<"EnemyComboMultiplier: "<<enemyComboMultiplier<<endl;
	}
}

//spawns a random powerup at the given point
//you can specify the type of powerup or it's value if those are known
//health powerups default to healing 1 health
void GameSystem::spawnRandomPowerup(orxVECTOR pos, int type, int v){
	orxVECTOR powerupSpeed={0,30,0};
	if(type==-1){
		type=(int)(rand()%NUM_POWERUP_TYPES);
	}

	switch(type){	
	case WEAPON_UNLOCK:
		if(v==-1){
			v=(rand()%NUMBER_OF_PLAYER_WEAPONS);
		}
		m_Instance->powerups.push_back( new Powerup(WEAPON_UNLOCK,v,pos,powerupSpeed));
		break;
	case WEAPON_RANK_UP:
		if(v<=0){
			v=1;
		}
		m_Instance->powerups.push_back( new Powerup(WEAPON_RANK_UP,v,pos,powerupSpeed));
		break;
	case CRYSTAL:
		if(v<=0){
			v=1;
		}
		m_Instance->powerups.push_back( new Powerup(CRYSTAL,v,pos,powerupSpeed));
		break;

	case HEALTH:
		if(v<=0){
			v=1;
		}
		m_Instance->powerups.push_back( new Powerup(HEALTH,v,pos,powerupSpeed));
		break;
	}
}

void GameSystem::missileExplode(Bullet* missile, int useNumber){
	orxVECTOR tempVec;
	Bullet* tempBullet;
	int movement;
	int currentBullet = 0;
	double step = 0;
	orxVECTOR* angle = new orxVECTOR();

	angle->fZ = 0;
	if(useNumber == 0){ // Level 1 - Standard
		int numberOfBullets = BASEDAMAGEPERSECOND*(MISSILE_COOLDOWN/60.0);
		while(currentBullet < numberOfBullets){

			Bullet* tempBullet = new Bullet(missile->GetPos(),"MissileFragment",1);

			step = ((2*3.14159)/50.0);
			step = step*(rand()%50);
			angle->fY = sin(step); //the math steps for unit circle
			angle->fX = cos(step);
			int randC = rand();

			if(randC <= (RAND_MAX*(1.0/3.0))){
				movement = MISSILE_FRAGMENT_SPEED1;
			}else if(randC <= (RAND_MAX*(2.0/3.0))){
				movement = MISSILE_FRAGMENT_SPEED2;
			}else{
				movement = MISSILE_FRAGMENT_SPEED3;
			}



			orxVector_Mulf(angle,angle,movement);
			tempBullet->SetSpeed(*angle);
			currentBullet++;

		}
	}else if(useNumber == 1){ // Level 1 - Strong
		int numberOfBullets = BASEDAMAGEPERSECOND*DAMAGEMULTSTRONG*(MISSILE_COOLDOWN/60.0)+BASEDAMAGEPERSECOND*DAMAGEMULTSTRONG*(MAXCHARGE/150.0);
		while(currentBullet < numberOfBullets){

			Bullet* tempBullet = new Bullet(missile->GetPos(),"MissileFragment",1);

			step = ((2*3.14159)/50.0);
			step = step*(rand()%50);
			angle->fY = sin(step); //the math steps for unit circle
			angle->fX = cos(step);
			int randC = rand();

			if(randC <= (RAND_MAX*(1.0/3.0))){
				movement = MISSILE_FRAGMENT_SPEED1;
			}else if(randC <= (RAND_MAX*(2.0/3.0))){
				movement = MISSILE_FRAGMENT_SPEED2;
			}else{
				movement = MISSILE_FRAGMENT_SPEED3;
			}



			orxVector_Mulf(angle,angle,movement);
			tempBullet->SetSpeed(*angle);
			currentBullet++;

		}
	}else if(useNumber == 2){ // Level 2 - Standard 
		int numberOfBullets = BASEDAMAGEPERSECOND*DAMAGEMULTPERRANK*(MISSILE_COOLDOWN/60.0);
		while(currentBullet < numberOfBullets){

			Bullet* tempBullet = new Bullet(missile->GetPos(),"MissileFragment",2);


			step = ((2*3.14159)/50.0);
			step = step*(rand()%50);
			angle->fY = sin(step); //the math steps for unit circle
			angle->fX = cos(step);
			int randC = rand();

			if(randC <= (RAND_MAX*(1.0/3.0))){
				movement = MISSILE_FRAGMENT_SPEED1;
			}else if(randC <= (RAND_MAX*(2.0/3.0))){
				movement = MISSILE_FRAGMENT_SPEED2;
			}else{
				movement = MISSILE_FRAGMENT_SPEED3;
			}



			orxVector_Mulf(angle,angle,movement);
			tempBullet->SetSpeed(*angle);
			currentBullet++;
		}
	}else if(useNumber == 3){ // Level 2 - Strong
		int numberOfBullets = BASEDAMAGEPERSECOND*DAMAGEMULTPERRANK*DAMAGEMULTSTRONG*(MISSILE_COOLDOWN/60.0)+BASEDAMAGEPERSECOND*DAMAGEMULTPERRANK*DAMAGEMULTSTRONG*(MAXCHARGE/150.0);
		while(currentBullet < numberOfBullets){

			Bullet* tempBullet = new Bullet(missile->GetPos(),"MissileFragment",2);


			step = ((2*3.14159)/50.0);
			step = step*(rand()%50);
			angle->fY = sin(step); //the math steps for unit circle
			angle->fX = cos(step);
			int randC = rand();

			if(randC <= (RAND_MAX*(1.0/3.0))){
				movement = MISSILE_FRAGMENT_SPEED1;
			}else if(randC <= (RAND_MAX*(2.0/3.0))){
				movement = MISSILE_FRAGMENT_SPEED2;
			}else{
				movement = MISSILE_FRAGMENT_SPEED3;
			}



			orxVector_Mulf(angle,angle,movement);
			tempBullet->SetSpeed(*angle);
			currentBullet++;
		}
	}else if(useNumber == 4){ // Level 3 - Standard
		int numberOfBullets = BASEDAMAGEPERSECOND*DAMAGEMULTPERRANK*DAMAGEMULTPERRANK*(MISSILE_COOLDOWN/60.0);
		while(currentBullet < numberOfBullets){

			Bullet* tempBullet = new Bullet(missile->GetPos(),"MissileFragment",2);


			step = ((2*3.14159)/50.0);
			step = step*(rand()%50);
			angle->fY = sin(step); //the math steps for unit circle
			angle->fX = cos(step);
			int randC = rand();

			if(randC <= (RAND_MAX*(1.0/3.0))){
				movement = MISSILE_FRAGMENT_SPEED1;
			}else if(randC <= (RAND_MAX*(2.0/3.0))){
				movement = MISSILE_FRAGMENT_SPEED2;
			}else{
				movement = MISSILE_FRAGMENT_SPEED3;
			}



			orxVector_Mulf(angle,angle,movement);
			tempBullet->SetSpeed(*angle);
			currentBullet++;
		}
	}else if(useNumber == 5){ // Level 3 - Strong
		int numberOfBullets = BASEDAMAGEPERSECOND*DAMAGEMULTPERRANK*DAMAGEMULTPERRANK*DAMAGEMULTSTRONG*(MISSILE_COOLDOWN/60.0)+BASEDAMAGEPERSECOND*DAMAGEMULTPERRANK*DAMAGEMULTPERRANK*DAMAGEMULTSTRONG*(MAXCHARGE/150.0);
		while(currentBullet < numberOfBullets){

			Bullet* tempBullet = new Bullet(missile->GetPos(),"MissileFragment",2);


			step = ((2*3.14159)/50.0);
			step = step*(rand()%50);
			angle->fY = sin(step); //the math steps for unit circle
			angle->fX = cos(step);
			int randC = rand();

			if(randC <= (RAND_MAX*(1.0/3.0))){
				movement = MISSILE_FRAGMENT_SPEED1;
			}else if(randC <= (RAND_MAX*(2.0/3.0))){
				movement = MISSILE_FRAGMENT_SPEED2;
			}else{
				movement = MISSILE_FRAGMENT_SPEED3;
			}



			orxVector_Mulf(angle,angle,movement);
			tempBullet->SetSpeed(*angle);
			currentBullet++;
		}
	}
}

void GameSystem::spawnTestBoss(){
	Behavior* currBehavior;
	BehaviorStep* currBehavStep;
	orxVECTOR* curPos;

	//create the boss object
	boss=new Boss();


	//create the boss parts
	orxSTRING temp = "stop";
	orxVECTOR start = {100,100,0};
	boss->addBossPart( new Unit(start, temp,100,10,-1,boss,1));
	orxVECTOR start2 = {200,100,0};
	boss->addBossPart( new Unit(start2, temp,100,10,-1,boss,1));

	//create the first behavior
	currBehavior= new Behavior();
	//create the first BehaviorStep
	currBehavStep= new BehaviorStep(0,60);
	//create the Action for the first boss part
	curPos= new orxVECTOR();
	curPos->fX=150;
	curPos->fY=150;
	curPos->fZ=0;
	currBehavStep->addAction(new Action(boss->getBossPart(0), curPos, 0));
	//create the Action  for the second boss part
	curPos= new orxVECTOR();
	curPos->fX=250;
	curPos->fY=150;
	curPos->fZ=0;
	currBehavStep->addAction(new Action(boss->getBossPart(1), curPos, 0));
	currBehavior->addBehaviorStep(currBehavStep);
	//create the second BehaviorStep
	currBehavStep= new BehaviorStep(60,120);
	//create the Action for the first boss part
	curPos= new orxVECTOR();
	curPos->fX=200;
	curPos->fY=100;
	curPos->fZ=0;
	currBehavStep->addAction(new Action(boss->getBossPart(0), curPos, 0));
	//create the Action  for the second boss part
	curPos= new orxVECTOR();
	curPos->fX=300;
	curPos->fY=100;
	curPos->fZ=0;
	currBehavStep->addAction(new Action(boss->getBossPart(1), curPos, 0));
	currBehavior->addBehaviorStep(currBehavStep);
	//create the third BehaviorStep
	currBehavStep= new BehaviorStep(120,180);
	//create the Action for the first boss part
	curPos= new orxVECTOR();
	curPos->fX=100;
	curPos->fY=100;
	curPos->fZ=0;
	currBehavStep->addAction(new Action(boss->getBossPart(0), curPos, 10));
	//create the Action  for the second boss part
	curPos= new orxVECTOR();
	curPos->fX=200;
	curPos->fY=100;
	curPos->fZ=0;
	currBehavStep->addAction(new Action(boss->getBossPart(1), curPos, 10));
	currBehavior->addBehaviorStep(currBehavStep);
	boss->addBehavior(currBehavior);
	//end of first behavior

	//create the second behavior
	currBehavior= new Behavior();

	//create the first BehaviorStep
	currBehavStep= new BehaviorStep(0,120);

	//create the Action for the first boss part
	curPos= new orxVECTOR();
	curPos->fX=175;
	curPos->fY=125;
	curPos->fZ=0;
	currBehavStep->addAction(new Action(boss->getBossPart(0), curPos, 0));

	//create the Action  for the second boss part
	curPos= new orxVECTOR();
	curPos->fX=225;
	curPos->fY=120;
	curPos->fZ=0;
	currBehavStep->addAction(new Action(boss->getBossPart(1), curPos, 0));
	currBehavior->addBehaviorStep(currBehavStep);

	//create the second BehaviorStep
	currBehavStep= new BehaviorStep(120,240);

	//create the Action for the first boss part
	curPos= NULL;
	currBehavStep->addAction(new Action(boss->getBossPart(0), curPos, 15));

	//create the Action  for the second boss part
	curPos= NULL;
	currBehavStep->addAction(new Action(boss->getBossPart(1), curPos, 15));
	currBehavior->addBehaviorStep(currBehavStep);

	boss->addBehavior(currBehavior);
	//end of second behavior

	boss->startInitialBehavior();

}

void GameSystem::savePlayerInventory(){
	player->saveInventory();
}

Player* GameSystem::getPlayerStatic(){
	return m_Instance->getPlayer();
}

void GameSystem::completeGame(){
	gameComplete = true;
}

void GameSystem::goToScoreScreen(){
	atStartScreen = false;
	orxVECTOR* storeage = new orxVECTOR();
	storeage->fX = -10000;
	storeage->fY = -10000;

	orxObject_SetPosition(startButt,storeage);
	orxObject_SetPosition(scoresButt,storeage);
	orxObject_SetPosition(exitButt,storeage);
	orxObject_SetPosition(startBack,storeage);
	orxObject_SetPosition(title,storeage);

	//TODO Make score screen background here

	//TODO Change music if wanted
	//orxSound_Stop(eyecandy); //is the way to stop the song currently playing at time of writing this comment
	//orxSound_Play(moleskinsoft); //starts the music we currently are using in game at time of writing this comment

	orxVECTOR ov = {100.0,-225.0,0.0};
	string temp;
	list<HighScore,std::allocator<HighScore>>* hs = HighScores::getScores();
	scoreDisplay = orxObject_CreateFromConfig("ScoresDisplay");
	oScoreDisplay = orxObject_CreateFromConfig("ScoresDisplay");

	hallOfFameDisplay = orxObject_CreateFromConfig("HallOfFameDisplay");
	temp = "HALL OF FAME";

	orxObject_SetTextString(hallOfFameDisplay,temp.c_str());
	headersLevel = orxObject_CreateFromConfig("HeadersLevel");
	temp = "LEVEL";

	orxObject_SetTextString(headersLevel,temp.c_str());

	headersName = orxObject_CreateFromConfig("HeadersName");
	temp = "NAME";
	orxObject_SetTextString(headersName,temp.c_str());

	headersScore = orxObject_CreateFromConfig("HeadersScore");
	temp = "SCORE";
	orxObject_SetTextString(headersScore,temp.c_str());
	//oScoreDisplay

	orxObject_SetPosition(oScoreDisplay,&ov);


	HighScore current;
	scoreString = ""; //Reset in case they played and want to see updated high scores
	oScoreString = "";
	while (hs->size() > 0){
		current = hs->front();
		scoreString += current.level;
		scoreString += "\t";
		scoreString += current.name;
		scoreString += "\t";
		//cout<<current.score<<endl;
		stringstream ss;
		ss << current.score;
		oScoreString += ss.str();
		scoreString += "\n";
		oScoreString += "\n";
		hs->pop_front();
	}
	orxObject_SetTextString(scoreDisplay,scoreString.c_str());
	orxObject_SetTextString(oScoreDisplay,oScoreString.c_str());

	scoresBack = orxObject_CreateFromConfig("ScoresBackground");
	backButt = orxObject_CreateFromConfig("BackButton");
}