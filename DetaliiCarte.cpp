#include <windows.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include "DetaliiCarte.h"
#include "sqlite3.h"

DetaliiCarte::DetaliiCarte(Carte carte, void (*cancel)()) // Constructor
{
    this->carte = carte;
    this->cancel = cancel;
}

void DetaliiCarte::afiseaza()
{
    int columns = get_console_size().columns;
    int rows = get_console_size().rows;

    system("cls");
    SetConsoleTextAttribute(console, FOREGROUND_GREEN);
// set the cursor visibility, http://stackoverflow.com/questions/18028808/blinking-underscore-with-console
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(console, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(console, &cursorInfo);

    vector<string> text_informatii;
    text_informatii.push_back("Titlu: " + carte.titlu);
    text_informatii.push_back("Autor: " + carte.autor);
    text_informatii.push_back("Pagini: " + carte.nr_pag);
    text_informatii.push_back("Editura: " + carte.editura);
    text_informatii.push_back("An publicare: " + carte.an);
    text_informatii.push_back("ISBN: " + carte.isbn);

    // Deseneaza partea de sus a chenarului
    cout << setfill((char)205) << (char)201 << (char)205 << setw(columns-2) << (char)187;
    cout << setfill(' ') << (char)186 << setw(columns-1) << (char)186;  // Lasam un rand gol, de estetica
    for(vector<string>::iterator it = text_informatii.begin(); it != text_informatii.end(); ++it)
    {
        string text = *it;
        cout << (char)186 << " " << text << setw(columns-1-text.length()-1) << (char)186;
    }

    int counter = 0;

    if (carte.imprumutat != 0)
    {
        string text = " Imprumutat catre: ";
        // Interogare SQL
        sqlite3 *db;
        sqlite3_stmt *stmt;

        int rc = sqlite3_open((ExePath() + "\\biblioteca.sqlite").c_str(), &db);
        if (rc)
        {
            cout << string("Eroare la deschiderea bazei de date: ") + sqlite3_errmsg(db);
            return;
        }

        string sql = "SELECT nume, prenume FROM studenti WHERE id='" + to_string(carte.imprumutat) + "'";
        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
        if (rc == SQLITE_OK)
        {
            while (TRUE)
            {
                rc = sqlite3_step(stmt);
                if (rc != SQLITE_ROW && rc != SQLITE_DONE)
                {
                    string errmsg(sqlite3_errmsg(db));
                    sqlite3_finalize(stmt);
                    sqlite3_close(db);
                    break;
                }

                if (rc == SQLITE_ROW)
                {
                    string nume = (char*)sqlite3_column_text(stmt, 0);
                    string prenume = (char*)sqlite3_column_text(stmt, 1);
                    text += nume + " " + prenume;
                    cout << setfill(' ') << (char)186 << text << setw(columns-text.length()-1) << (char)186;
                    counter++;
                }
                else
                {
                    sqlite3_finalize(stmt);
                    sqlite3_close(db);
                    break;
                }
            }
        }
        else
        {
            cout << sqlite3_errmsg(db);
        }
    }

    // Deseneaza restul chenarului
    for (int i=0; i<rows-text_informatii.size()-counter-4; i++)
    {
        cout << setfill(' ') << (char)186 << setw(columns-1) << (char)186;
    }
    cout << setfill((char)205) << (char)200 << (char)205 << setw(columns-2) << (char)188;

    while (int key = getch())
    {
        if (key == 27)
        {
            cancel();
            break;
        }
    }
}

DetaliiCarte::~DetaliiCarte()
{
    //dtor
}
