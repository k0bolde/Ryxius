#ifndef __PARSER_H__
#define __PARSER_H__

class Parser;
class LevelSystem;

#include "orx.h"
#include <string>
#include <string.h>
#include "LevelSystem.h"

class Parser 
{
public:
Parser();
int Parser::Read(std::string name);
int Parser::Write();
MapEvent Parser::ParseEvent(char* s[]);

};
#endif