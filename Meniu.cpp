#include <iostream>
#include <windows.h>
#include <conio.h>
#include <iomanip>
#include "Meniu.h"
using namespace std;

Meniu::Meniu(string titlu, vector<string> optiuni, void (*cb)(int))
{
    this->optiuni = optiuni;
    this->callback = cb;
    this->titlu = titlu;
}
// Retuneaza indexul liniei la care ne aflam
int Meniu::row_for_index(int index)
{
    int rows = get_console_size().rows;

    return (rows/2 - optiuni.size()/2) + index;
}

void Meniu::clear_selection(int index)
{
    SetConsoleTextAttribute(console, FOREGROUND_GREEN);
    int columns = get_console_size().columns;

    int row_of_index = row_for_index(index);
    COORD pos;
    pos.X = 1;
    pos.Y = row_of_index;

    SetConsoleTextAttribute(console, FOREGROUND_GREEN);
    for (int i = pos.X; i<columns-1; i++)
    {
        char c = charAtXY(i, row_of_index);

        COORD newpos;
        newpos.X = i;
        newpos.Y = row_of_index;
        SetConsoleCursorPosition(console, newpos);
        cout << c;
    }
}

void Meniu::highlight_index(int index)
{
    int columns = get_console_size().columns;

    int row_of_index = row_for_index(index);
    COORD pos;
    pos.X = 1;
    pos.Y = row_of_index;

    SetConsoleTextAttribute(console, FOREGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED);//SENZATIE DE ALB;
    for (int i = pos.X; i<columns-1; i++)
    {
        char c = charAtXY(i, row_of_index);

        COORD newpos;
        newpos.X = i;
        newpos.Y = row_of_index;
        SetConsoleCursorPosition(console, newpos);
        cout << c;
    }
}

void Meniu::afiseaza()
{
    int columns = get_console_size().columns;
    int rows = get_console_size().rows;

    system("cls");
    SetConsoleTextAttribute(console, FOREGROUND_GREEN);
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms683163%28v=vs.85%29.aspx

    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(console, &cursorInfo);
    cursorInfo.bVisible = FALSE;  // set the cursor visibility, http://stackoverflow.com/questions/18028808/blinking-underscore-with-console
    SetConsoleCursorInfo(console, &cursorInfo);

    // Deseneaza partea de sus a chenarului
    cout << (char)201;
    for (int i=1; i<columns-1; i++)
    {
        cout << (char)205;
    }
    cout << (char)187;

    // Deseneaza randul cu titlul
    int hspace = columns/2 - titlu.length()/2;
    cout << (char)186;
    for (int i=1; i<hspace; i++)
    {
        cout << " ";
    }
    cout << titlu;
    for (int i=hspace+titlu.length(); i<columns-1; i++)
    {
        cout << " ";
    }
    cout << (char)186;

    // Deseneaza randuri goale pana cand e nevoie sa afisam optiunile
    int vspace = rows/2 - optiuni.size()/2;
    for (int i=2; i<vspace; i++)     // i=2 deoarece suntem deja pe al doilea rand
    {
        cout << setfill(' ') << (char)186 << setw(columns-1) << (char)186;
    }

    // Afiseaza optiunile meniului
    for(vector<string>::iterator it = optiuni.begin(); it != optiuni.end(); ++it)
    {
        string nume_optiune = *it;
        int x_start = columns/2 - nume_optiune.length()/2;
        cout << (char)186;

        for (int i=1; i<x_start; i++)
        {
            cout << " ";
        }

        cout << nume_optiune;

        for (int i=x_start+nume_optiune.length(); i<columns-1; i++)
        {
            cout << " ";
        }
        cout << (char)186;
    }

    // Deseneaza randuri goale mai departe
    for (int i=vspace+optiuni.size(); i<rows-2; i++)         // i<rows-2 deoarece vrem sa lasam un rand
    {
        cout << (char)186 << setw(columns-1) << (char)186;  // pentru partea de jos a chenarului si totodata
    }                                                        // sa prevenim consola sa faca scroll

    // Deseneaza partea de jos a chenarului
    cout << (char)200;
    for (int i=0; i<columns-2; i++)
    {
        cout << (char)205;
    }
    cout << (char)188;

    current_index = 0;
    highlight_index(current_index);

    while (int key = getch())
    {
        if (key == 80)
        {
            clear_selection(current_index);
            current_index += 1;
            if (current_index > optiuni.size()-1) current_index = 0;
            highlight_index(current_index);
        }
        else if (key == 72)
        {
            clear_selection(current_index);
            current_index -= 1;
            if (current_index < 0) current_index = optiuni.size() - 1;
            highlight_index(current_index);
        }
        else if (key == 13)
        {
            callback(current_index);
            break;
        }
        else if (key == 255)
        {
            afiseaza();
            break;
        }
    }
}

Meniu::~Meniu()
{
    //dtor
}
