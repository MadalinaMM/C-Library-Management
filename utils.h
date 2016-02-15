#include <string>
#include <windows.h>

using namespace std;

#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

extern HANDLE console; // Un HANDLE catre consola
// In computer programming, a handle is an abstract reference to a resource

struct console_size
{
    int columns;
    int rows;
};

console_size get_console_size();
string ExePath();
COORD getCursorXY();
char charAtXY(int x, int y);
string to_string(int number);

#endif // UTILS_H_INCLUDED
