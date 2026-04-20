//Legend of Saladir - trade.h

//Unit trade: Shop routines.

#ifndef TRADE_H
#define TRADE_H

class equipment;
struct being;
class inventory;
struct invnode;
struct level_type;
struct playerinfo;

bool inv_paymoney(inventory &from_inv, inventory &to_inv, equipment &gear,
	int copperneed, bool checkonly);
void paybill(playerinfo *plr, level_type *level);
bool shopkeeper_buy(level_type *level, being *keeper, invnode *item);

#endif
