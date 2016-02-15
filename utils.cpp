#include <windows.h>
#include <sstream>
#include "utils.h"
// Handle catre consola
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);


// https://msdn.microsoft.com/en-us/library/windows/desktop/ms683171%28v=vs.85%29.aspx
console_size get_console_size()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns, rows;
    GetConsoleScreenBufferInfo(console, &csbi);
    columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    console_size csize;
    csize.columns = columns;
    csize.rows = rows;

    return csize;
}



// sursa: http://stackoverflow.com/questions/875249/how-to-get-current-directory
string ExePath()
{
    char buffer[MAX_PATH];
    GetModuleFileName( NULL, buffer, MAX_PATH );
    string::size_type pos = string( buffer ).find_last_of( "\\/" );
    return string( buffer ).substr( 0, pos);
}

// http://www.cplusplus.com/forum/windows/181927/
COORD getCursorXY()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD zero;

    if (GetConsoleScreenBufferInfo(console, &csbi))
    {
        return csbi.dwCursorPosition;
    }

    return zero;
}



// http://stackoverflow.com/questions/31494175/c-check-if-a-character-is-present-at-a-given-position-on-the-console
char charAtXY( int x, int y )
{

    CHAR_INFO ci;
    COORD xy = {0, 0};
    COORD wh = {1, 1};
    SMALL_RECT rect = { x, y, x, y };
    return ReadConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE), &ci, wh, xy, &rect) ? ci.Char.AsciiChar : '\0';
}

// http://www.cplusplus.com/articles/D9j2Nwbp/
string to_string(int number)
{
    return static_cast<ostringstream*>( &(ostringstream() << number) )->str();
}
