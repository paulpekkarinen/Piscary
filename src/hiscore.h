//Legend of Saladir - hiscore.h

//Unit hiscore: Scoreboard.

#ifndef HISCORE_H
#define HISCORE_H

#include <vector>

struct Score;

//List of current scores.
class Scoreboard
{
private:
	/* 
	 * version of the scorefile, *MUST* be incremented
	 * whenever the score file format is changed! 
	 */
	const int SCORE_FILEVERSION=0x02;

	const int Max_Highscores=100;

	std::vector<Score*> highscores;
	bool changed; //true if new score added
	
	typedef std::vector<Score*>::iterator scritr;

	int Add_New(Score *contender);
	void Show_At_Index(int index, Score *latest);

public:
	Scoreboard();
	~Scoreboard();

	bool Is_Changed() const { return changed; }

	//create score entry from player's data and try to enter the highscore list
	void Calculate();

	void Showbest(Score *latest); //show the list of best scores

	bool Load();
	bool Save();
};

#endif
