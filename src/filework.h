//Legend of Saladir - filework.h

//Unit filework: File and directory functions.

#ifndef FILEWORK_H
#define FILEWORK_H

#include <string>

bool change_directory(const char *dirname);
bool check_directories(); //find out if directory system is working
void clean_temp_files();
bool create_directory(const char *dirname);
bool deletefile(const char *filename);
void delete_lock_file();
bool has_matching_file_extension(const char *src, char *dest);
bool is_temp_extension(const std::string &source);
std::string load_text_file(const char *filename);
bool switch_to_savedir();
bool testfile(const char *filename);

#endif
