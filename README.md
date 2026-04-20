# Legend of Saladir - Fishing Route

Roguelike game using curses library for ASCII graphics. This project is based on Legend of Saladir by Erno Pakarinen, available at [https://github.com/codesmith-fi/saladir](https://github.com/codesmith-fi/saladir)

## Installation

### Requirements

You need some type of compatible curses library (curses.h) to compile and run this game. The terminal must support 16 or more colors and at least 80 x 24 characters size. The minimum required C++ version is C++20. In some terminals you have to turn on "show bold text in bright colors" or similar setting to see black color's brighter versions (notably CH_DGRAY as dark grey).

### Linux

Go to the root directory of the project and type these commands:

```
make
./saladir
```

### Windows

Using an IDE create a console project, add all source files to the project and compile. You need to add curses library for the linker, details vary depending on your IDE. Note that if using dynamic linking you probably have to define PDC_DLL_BUILD found in output.h file. You can also compile this project from terminal, the procedure should be same as on other platforms.

## Coding style and philosophy

Object-oriented C++ with some C-style functions. The preferred tab size is 4, using a tab character. Naming conventions are:

```
enum, class, struct, member functions: Title_Case
define: MACRO_CASE
everything else: lowercase or snake_case
```

While this project is proceeding it's going to use more and more the much dreaded OOP style of C++, but there is no forced reason to use classes everywhere, that's why there are procedures and functions as well.

The source code has flat structure, everything is in 'src' directory and there is no pre-processing or build scripts required. This makes it easier to compile the project in an IDE. Each header file has a comment starting with Unit keyword. By searching "Unit" from header files you can get an idea of the source code structure. You can also search for comments starting with note: keyword to find things that are unfinished or some kind of a problem.
