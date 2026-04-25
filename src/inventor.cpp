/* player and monster
	inventory routines

   Legend of Saladir

   (C) Erno Tuomainen 1997/1998

*/

//Refactored 4.9.2021 - 7.9.2025 Paul K. Pekkarinen

#define _CRT_SECURE_NO_DEPRECATE 1

#include <cstring>
#include "avatar.h"
#include "display.h"
#include "game.h"
#include "input.h"
#include "inventor.h"
#include "itemdata.h"
#include "output.h"
#include "pocket.h"
#include "purse.h"

using std::string;

//Using player's backpack as default inventory.
Stockpile::Stockpile(int flt)
	: arrayweight(0), lasttype(-1)
{
	Set_Filter(flt);
}

bool Stockpile::Change_Item_Category(int select)
{
	int flt=-1; //selected category type
	int i=0; //index to category

	bool change=false; //true if category is changed

	while (gategories[i].out)
	{
		if (gategories[i].out==select) //matches the ascii type of gatecory
		{
			flt=i; //filter is now the category type
			change=true;
			break;
		}
		i++;
	}

	if (select=='.') //this selects everything
	{
		flt=-1;
		change=true;
	}

	if (change)
		Set_Filter(flt);

	return change;
}

void Stockpile::Show_Header()
{
	int i=0;
	int usefilter=-1;

	while (gategories[i].out)
	{
		if (gategories[i].out==actfilter)
		{
			usefilter = i;
			break;
		}
		i++;
	}

	display->Header(prompt.c_str(), CH_GREEN);
	set_color(C_WHITE);

	if (usefilter >= 0)
	{
		gotoxy(SCREEN_COLS - strlen(gategories[usefilter].name) - 4, 1);
	
		my_printf("[ %s ]", gategories[usefilter].name);
	}
	else
		print_text_to(SCREEN_COLS-14, 1, "[ all items ]");

	display->Footer("a-w = select, Space = choose, Ctrl+O = open, x = exit", CH_GREEN);
}

bool Stockpile::Open_Container(int key, int view_index)
{
	const int index=get_index_from_key(key, Items_Per_Page);
	if (index==-1)
		return false;

	//list items inside container
	inventory *inv=mypocket.Get_Container(view_index+index);
	if (inv!=0)
	{
		mypocket.Clear_Items();
		inv->builditemarray(mypocket, actfilter, -1, -1);
		return true;
	}

	return false;
}

