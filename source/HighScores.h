#ifndef __HIGHSCORES_H_
#define __HIGHSCORES_H_

#include <string>
#include <list>
#include <iostream>
#include <fstream>

#define MAX_HIGH_SCORES 10

using namespace std;

struct HighScore {
	string name;
	long score;
	string level;
};

class HighScores
{
public:
	static list<HighScore> * getScores();
	static bool isHighScore(long score);
	static void submitHighScore(string name, long score, string level);
private:
	static int tosz(int l1, int l2);
};

#endif __HIGHSCORES_H_