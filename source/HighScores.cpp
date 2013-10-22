#include "HighScores.h"

list<HighScore> * HighScores::getScores() {
	ifstream fin("aXZrq49.bin", ifstream::in);

	list<HighScore> * scores = new list<HighScore>();

	if (fin.fail())
		return scores;

	while (true) {
		int sz;
		fin.read((char*)&sz, 4);
		if (fin.eof())
			break;
		int l2 = sz & 0xFF;
		int l1 = (sz >> 8) & 0xFF;
		int totsz = (sz >> 16) & 0xFFFF;
		char * data = new char[totsz + 3];
		fin.read(data, totsz);
		for (int i = 0; i < totsz; i += 4)
		{
			long l = *((int*)(data + i));
			l ^= 0xDEADBEEFl;
			*((int*)(data + i)) = (int)(l & 0xFFFFFFFF);
		}
		HighScore hs;
		hs.name.append(data, l1);
		hs.level.append((data + l1), l2);
		hs.score = *((long*)(data + l1 + l2));
		scores->push_back(hs);
	}

	fin.close();
	
	return scores;
}

bool HighScores::isHighScore(long score) {
	list<HighScore> * scores = getScores();
	if (scores->size() < MAX_HIGH_SCORES)
		return true;
	for (list<HighScore>::iterator it = scores->begin(); it != scores->end(); it++) {
		if (score > it->score)
			return true;
	}
	return false;
}

void HighScores::submitHighScore(string name, long score, string level) {
	if (!isHighScore(score)) return;
	list<HighScore> * scores = getScores();

	if (scores->size() >= MAX_HIGH_SCORES)
		scores->pop_back();

	HighScore hs;
	hs.name = name;
	hs.score = score;
	hs.level = level;

	for (list<HighScore>::iterator it = scores->begin(); ;it++) {
		if (it == scores->end() || it->score < score) {
			scores->insert(it, hs);
			break;
		}
	}

	ofstream fout("aXZrq49.bin", ofstream::out);

	for (list<HighScore>::iterator it = scores->begin(); it != scores->end() ;it++) {
		hs = (*it);
		int totsz = tosz(hs.name.size(), hs.level.size());
		int sz = hs.name.size() + hs.level.size() + 8;
		fout.write((char*)&totsz, 4);
		char * out = new char[sz + 3];
		memcpy(out, hs.name.c_str(), hs.name.size());
		memcpy(out + hs.name.size(), hs.level.c_str(), hs.level.size());
		memcpy(out + hs.name.size() + hs.level.size(), &hs.score, 8);
		for (int i = 0; i < sz; i += 4) {
			long l = *((int*)(out + i));
			l ^= 0xDEADBEEFl;
			*((int*)(out + i)) = (l & 0xFFFFFFFF);
		}
		fout.write(out, sz);
	}

	fout.close();
}

int HighScores::tosz(int l1, int l2)
{
	int totsz = l1 + l2 + 8;
	return ((totsz & 0xFF) << 16) | ((l1 & 0xFF) << 8) | (l2 & 0xFF);
}