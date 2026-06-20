//Legend of Saladir - body.h

//Unit body: Body data for creatures.

#ifndef BODY_H
#define BODY_H

/* monster and player 'sex' values */
#define SEX_UNKNOWN	0
#define SEX_MALE	1
#define SEX_FEMALE	2
#define SEX_NEUTRAL	3

class Gender
{
public:
	enum Types
	{
		Hermaphrodite, //both male and female
		Male,
		Female,
		Neutral, //not reproductive
		Amt_Of_Genders
	};

private:
	int type;

public:
	Gender(int t) : type(t) { }

	const char *Get_Art(int a);
	const char *Get_Name();
};

#endif
