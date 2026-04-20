//textdata.cpp - pieces of text

//Refactored 26.9.2021 - 29.8.2023 Paul K. Pekkarinen

#include <string>
#include "filework.h"
#include "textdata.h"
#include "textview.h"

using std::string;

const char *Text_Data::filenames[Amt_Of_Pieces]=
{
    "inventor.txt",
    "manual.txt",
    "missile.txt"
};

Text_Data::Text_Data()
{
    for (int t=0; t<Amt_Of_Pieces; t++)
    {
        data[t]=load_text_file(filenames[t]);

        //put something in if loading fails (the file was not found etc.)
        if (data[t].empty())
        {
            data[t].append("Text file ");
            data[t].append(filenames[t]);
            data[t].append(" was not loaded for some reason.");
        }
    }
}

void Text_Data::View(int piece)
{
    viewtext(data[piece].c_str());
}
