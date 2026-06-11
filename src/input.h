//Legend of Saladir - input.h

//Unit input: Input routines.

#ifndef INPUT_H
#define INPUT_H

#include <string>

struct level_type;
struct monsterdef;
struct Spell;
struct Target;

bool confirm_yn(const char *prompt, bool defaultresult, bool showprompt); //get y/n
int dir_askdir(const char *prompt, bool self); //ask direction
int get_amount_of_items(int max_count); //input a number
int get_index_from_key(int k, int max_items);
int get_response(const char *prompt, const char *str); //get one of keys from 'str'
std::string get_string(bool safe_chars, const int max_len); //enter a string
bool is_confirm_key(int k);
bool is_quit_key(int k);
bool list_more(int &y); //check position of text, show --more-- if at end of screen
int my_getch();
int my_gets(char *str, int mlen); //get a string
void player_gettarget(level_type *level, Target *target, Spell *spell);
void rename_monster(monsterdef &mondef); //give a name for this monster (not for player!)
void retitle_player(monsterdef &mondef); //change player's title
int select_textlist(const char *prompt, const char **list, bool quit);
void showmore(bool msgmode, bool shortmsg);
void wait_key();
bool wait_more();
bool wait_more_lines(int cy, const char *teksti);

#endif