//Browse pocket and select items from it.
int Stockpile::Select(const char *preprompt, bool oneshot)
{
	arrayweight = mypocket.get_weight_of_items();

	Set_Header(preprompt);
	Show_Header();

	int pitemcount=0;
	int itemcount=0;
	lasttype=-1;
	int rv=Cancel;
	int index=0;
	bool browsing=true;
	
	while (browsing)
	{
		//show the current filtered list (note: check darklevel parameter)
		goto_content();
		mypocket.Show(index, lasttype, false);

		//selectionprompt(arrayweight);

		const int key=my_getch();

		if (filter < 0)
		{
			if (Change_Category(key))
			{
				arrayweight = mypocket.get_weight_of_items();

				//itemcount=i=j=0;
				lasttype=-1;
				Show_Header();
				continue;
			}
		}

		switch (key)
		{
			case ' ':
			case KEY_ENTER:
				if (mypocket.Get_Selected_Amount()>0)
				{
					rv=Selected;
					browsing=false;
				}
			break;
			case 'x': rv=Cancel; browsing=false; break;
			case MYKEY_CTRLO:
			{
				my_setcolor(CH_YELLOW);
				clear_lines(SCREEN_LINES-3, SCREEN_LINES-1);

				print_centered(SCREEN_LINES-2,
					"Open which container [press item letter]?");
				int selection = my_getch();			
				if (Open_Container(selection, index)==false)
					display->Footer_Failure("That is not a container.");

				Show_Header();
			}
			break;
			default:
				if (key>='a' && key<='w')
				{
					const int i=get_index_from_key(key, Items_Per_Page);
					if (i!=-1)
					{
						invnode *esine=mypocket.Get_Item_Handle(i);
						if (esine!=0)
						{
							mypocket.Toggle(index+i);
						}
					}	
				}
			break;
		}
	}
/*
			// move to container
					else if (selection==MYKEY_CTRLT && !darklevel)
					{
						clear_lines(SCREEN_LINES-2, SCREEN_LINES);

						newlist = inv_buildmultilist(ptrlist);
						if (!newlist)
						{
							Failure("No items selected for transfer.");
							break;
						}

						Inform(
							"Move selected items, select a container"
							" [press item letter]?", CH_YELLOW);
						selection = my_getch();

						if (!((tolower(selection)>='a') &&
							(tolower(selection) < ('a'+itemcount))))
						{
							inv_freemultilist(&newlist);
							break;
						}

						container=(ptrlist + (tolower(selection)-'a')+j)->ptr;

						if (container->i.inv != NULL)
						{
							i = 0;

							// transfer all selected items to the container
							while (newlist[i].ptr!=NULL)
							{
								if (container == newlist[i].ptr)
								{
									Error(
										"Can't store container inside itself. [KEY]");
								}
								else if (newlist[i].ptr->slot>=0)
								{
									Error(
										"Can't move equipped items. [KEY]");
								}
								else
								{
									inv_transfer2inv(c_level, inv, NULL, container->i.inv,
										newlist[i].ptr, -1,
										0, 0);
								}
								i++;
							}
							inv_freemultilist(&newlist);

							if (x>=0 && y>=0)
								inv_pileitems_coords(inv);
							else
								inv_pileitems(inv);

							newlist=inv_list_changecategory(inv, ptrlist,
								actfilter, x, y);

							if (newlist!=NULL)
							{
								ptrlist=newlist;
								arrayweight = mypocket.get_weight_of_items(); //(ptrlist);
							}
						}
						else
						{
							inv_freemultilist(&newlist);
							Failure("That item can't contain anything.");
						}

						itemcount=0;
						lasttype=-1;
						Show_Header();
						i=0;
						j=0;
						break;
					}
			// open container commands

							if (newlist)
							{
								i=0;

								// transfer all selected items to upper container
								while (newlist[i].ptr!=NULL)
								{
									inv_transfer2inv(c_level, container->i.inv, NULL, inv,
										newlist[i].ptr, -1,
										container->x, container->y);
									i++;
								}

								inv_freemultilist(&newlist);

								if (x>=0 && y>=0)
									inv_pileitems_coords(inv);
								else
									inv_pileitems(inv);

								newlist=inv_list_changecategory(inv, ptrlist,
									actfilter,
									x, y);
								if (newlist!=NULL)
								{
									ptrlist=newlist;
									arrayweight = mypocket.get_weight_of_items(); //(ptrlist);
								}

							}
							itemcount=0;
							lasttype=-1;
							Show_Header();
							i=0;
							j=0;
							break;
						}
						else
						{
							Failure("It doesn't contain anything.");
						}
					}
					else if ((selection=='3' || selection==KEY_NPAGE ||
						selection=='2' || selection==KEY_DOWN)
						&& (ptrlist+i)->ptr)
					{
						pitemcount=itemcount;
						Show_Header();
						itemcount=0;
						lasttype=-1;
						if (selection=='2' || selection == KEY_DOWN)
							j=j+1;
						else
							j=i;

						i=j;
						break;
					}
					else if (selection=='9' || selection==KEY_PPAGE ||
						selection=='8' || selection==KEY_UP)
					{
						Show_Header();
						//	       i=j-SCREEN_LINES-4;
						if (selection == '9' || selection==KEY_PPAGE)
							i=j-pitemcount;
						else
							i=j-1;
						j=i;
						itemcount=0;
						lasttype=-1;
						if (i<0)
						{
							i = j = 0;
						}
						break;
					}
				}
			}
		}

		if (selected) break;
	}*/

	return rv;
}

void Stockpile::Set_Filter(int flt)
{
	filter=flt;

	//determine active filter's initial type
	if (filter < 0)
		actfilter='.';
	else
		actfilter=gategories[filter].out;
}
