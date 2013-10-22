#include "Parser.h"
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include "LevelSystem.h"
using namespace std;



Parser::Parser(){

}

int Parser::Write(){
	int size;
	//ostream index ("index.txt");
	ifstream index ("levels/squid.txt");
	size = 1024;
	char *line = new char[size];
	if (index.is_open())
	{
		while (!index.eof()){
			//cout<<size<<endl;
			index.getline(line,size);
		}
	}
	//index.open("squid.txt");
	//index << "This is squid";
	index.close();
	return 0;
}


int Parser::Read(string name){ //name is the name of the file
	int size;
	ifstream index ("level/"+name); //level files go in the level folder
	size = 1024; //arbitrary size, please dont make lines more characters than this
	char *line = new char[size];
	if (index.is_open())
	{
		while (!index.eof()){
			int i = 0;
			index.getline(line,size);
			
		}
	}
	else
		return -1;
	//index.open("squid.txt");
	//index << "This is squid";
	index.close();
	return 0;
}

/*
struct MapEvent{
	short command;
	int time;
	int iValue1;
	int iValue2;
	int iValue3;
	string sValue1;
	string sValue2;
	string sValue3;
};
*/
MapEvent Parser::ParseEvent(char* s[]){
	struct MapEvent newEvent;
	
	int args = atoi(s[0]);		//first will always be number of arguments
	int command = atoi(s[1]);	//Depending on the command, different arguments mean different things
	int time = atoi(s[2]);		//EVERY SINGLE EVENT needs a time, this is when they start

	switch( command ){

	case 0:
		//Change map speed event
		break;
	case 1:
		//Create enemy event
		break;
	}

	return newEvent;
}