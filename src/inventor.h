//Legend of Saladir - inventor.h

//Unit inventor: Base class for inventory routines.

#ifndef INVENTOR_H
#define INVENTOR_H

#include <string>
#include "geometry.h"
#include "pocket.h"

/* carry weight defines */
#define INV_OK       0
#define INV_BURDEN   1
#define INV_STRAIN   2
#define INV_OVERLOAD 3

class SelectItems
{
private:
	int actfilter; //active filter
	int filter; //original filter
	int arrayweight; //weight of items in the pocket
	int lasttype; //last shown item type in the pocket's list

	void Show_Header();

protected:
	std::string prompt; //inventory header text
	Pocket mypocket; //stores selected items here

	int Get_Filter() const { return filter; }

	bool Change_Item_Category(int select); //base class version
	bool Select(const char *preprompt);

public:
	SelectItems(int flt);
	virtual ~SelectItems() { }

	Pocket &Get_Pocket() { return mypocket; }

	virtual bool Change_Category(int select) = 0;
	void Set_Filter(int flt);
	virtual void Set_Header(const char *preprompt) = 0;
};

#endif
