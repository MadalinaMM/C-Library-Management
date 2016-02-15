#include <iostream>
#include <conio.h>
#include "sqlite3.h"
#include "EcranLogin.h"

static string nume_utilizator = "Nume utilizator: ";
static string parola_utilizator = "Parola utilizator: ";
static string val_username;
static string val_password;
static int max_field_length = 10;

Utilizator EcranLogin::current_user;

EcranLogin::EcranLogin(string titlu, string dbname, void (*succes)(), void (*cancel)()) // Constructor
{
    this->titlu = titlu;
    this->dbname = dbname;
    this->succes = succes;
    this->cancel = cancel;
}

// Pozitionare cursor username
void EcranLogin::introducere_user()
{
    int columns = get_console_size().columns;
    int rows = get_console_size().rows;
    int vposition = rows/2;
    int x_start = columns/2 - (nume_utilizator.length()+max_field_length)/2 + val_username.length();

    COORD pos;
    pos.X = x_start + nume_utilizator.length();
    pos.Y = vposition;
    SetConsoleCursorPosition(console, pos);
}
// Pozitionare cursor password
void EcranLogin::introducere_pass()
{
    int columns = get_console_size().columns;
    int rows = get_console_size().rows;
    int vposition = rows/2;
    int x_start = columns/2 - (parola_utilizator.length()+max_field_length)/2 + val_password.length();

    COORD pos;
    pos.X = x_start + parola_utilizator.length();
    pos.Y = vposition + 1;
    SetConsoleCursorPosition(console, pos);
}
// Autentificare esuata
void EcranLogin::afiseaza_eroare(string eroare)
{
    COORD last_cursor_pos = getCursorXY();
    int columns = get_console_size().columns;
    int rows = get_console_size().rows;

    int x_start = columns/2 - eroare.length()/2;
    COORD pos;
    pos.X = x_start;
    pos.Y = rows - 3;
    SetConsoleCursorPosition(console, pos);
    cout << eroare;
    SetConsoleCursorPosition(console, last_cursor_pos);
}

void EcranLogin::afiseaza()
{
    int columns = get_console_size().columns;
    int rows = get_console_size().rows;

    system("cls");
    val_username = "";
    val_password = "";
    SetConsoleTextAttribute(console, FOREGROUND_GREEN);

    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(console, &cursorInfo);
    cursorInfo.bVisible = TRUE;  // set the cursor visibility, http://stackoverflow.com/questions/18028808/blinking-underscore-with-console
    SetConsoleCursorInfo(console, &cursorInfo);

    // Deseneaza partea de sus a chenarului
    cout << (char)201;
    for (int i=0; i<columns-2; i++)
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

    // Deseneaza randuri goale pana cand e nevoie sa afisam campurile de intrare
    int vspace = rows/2;
    for (int i=2; i<vspace; i++)     // i=2 deoarece suntem deja pe al doilea rand
    {
        cout << (char)186;
        for (int j=2; j<columns; j++)
        {
            cout << " ";
        }
        cout << (char)186;
    }

    // Afiseaza campurile de intrare
    int x_start = columns/2 - (nume_utilizator.length()+max_field_length)/2;

    // Nume utilizator
    cout << (char)186;
    for (int i=1; i<x_start; i++)
    {
        cout << " ";
    }
    cout << nume_utilizator;
    for (int i=x_start+nume_utilizator.length(); i<columns-1; i++)
    {
        cout << " ";
    }
    cout << (char)186;

    // Parola utilizator
    cout << (char)186;
    for (int i=1; i<x_start; i++)
    {
        cout << " ";
    }
    cout << parola_utilizator;
    for (int i=x_start+parola_utilizator.length(); i<columns-1; i++)
    {
        cout << " ";
    }
    cout << (char)186;

    // Deseneaza randuri goale mai departe
    for (int i=vspace+2; i<rows-2; i++)    // i<rows-2 deoarece vrem sa lasam un rand
    {
        cout << (char)186;                              // pentru partea de jos a chenarului si totodata
        for (int j=2; j<columns; j++)                   // sa prevenim consola sa faca scroll
        {
            cout << " ";
        }
        cout << (char)186;
    }

    // Deseneaza partea de jos a chenarului
    cout << (char)200;
    for (int i=0; i<columns-2; i++)
    {
        cout << (char)205;
    }
    cout << (char)188;

    introducere_user();
    int current_index = 0; // user; pt parola e 1

    while (int key = getch())
    {
        if (key == 224)    // vezi http://stackoverflow.com/questions/10463201/getch-and-arrow-codes
        {
            // empty the buffer
            getch(); // consuma bufferul
        }
        else if (key == 8)      // backspace
        {
            if (current_index == 0)
            {
                if (val_username.length() == 0) continue;
                val_username = val_username.substr(0, val_username.size()-1); // stergem ultima
            }
            else if (current_index == 1)
            {
                if (val_password.length() == 0) continue;
                val_password = val_password.substr(0, val_password.size()-1);
            }
            // Backspace doar muta cursorul, nu sterge litera
            // Mutam cursorul, afisam spatiu si apoi iar mutam cursorul
            // Exemplu Notepad
            cout << (char)key;
            cout << " ";
            cout << (char)key;
        }
        else if (key == 9)      // tab
        {
            if (current_index == 0)
            {
                introducere_pass();
                current_index = 1;
            }
            else if (current_index == 1)
            {
                introducere_user();
                current_index = 0;
            }
        }
        else if (key >= 32 && key <= 126)     // A...z, 0...9
        {
            if (current_index == 0)  // Username
            {
                if (val_username.length() > max_field_length) continue;
                val_username += (char)key;
                cout << (char)key;
            }
            else if (current_index == 1)  // Password
            {
                if (val_password.length() > max_field_length) continue;
                val_password += (char)key;
                cout << "*"; // Ascundem parola
            }
        }
        else if (key == 13)
        {
            if (verifica_auth()) break;
        }
        else if (key == 27)
        {
            cancel();
        }
        else if (key == 255)      // Tasta speciala care redeseneaza ecranul;
        {
            afiseaza();
            break;
        }

    }
}

