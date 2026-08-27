/*
 * options.h for the Legend of Saladir roguelike
 * (C) 1997/1998 by Erno Tuomainen
 *
 */

//Unit options: Gameplay options.

#ifndef OPTIONS_H
#define OPTIONS_H

class Tar_Ball;

//Abstract base class for option data.
class Option
{
private:
	const char *name;
	const char *desc; //description of option
	char letter; //assigned to this letter

public:
	Option(const char *n, const char *od, char which)
		: name(n), desc(od), letter(which) { }
	virtual ~Option() { }

	int Get_Input_Offset();

	virtual void Change() = 0;
	void Show();
	void Show_Description();
	virtual void Show_Value() = 0;
	virtual void Load(Tar_Ball &tb) = 0;
	virtual void Save(Tar_Ball &tb) = 0;
};

class Bool_Option : public Option
{
private:
	bool &value;

public:
	Bool_Option(const char *n, const char *od, char which, bool &v)
		: Option(n, od, which), value(v) { }

	void Change() override;
	void Show_Value() override;
	void Load(Tar_Ball &tb) override;
	void Save(Tar_Ball &tb) override;
};

class Integer_Option : public Option
{
private:
	int &value;
	int min_value;
	int max_value;

	//for text input, need 21 characters for item categories + zero
	static const int opt_txt_size=22;
	char opt_txt[opt_txt_size];

public:
	Integer_Option(const char *n, const char *od, char which, int &v,
	int minv, int maxv)
		: Option(n, od, which), value(v),
		min_value(minv), max_value(maxv) { }

	void Change() override;
	void Show_Value() override;
	void Load(Tar_Ball &tb) override;
	void Save(Tar_Ball &tb) override;
};

class Array_Option : public Option
{
private:
	char *arr;
	int size;

public:
	Array_Option(const char *n, const char *od, char which, char *a,
	int sz)
		: Option(n, od, which), arr(a), size(sz) { }

	void Change() override;
	void Show_Value() override;
	void Load(Tar_Ball &tb) override;
	void Save(Tar_Ball &tb) override;
};

struct gameconfig
{
	bool anykeymore; // if set any key will continue when (more)
	bool autodoor;
	bool autopickup;
	bool autopush;
	bool colormessages;
	bool colortext;
	bool compactmessages;
	bool door_disturb;
	bool droppiles;
	bool foodwarn;
	bool getallmoney;       // if set, get all money at once
	int health_alarm;    // alarm player when health goes below this
	bool item_disturb;      // walk mode is disturbed by items
	bool monster_disturb;   // --    by monsters in sight
	char pickuptypes[20];   // autopickup types
	int repeatcount;     // max number of repeats per command
	bool repeatupdate;      // update while in repeat walk
	bool stair_disturb;
	bool targetline;
	int ticksperminute;

	void Reset();
};

extern gameconfig CONFIGVARS;

#endif
