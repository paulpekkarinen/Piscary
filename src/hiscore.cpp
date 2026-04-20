/*
 * hiscore.cpp for Legend of Saladir
 *
 * (C)1997/1998 by Erno Tuomainen
 *
 * Score board
 *
 */

//Refactored 9.6.2023 - 21.9.2025 Paul K. Pekkarinen

#include "avatar.h"
#include "display.h"
#include "file.h"
#include "hiscore.h"
#include "input.h"
#include "output.h"
#include "score.h"
#include "storage.h"

const char FILE_SCOREFILE[] = "score.dat";

Scoreboard::Scoreboard()
	: changed(false)
{
	//debug list of scores
	/*for (int t=1; t<51; t++)
	{
		monsterdef m;
		m.name="Dick";
		m.level=1;
		m.race=0;
		m.gender=SEX_MALE;

		Score *s=new Score(m, t);
		if (Add_New(s)==0)
		{
			delete s;
			break;
		}
	}*/
}

Scoreboard::~Scoreboard()
{
	for (scritr pos=highscores.begin(); pos!=highscores.end(); ++pos)
		delete (*pos);
}

int Scoreboard::Add_New(Score *contender)
{
	int place=1; //places are from 1 to max highscores

	//adds score to the place where it belongs in the list by comparing it to
	//the current score starting from the top of the list
	for (scritr pos=highscores.begin(); pos!=highscores.end(); ++pos)
	{
		if (contender->Is_Better_Or_Equal(*pos))
		{
			highscores.insert(pos, contender);

			//remove the last score if the list exceeds max number of scores
			const int sz=(int)highscores.size();
			if (sz>=Max_Highscores)
				highscores.pop_back();

			changed=true;
			return place; //return the place when player is added to scoreboard
		}
		place++;
	}

	//if the score was lower or no scores yet in the list, add it to the end
	//of the list, or start if it's the first score
	const int sz=(int)highscores.size();
	if (sz<Max_Highscores)
	{
		highscores.push_back(contender);
		changed=true;
		return place;
	}

	return 0; //returns zero if the score was too low to enter the list
}

void Scoreboard::Calculate()
{
	Score *myscore=new Score(player);

	if (Add_New(myscore)==0)
	{
		my_printf("Sorry, you didn't make it to the top list!");
		delete myscore;
		return;
	}

	Showbest(myscore);
}

bool Scoreboard::Load()
{
	File savefile(FILE_SCOREFILE);
	if (savefile.Open()==false)
		return false;

	Tar_Ball tb;
	tb.Load(&savefile);

	//check version
	const int version=tb.Get_Next_Value();

	if (version!=SCORE_FILEVERSION)
	{
		my_printf("Your score file is an old version. "
			"It will be overwritten when the "
			"program is exit, if new scores are added.");
		return true; //success anyway
	}

	//load amount of score entries
	const int amt=tb.Get_Next_Value();

	highscores.clear();

	//load and add them to the list
	for (int t=0; t<amt; t++)
	{
		Score *s=new Score;
		s->Load(tb);
		highscores.push_back(s);
	}

	changed=false;
	return true;
}

bool Scoreboard::Save()
{
	//if not changed, no need to save, but return true as success
	if (changed==false)
		return true;

	File savefile(FILE_SCOREFILE);

	if (savefile.Create()==false)
		return false;

	Tar_Ball tb;
	tb.Put(SCORE_FILEVERSION);

	//save amount of score entries
	const int amt=(int)highscores.size();
	tb.Put(amt);

	for (scritr ii = highscores.begin() ; ii != highscores.end() ; ++ii)
	{
		(*ii)->Save(tb);
	}

	tb.Save(&savefile);

	changed=false; //not needed, but set anyway
	return true;
}

void Scoreboard::Show_At_Index(int index, Score *latest)
{
	scritr ii = highscores.begin();
	int i=1;
	while (i!=index)
	{
		++ii;
		i++;
	}

	int y=3;
	int ni=i;

	while (ii != highscores.end())
	{
		Score *si=(*ii);

		if (latest!=0 && si->Is_List_Match(latest))
			set_color(CH_YELLOW);
		else
			set_color(C_WHITE);

		si->Show(ni, y);

		++ii;
		ni++;
		y++;
		if (y>LINES-3)
			break;
	}
}

void Scoreboard::Showbest(Score *latest)
{
	if (highscores.empty())
	{
		clear_screen();
		my_printf("No recorded highscores yet.");
		wait_key();
		return;
	}

	display->Hiscore_Header();

	bool loop=true;
	int i=1;
	const int amt=(int)highscores.size();

	while (loop)
	{
		Show_At_Index(i, latest);

		switch (my_getch())
		{
			case KEY_DOWN: i++; break;
			case KEY_UP: i--; break;
			case KEY_ESC: loop=false; break;
			default: break;
		}

		if (i<1) i=1;
		else
		{
			if (i>amt-24)
				i=amt-24;
		}
	}
}
