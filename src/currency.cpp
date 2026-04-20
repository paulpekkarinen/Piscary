//Legend of Saladir - currency.cpp

#include "currency.h"
#include "itemdata.h"
#include "items.h"

Currency::Currency(int value)
{
	Coppervalue(value);
}

/* calculate price from COPPER to print a string: ie. "3g-2s-13c" or "12g" */
void Currency::Coppervalue(int value)
{
	//     real rmod;
	int c2=0;

	/*
		 rmod=(real)((1.0)/valuables[MONEY_COPPER].value);	// 8

		 c1=(int32u)(copper/rmod);

		 copper-=(int32u)(c1*rmod);

		 if(copper > 0) {
		  rmod=(real)(1.0/valuables[MONEY_SILVER].value);	// 4
		  c2=(int32u)(copper/rmod);

		  copper-=(int32u)(c2*rmod);
		 }
	*/

	int c1=value/valuables[MONEY_GOLD].value;
	value-=c1*valuables[MONEY_GOLD].value;

	if (value > 0)
	{
		//	  rmod=(real)(1.0/valuables[MONEY_SILVER].value);	// 4
		c2=(value/valuables[MONEY_SILVER].value);
		value-=c2*valuables[MONEY_SILVER].value;
	}

	gold=c1;
	silver=c2;
	copper=value;
}
