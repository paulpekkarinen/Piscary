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
SelectItems::SelectItems(int flt)
	: arrayweight(0), lasttype(-1)
{
	Set_Filter(flt);
}

bool SelectItems::Change_Item_Category(int select)
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

void SelectItems::Show_Header()
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

	display->Header(prompt.c_str(), CH_WHITE);
	set_color(C_WHITE);

	if (usefilter >= 0)
	{
		gotoxy(SCREEN_COLS - strlen(gategories[usefilter].name) - 4, 1);
	
		my_printf("[ %s ]", gategories[usefilter].name);
	}
	else
		print_text_to(SCREEN_COLS-14, 1, "[ all items ]");

	goto_content();
}

/*bool inv_listitems_multi(
	inventory &inv,
	Pocket &mypocket,
	const char *preprompt,
	int filter,
	bool resmode, //note: 'resmode' never used, but it's in some calling routines
	bool multiselect,
	int x, int y)*/

//Browse inventory and select items from it, they are listed in mypocket.
bool SelectItems::Select(const char *preprompt)
{
	arrayweight = mypocket.get_weight_of_items();

	// we need to do a redraw after this
	GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;

	// show screen title
	Set_Header(preprompt);
	Show_Header();

	int pitemcount=0;
	bool selected=false;
	bool cancel=true;
	int itemcount=0;
	int lasttype=-1;
	int i=0; //starting index
	int j=0;
/*
	while (1)
	{
		//info=(ptrlist+i)->ptr;

		//show the current filtered inventory which is the pocket list
		mypocket.Show(i, lasttype, darklevel);

		while (1)
		{
			selectionprompt(arrayweight);

			if ((ptrlist+i)->ptr)
				display->More_Inventory(SCREEN_LINES-3);

			if (j!=0)
				display->More_Inventory(2);

			if (!itemcount)
			{
				gotoxy(0, 3);
				my_printf("..Nothing...\n");
			}

			const int key=my_getch();

			if (filter < 0)
			{
				if (Change_Category(key))
				{
					actfilter = key;
					arrayweight = mypocket.get_weight_of_items();

					itemcount=i=j=0;
					lasttype=-1;
					Show_Header();
					break;
				}
			}
			if (key==MYKEY_CTRLI)
			{
				text_data->View(Text_Data::Inventory_Help);
				itemcount=0;
				lasttype=-1;
				Show_Header();
				i=j;
				break;
			}
			if (is_confirm_key(key))
			{
				cancel=false;
				selected=true;
				break;
			}
			else
			{
				if (key==32 || key==KEY_ESC)
				{
					selected=true;
					cancel=true;
					break;
				}
				else
				{
					// select item commands
					if ((selection>='a') && (selection < ('a'+itemcount)))
					{
						selptr=(ptrlist + (selection-'a')+j)->ptr;

						(ptrlist + (selection-'a')+j)->sel = 1;
						selected=true;
						cancel=false;
						break;
					}
			// multiselection
					else if ((selection>='A') && (selection < ('A'+itemcount)))
					{
						// toggle selection
						if ((ptrlist + (selection-'A')+j)->sel == 1)
							(ptrlist + (selection-'A')+j)->sel = 0;
						else
							(ptrlist + (selection-'A')+j)->sel = 1;

						cancel=false;
						if (!multiselect)
						{
							selected=true;
							break;
						}
						else
						{
							itemcount=0;
							lasttype=-1;
							Show_Header();
							i=j;
							break;
						}
					}
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
	//	    else if((selection>='A') && (selection < ('A'+itemcount))) {
					else if (selection==MYKEY_CTRLO && !darklevel)
					{
						my_setcolor(CH_YELLOW);
						clear_lines(SCREEN_LINES-3, SCREEN_LINES-1);

						print_centered(SCREEN_LINES-2,
							"Open which container [press item letter]?");
						selection = my_getch();

						if (!((tolower(selection)>='a') &&
							(tolower(selection) < ('a'+itemcount))))
						{
							break;
						}

						selptr=(ptrlist + (tolower(selection)-'a')+j)->ptr;

						if (selptr->i.inv != NULL)
						{
							container = selptr;

							char * newprompt=NULL;

							newprompt = (char *)malloc(sizeof(char) *
								(my_strlen(container->i.name) +
									my_strlen(text_defaultinv) + 1));
							if (newprompt)
							{
								my_strcpy(newprompt, text_defaultinv,
									(my_strlen(container->i.name) +
										my_strlen(text_defaultinv) + 1));
								my_strcat(newprompt, container->i.name,
									(my_strlen(container->i.name) +
										my_strlen(text_defaultinv) + 1));

								newlist = inv_listitems_multi(container->i.inv,
									newprompt,
									filter, false, true, -1, -1);
								free(newprompt);
							}
							else
							{
								newlist = inv_listitems_multi(container->i.inv,
									"Item inventory",
									filter, false, true, -1, -1);

							}

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

	/*
	if (!cancel)
		newlist = inv_buildmultilist(ptrlist);
	else
		newlist = NULL;

	// return the multiselection list
	return newlist;*/

	return false;
}

void SelectItems::Set_Filter(int flt)
{
	filter=flt;

	//determine active filter's initial type
	if (filter < 0)
		actfilter='.';
	else
		actfilter=gategories[filter].out;
}
