#include <windows.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include "DetaliiUtilizator.h"
#include "sqlite3.h"

DetaliiUtilizator::DetaliiUtilizator(Utilizator utilizator, void (*cancel)())
{
    this->utilizator = utilizator;
    this->cancel = cancel;
}

void DetaliiUtilizator::afiseaza()
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
    text_informatii.push_back("Nume: " + utilizator.nume);
    text_informatii.push_back("Prenume: " + utilizator.prenume);
    text_informatii.push_back("Nume utilizator: " + utilizator.username);
    text_informatii.push_back("Carti imprumutate:");

    // Deseneaza partea de sus a chenarului
    cout << setfill((char)205) << (char)201 << (char)205 << setw(columns-2) << (char)187;
    cout << setfill(' ') << (char)186 << setw(columns-1) << (char)186;  // Las un rand gol, de estetica
    for(vector<string>::iterator it = text_informatii.begin(); it != text_informatii.end(); ++it)
    {
        string text = *it;
        cout << (char)186 << " " << text << setw(columns-1-text.length()-1) << (char)186;
    }

    // Interogare SQL
    sqlite3 *db;
    sqlite3_stmt *stmt;

    // Deschidem baza de date
    int rc = sqlite3_open((ExePath() + "\\biblioteca.sqlite").c_str(), &db);
    if (rc)
    {
        cout << string("Eroare la deschiderea bazei de date: ") + sqlite3_errmsg(db);
        return;
    }

    string sql = "SELECT titlu, autor FROM carti WHERE imprumutat='" + to_string(utilizator.id) + "'";
    int counter = 0;
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL); // Pointer catre o portiune compilata
    if (rc == SQLITE_OK)
    {
        int idx = 0;
        while (TRUE)
        {
            rc = sqlite3_step(stmt); // Executa
            if (rc != SQLITE_ROW && rc != SQLITE_DONE)
            {
                string errmsg(sqlite3_errmsg(db));
                sqlite3_finalize(stmt); // Delete the compiled statement
                sqlite3_close(db);
                break;
            }

            if (rc == SQLITE_ROW)
            {
                string titlu = (char*)sqlite3_column_text(stmt, 0);
                string autor = (char*)sqlite3_column_text(stmt, 1);
                string text = titlu + ", " + autor;
                cout << setfill(' ') << (char)186 << "- " << text << setw(columns-text.length()-3) << (char)186;
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

    // Deseneaza restul chenarului
    for (int i=0; i<rows-counter-text_informatii.size()-4; i++)
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

DetaliiUtilizator::~DetaliiUtilizator()
{
    //dtor
}
