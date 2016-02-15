#include <conio.h>
#include "ListaUtilizatori.h"
#include "sqlite3.h"

ListaUtilizatori::ListaUtilizatori(string titlu, void (*selection)(Utilizator utilizator), void (*editare)(Utilizator utilizator), void (*cancel)())
{
    this->titlu = titlu;
    this->selection = selection;
    this->cancel = cancel;
    this->editare = editare;
}

int ListaUtilizatori::row_for_index(int index)
{
    int rows = get_console_size().rows;

    return index + 3;
}

void ListaUtilizatori::clear_selection(int index)
{
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

void ListaUtilizatori::highlight_index(int index)
{
    int columns = get_console_size().columns;

    int row_of_index = row_for_index(index);
    COORD pos;
    pos.X = 1;
    pos.Y = row_of_index;

    SetConsoleTextAttribute(console, FOREGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED);
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

void ListaUtilizatori::editeaza_utilizator(int index)
{
    Utilizator utilizator = indexuri[index];
    editare(utilizator);
}

void ListaUtilizatori::sterge_utilizator(int index)
{
    Utilizator utilizator = indexuri[index];
    sqlite3 *db;
    char *err_msg = 0;

    int rc = sqlite3_open((ExePath() + "\\biblioteca.sqlite").c_str(), &db);
    if (rc)
    {
        cout << string("Eroare la deschiderea bazei de date: ") + sqlite3_errmsg(db);
        return;
    }
    string sql = string("DELETE FROM studenti WHERE id=") + to_string(utilizator.id);
    rc = sqlite3_exec(db, sql.c_str(), 0, 0, &err_msg);

    if (rc)
    {
        cout << "Stergerea a esuat: " << err_msg;
    }

    sql = "UPDATE carti SET imprumutat=0 WHERE imprumutat=" + to_string(utilizator.id);
    rc = sqlite3_exec(db, sql.c_str(), 0, 0, &err_msg);

    if (rc)
    {
        cout << "Operatia a esuat: " << err_msg;
    }

    sqlite3_close(db);
    afiseaza();
}

void ListaUtilizatori::afiseaza()
{
    int columns = get_console_size().columns;
    int rows = get_console_size().rows;
    string c_nume_text = "Nume";
    string c_prenume_text = "Prenume";
    string c_username_text = "Nume utilizator";

    int column_w = columns/3;

    system("cls");
    SetConsoleTextAttribute(console, FOREGROUND_GREEN);

    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(console, &cursorInfo);
    cursorInfo.bVisible = FALSE;  // set the cursor visibility, http://stackoverflow.com/questions/18028808/blinking-underscore-with-console
    SetConsoleCursorInfo(console, &cursorInfo);

    // Deseneaza partea de sus a chenarului
    cout << (char)201;
    // Nume
    for (int i=0; i<column_w-2; i++)    // -2 deoarece aici avem si coltul din stanga-sus
    {
        cout << (char)205;
    }
    cout << (char)209;  // Separator de coloana
    // Prenume
    for (int i=0; i<column_w-1; i++)
    {
        cout << (char)205;
    }
    cout << (char)209;  // Separator de coloana
    // Username
    for (int i=0; i<column_w-1; i++)
    {
        cout << (char)205;
    }
    cout << (char)187;

    // Randul cu titlurile coloanelor
    // Nume
    cout << (char)186;
    int nume_x_start = column_w/2 - c_nume_text.length()/2;
    for (int i=1; i<nume_x_start; i++)
    {
        cout << " ";
    }
    cout << c_nume_text;
    for (int i=nume_x_start+c_nume_text.length(); i<column_w-1; i++)
    {
        cout << " ";
    }
    cout << (char)179;

    // Prenume
    int prenume_x_start = column_w/2 - c_prenume_text.length()/2;
    for (int i=1; i<prenume_x_start; i++)
    {
        cout << " ";
    }
    cout << c_prenume_text;
    for (int i=prenume_x_start+c_prenume_text.length(); i<column_w; i++)
    {
        cout << " ";
    }
    cout << (char)179;

    // Username
    int username_x_start = column_w/2 - c_username_text.length()/2;
    for (int i=1; i<username_x_start; i++)
    {
        cout << " ";
    }
    cout << c_username_text;
    for (int i=username_x_start+c_username_text.length(); i<column_w; i++)
    {
        cout << " ";
    }
    cout << (char)186;

    // Separator
    cout << (char)199;
    for (int i=1; i<column_w-1; i++)
    {
        cout << (char)196;
    }
    cout << (char)197;
    for (int i=1; i<column_w; i++)
    {
        cout << (char)196;
    }
    cout << (char)197;
    for (int i=1; i<column_w; i++)
    {
        cout << (char)196;
    }
    cout << (char)182;

    // Interogare SQL
    sqlite3 *db;
    sqlite3_stmt *stmt;
    indexuri.clear();

    int rc = sqlite3_open((ExePath() + "\\biblioteca.sqlite").c_str(), &db);
    if (rc)
    {
        cout << string("Eroare la deschiderea bazei de date: ") + sqlite3_errmsg(db);
        return;
    }
    string sql = "SELECT id, username, password, nume, prenume FROM studenti";
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    if (rc == SQLITE_OK)
    {
        int idx = 0;
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
                Utilizator utilizator;
                utilizator.id = sqlite3_column_int(stmt, 0);
                utilizator.username = (char*)sqlite3_column_text(stmt, 1);
                utilizator.password = (char*)sqlite3_column_text(stmt, 2);
                utilizator.nume = (char*)sqlite3_column_text(stmt, 3);
                utilizator.prenume = (char*)sqlite3_column_text(stmt, 4);

                indexuri[idx] = utilizator;

                // Nume
                cout << (char)186;
                int nume_utilizator_x_start = column_w/2 - utilizator.nume.length()/2;
                for (int i=1; i<nume_utilizator_x_start; i++)
                {
                    cout << " ";
                }
                cout << utilizator.nume;
                for (int i=nume_utilizator_x_start+utilizator.nume.length(); i<column_w-1; i++)
                {
                    cout << " ";
                }
                cout << (char)179;

                // Prenume
                int prenume_utilizator_x_start = column_w/2 - utilizator.prenume.length()/2;
                for (int i=1; i<prenume_utilizator_x_start; i++)
                {
                    cout << " ";
                }
                cout << utilizator.prenume;
                for (int i=prenume_utilizator_x_start+utilizator.prenume.length(); i<column_w; i++)
                {
                    cout << " ";
                }
                cout << (char)179;

                // Editura cartii
                int username_utilizator_x_start = column_w/2 - utilizator.username.length()/2;
                for (int i=1; i<username_utilizator_x_start; i++)
                {
                    cout << " ";
                }
                cout << utilizator.username;
                for (int i=username_utilizator_x_start+utilizator.username.length(); i<column_w; i++)
                {
                    cout << " ";
                }
                cout << (char)186;

                idx++;
            }
            else
            {
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                break;
            }
        }
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    // Deseneaza randurile goale, daca e nevoie
    for (int i=indexuri.size(); i<rows-6; i++)
    {
        cout << (char)186;
        for (int j=1; j<column_w-1; j++)
        {
            cout << " ";
        }
        cout << (char)179;
        for (int j=1; j<column_w; j++)
        {
            cout << " ";
        }
        cout << (char)179;
        for (int j=1; j<column_w; j++)
        {
            cout << " ";
        }
        cout << (char)186;
    }

    // Deseneaza latura de jos a chenarului
    cout << (char)200;
    for (int j=1; j<column_w-1; j++)
    {
        cout << char(205);
    }
    cout << (char)207;
    for (int j=1; j<column_w; j++)
    {
        cout << char(205);
    }
    cout << (char)207;
    for (int j=1; j<column_w; j++)
    {
        cout << char(205);
    }
    cout << (char)188;

    // Afiseaza tastele pentru modificarea listei
    string taste = "F2: Modifica | F8: Sterge";
    cout << taste;
    for (int i=taste.length(); i<columns; i++)
    {
        cout << " ";
    }

    current_index = 0;
    highlight_index(current_index);

    while (TRUE)
    {
        int key = getch();
        if (key == 80)
        {
            clear_selection(current_index);
            current_index += 1;
            if (current_index > indexuri.size()-1) current_index = 0;
            highlight_index(current_index);
        }
        else if (key == 72)
        {
            clear_selection(current_index);
            current_index -= 1;
            if (current_index < 0) current_index = indexuri.size() - 1;
            highlight_index(current_index);
        }
        else if (key == 13)
        {
            selection(indexuri[current_index]);
            break;
        }
        else if (key == 255)
        {
            afiseaza();
            break;
        }
        else if (key == 27)
        {
            cancel();
            break;
        }
        else if (key == 0)      // S-a apasat una din tastele F1, F2, F3, etc...
        {
            int f = getch();
            if (f == 60)    // F2
            {
                editeaza_utilizator(current_index);
            }
            else if (f == 66)      // F8
            {
                sterge_utilizator(current_index);
            }
        }
    }
}

ListaUtilizatori::~ListaUtilizatori()
{
    //dtor
}
