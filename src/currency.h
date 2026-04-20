//currency.h

//Unit currency: Coinage.

#ifndef CURRENCY_H
#define CURRENCY_H

struct Currency
{
	int gold;
	int silver;
	int copper;

	Currency()
		: gold(0), silver(0), copper(0) { }
	explicit Currency(int value);

	void Coppervalue(int value);
};

#endif
