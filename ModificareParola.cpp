#include <conio.h>
#include <windows.h>
#include <iomanip>
#include <sstream>
#include "ModificareParola.h"
#include "utils.h"
#include "sqlite3.h"

ModificareParola::ModificareParola(string titlu, string dbname, int user_id, void (*cancel)())
{
    this->titlu = titlu;
    this->dbname = dbname;
    this->user_id = user_id;
    this->cancel = cancel;

    titlu_camp = "Parola noua: ";
    max_field_length = 15;
}

void ModificareParola::afiseaza_eroare(string sql, string eroare)
{
    COORD last_cursor_pos = getCursorXY();
    int columns = get_console_size().columns;
    int rows = get_console_size().rows;

    int x_start = columns/2 - eroare.length()/2;
    COORD pos;
    pos.X = x_start;
    pos.Y = rows - 6;
    SetConsoleCursorPosition(console, pos);
    cout << sql << "\n" << eroare;
    SetConsoleCursorPosition(console, last_cursor_pos);
}

bool ModificareParola::salveaza_parola()
{
    sqlite3 *db;
    char *err_msg = 0;

    int rc = sqlite3_open((ExePath() + "\\biblioteca.sqlite").c_str(), &db);
    if (rc)
    {
        cout << string("Eroare la deschiderea bazei de date: ") + sqlite3_errmsg(db);
        return FALSE;
    }
// http://www.cplusplus.com/reference/sstream/stringstream/
// Pt operare cu stringuri
    stringstream sql;
    sql << "UPDATE " << dbname << " SET password='" << val_password << "' WHERE id='" << user_id << "'";
    rc = sqlite3_exec(db, sql.str().c_str(), 0, 0, &err_msg);

    if (rc)
    {
        sqlite3_close(db);
        afiseaza_eroare(sql.str(), string("Operatia a esuat: ") + err_msg);
        return FALSE;
    }
    else
    {
        sqlite3_close(db);
        cancel();
        return TRUE;
    }

    return FALSE;
}

// Setam pozitia cursorului
void ModificareParola::introducere_pass()
{
    int columns = get_console_size().columns;
    int rows = get_console_size().rows;
    int vspace = rows/2;
    int x_start = columns/2 - (titlu_camp.length()+max_field_length)/2 + val_password.length();

    COORD pos;
    pos.X = x_start + titlu_camp.length() + 1;
    pos.Y = vspace;
    SetConsoleCursorPosition(console, pos);
}


void ModificareParola::afiseaza()
{
    int columns = get_console_size().columns;
    int rows = get_console_size().rows;

    system("cls");
    SetConsoleTextAttribute(console, FOREGROUND_GREEN);
// set the cursor visibility, http://stackoverflow.com/questions/18028808/blinking-underscore-with-console
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(console, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(console, &cursorInfo);

    // Deseneaza partea de sus a chenarului
    cout << setfill((char)205) << (char)201 << (char)205 << setw(columns-2) << (char)187;

    // Deseneaza titlul
    cout << (char)186;
    for (int i=1; i<columns/2-titlu.length()/2; i++)
    {
        cout << " ";
    }
    cout << titlu;
    for (int i=columns/2-titlu.length()/2+titlu.length(); i<columns-1; i++)
    {
        cout << " ";
    }
    cout << (char)186;

    // Deseneaza restul chenarului
    for (int i=0; i<rows/2-2; i++)
    {
        cout << setfill(' ') << (char)186 << setw(columns-1) << (char)186;
    }

    // Deseneaza campul de intrare
    cout << (char)186;
    for (int i=1; i<columns/2-titlu_camp.length()/2-max_field_length/2; i++)
    {
        cout << " ";
    }
    cout << titlu_camp;
    for (int i=columns/2-titlu.length()/2+max_field_length/2; i<columns-1; i++)
    {
        cout << " ";
    }
    cout << (char)186;

    // Deseneaza restul chenarului
    for (int i=rows/2; i<rows-3; i++)
    {
        cout << setfill(' ') << (char)186 << setw(columns-1) << (char)186;
    }

    cout << setfill((char)205) << (char)200 << (char)205 << setw(columns-2) << (char)188;

    introducere_pass();

    while (int key = getch())
    {
        if (key == 224)    // vezi http://stackoverflow.com/questions/10463201/getch-and-arrow-codes
        {
            // empty the buffer
            getch();
        }
        else if (key == 8)      // backspace
        {
            if (val_password.length() == 0) continue;
            val_password = val_password.substr(0, val_password.size()-1);
            cout << (char)key;
            cout << " ";
            cout << (char)key;
        }
        else if (key == 9)      // tab
        {
            continue;
        }
        else if (key >= 32 && key <= 126)
        {
            if (val_password.length() > max_field_length) continue;
            val_password += (char)key;
            cout << (char)key;
        }
        else if (key == 13)
        {
            if (salveaza_parola()) break;
        }
        else if (key == 27)
        {
            cancel();
        }
        else if (key == 255)
        {
            afiseaza();
            break;
        }
    }
}

ModificareParola::~ModificareParola()
{
    //dtor
}