bool EcranLogin::verifica_auth()
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    // http://www.cplusplus.com/reference/string/string/c_str/
    int rc = sqlite3_open((ExePath() + "\\biblioteca.sqlite").c_str(), &db); // c_str() ==> Returns a pointer to an array that contains a null-terminated sequence of characters
    if (rc)
    {
        afiseaza_eroare(string("Eroare la deschiderea bazei de date: ") + sqlite3_errmsg(db));
        return FALSE;
    }

    string sql;
    if (dbname == "admin")
    {
        // http://www.codebashing.com/sql_demo
        sql = "SELECT id, username, password FROM admin WHERE username=? AND password=?";//injection "?";
    }
    else
    {
        sql = "SELECT id, username, password, nume, prenume FROM studenti WHERE username=? AND password=?";
    }
    // stmt ==> varianta compilata a codului sql
    // Compile SQL text into byte-code that will do the work of querying or updating the database.
    // The constructor for sqlite3_stmt. Returneaza SQLITE_OK daca s-a creat cu succes, sau o eroare altfel.
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    if (rc == SQLITE_OK)
    {
        //Pentru prevenirea injectiilor, sa nu lasam utilizatorul sa introduca ce doreste el; asteptam input de la user
        // https://www.sqlite.org/c3ref/bind_blob.html
        sqlite3_bind_text(stmt, 1/*username*/, val_username.c_str(), -1, NULL);
        sqlite3_bind_text(stmt, 2/*password*/, val_password.c_str(), -1, NULL);

        rc = sqlite3_step(stmt); // Evaluate si return SQL_ROW ori SQLITE
        // indicates that another row of output is available  ;   indicating that the SQL statement has run to completion ;
        if (rc != SQLITE_ROW && rc != SQLITE_DONE)
        {
            string errmsg(sqlite3_errmsg(db));
            sqlite3_finalize(stmt); // Destroy A Prepared Statement Object
            sqlite3_close(db);
            return FALSE;
        }

        if (rc == SQLITE_ROW)
        {
            Utilizator utilizator;
            utilizator.id = sqlite3_column_int(stmt, 0);
            utilizator.username = (char*)sqlite3_column_text(stmt, 1);
            utilizator.password = (char*)sqlite3_column_text(stmt, 2);
            if (dbname == "studenti")
            {
                utilizator.nume = (char*)sqlite3_column_text(stmt, 3);
                utilizator.prenume = (char*)sqlite3_column_text(stmt, 4);
            }

            current_user = utilizator;
        }

        if (rc == SQLITE_DONE)
        {
            afiseaza_eroare("Autentificare esuata!");
        }
        else
        {
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            succes();
            return TRUE;
        }
    }
    else
    {
        afiseaza_eroare(string("Eroare DB: ") + sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return FALSE;
}

EcranLogin::~EcranLogin()
{
    //dtor
}
