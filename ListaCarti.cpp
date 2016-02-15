#include <iostream>
#include <conio.h>
#include <iomanip>
#include "ListaCarti.h"
#include "AdaugaCarte.h"
#include "EcranLogin.h"
#include "CampText.h"
#include "sqlite3.h"

#define EDITURA_W 20
#define AN_W 10
#define IMPRUMUTAT_W 10
#define MAX_FILTRU_LENGTH 15

ListaCarti::ListaCarti(string titlu, filtru_carti filtrare, void (*selection)(Carte carte), void (*editare)(Carte carte), void (*cancel)())
{
    this->titlu = titlu;
    this->filtrare = filtrare;
    this->selection = selection;
    this->cancel = cancel;
    this->editare = editare;
}
int a;
int ListaCarti::row_for_index(int index)
{
    int rows = get_console_size().rows;

    return index + 3;
}
void ListaCarti::clear_selection(int index)
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

void ListaCarti::highlight_index(int index)
{
    if (indexuri.size() == 0) return;
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

void ListaCarti::editeaza_carte(int index)
{
    Carte carte = indexuri[index];
    editare(carte);
}

void ListaCarti::sterge_carte(int index)
{
    Carte carte = indexuri[index];
    sqlite3 *db;
    char *err_msg = 0;

    int rc = sqlite3_open((ExePath() + "\\biblioteca.sqlite").c_str(), &db);
    if (rc)
    {
        cout << string("Eroare la deschiderea bazei de date: ") + sqlite3_errmsg(db);
        return;
    }
    string sql = string("DELETE FROM carti WHERE id=") + to_string(carte.id);
    rc = sqlite3_exec(db, sql.c_str(), 0, 0, &err_msg);

    if (rc)
    {
        cout << "Stergerea a esuat: " << err_msg;
    }

    sqlite3_close(db);
    afiseaza();
}

void ListaCarti::imprumuta_carte(int index)
{
    Carte carte = indexuri[index];
    sqlite3 *db;
    char *err_msg = 0;

    int rc = sqlite3_open((ExePath() + "\\biblioteca.sqlite").c_str(), &db);
    if (rc)
    {
        cout << string("Eroare la deschiderea bazei de date: ") + sqlite3_errmsg(db);
        return;
    }
    string sql = string("UPDATE carti SET imprumutat=") + to_string(EcranLogin::current_user.id) +  " WHERE id=" + to_string(carte.id);
    rc = sqlite3_exec(db, sql.c_str(), 0, 0, &err_msg);

    if (rc)
    {
        cout << "Modificarea a esuat: " << err_msg;
    }

    sqlite3_close(db);
    afiseaza();
}

void ListaCarti::returneaza_carte(int index)
{
    Carte carte = indexuri[index];
    sqlite3 *db;
    char *err_msg = 0;

    int rc = sqlite3_open((ExePath() + "\\biblioteca.sqlite").c_str(), &db);
    if (rc)
    {
        cout << string("Eroare la deschiderea bazei de date: ") + sqlite3_errmsg(db);
        return;
    }
    string sql = string("UPDATE carti SET imprumutat=0 WHERE id=") + to_string(carte.id);
    rc = sqlite3_exec(db, sql.c_str(), 0, 0, &err_msg);

    if (rc)
    {
        cout << "Modificarea a esuat: " << err_msg;
    }

    sqlite3_close(db);
    afiseaza();
}

// Afisam cautarea din partea de jos a ecranului
void ListaCarti::afiseaza_cautare()
{
    int columns = get_console_size().columns;
    int rows = get_console_size().rows;
    string text = "Cautare: ";

    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(console, &cursorInfo);
    cursorInfo.bVisible = TRUE;  // set the cursor visibility, http://stackoverflow.com/questions/18028808/blinking-underscore-with-console
    SetConsoleCursorInfo(console, &cursorInfo);

    SetConsoleTextAttribute(console, FOREGROUND_GREEN);

    COORD newpos;
    newpos.X = columns - text.length() - MAX_FILTRU_LENGTH - 5;
    newpos.Y = rows - 2;
    SetConsoleCursorPosition(console, newpos);

    cout << text << filtru;
}

void ListaCarti::ascunde_cautare()
{
    int columns = get_console_size().columns;
    int rows = get_console_size().rows;
    string text = "Cautare: ";

    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(console, &cursorInfo);
    cursorInfo.bVisible = FALSE;  // set the cursor visibility, http://stackoverflow.com/questions/18028808/blinking-underscore-with-console
    SetConsoleCursorInfo(console, &cursorInfo);

    COORD newpos;
    newpos.X = columns - text.length() - MAX_FILTRU_LENGTH - 5;
    newpos.Y = rows - 2;
    SetConsoleCursorPosition(console, newpos);

    for (int i=0; i<text.length()+MAX_FILTRU_LENGTH; i++)
    {
        cout << " ";
    }
}

void ListaCarti::afiseaza()
{
    int columns = get_console_size().columns;
    int rows = get_console_size().rows;
    string c_titlu_text = "Titlu";
    string c_autor_text = "Autor";
    string c_editura_text = "Editura";
    string c_an_text = "An";
    string c_imprumutat_text = "Imprum.";

    int title_column_w = columns - 40;  // 40 = 20 pentru Editura, 10 pentru An, 10 pentru Imprumutat
    system("cls");
    SetConsoleTextAttribute(console, FOREGROUND_GREEN);

    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(console, &cursorInfo);
    cursorInfo.bVisible = FALSE;  // set the cursor visibility, http://stackoverflow.com/questions/18028808/blinking-underscore-with-console
    SetConsoleCursorInfo(console, &cursorInfo);

    // Deseneaza partea de sus a chenarului
    cout << (char)201;
    // Titlu
    for (int i=0; i<title_column_w/2-2; i++)    // -2 deoarece aici avem si coltul din stanga-sus
    {
        cout << (char)205;
    }
    cout << (char)209;  // Separator de coloana - https://www.google.ro/search?q=ascii+tabel&tbm=isch&imgil=IvVP_xaycIIKOM%253A%253BJMtOixefP_tDJM%253Bhttp%25253A%25252F%25252Fwww.asciitable.com%25252F&source=iu&pf=m&fir=IvVP_xaycIIKOM%253A%252CJMtOixefP_tDJM%252C_&biw=1536&bih=701&ved=0ahUKEwi-46TwwrjKAhUDWiwKHTzCCSwQyjcIMg&ei=Jo-fVr64DoO0sQG8hKfgAg#imgrc=IvVP_xaycIIKOM%3A
    // Autor
    for (int i=0; i<title_column_w/2-1; i++)
    {
        cout << (char)205;
    }
    cout << (char)209;  // Separator de coloana
    // Editura
    for (int i=0; i<EDITURA_W-1; i++)
    {
        cout << (char)205;
    }
    cout << (char)209;  // Separator de coloana
    // An
    for (int i=0; i<AN_W-1; i++)
    {
        cout << (char)205;
    }
    cout << (char)209;  // Separator de coloana
    // Imprumutat
    for (int i=0; i<IMPRUMUTAT_W-1; i++)
    {
        cout << (char)205;
    }
    cout << (char)187;

    // Randul cu titlurile coloanelor
    // Titlu
    cout << (char)186;
    int titlu_x_start = title_column_w/4 - c_titlu_text.length()/2;
    for (int i=1; i<titlu_x_start; i++)
    {
        cout << " ";
    }
    cout << c_titlu_text;
    for (int i=titlu_x_start+c_titlu_text.length(); i<title_column_w/2-1; i++)
    {
        cout << " ";
    }
    cout << (char)179;

    // Autor
    int autor_x_start = title_column_w/4 - c_autor_text.length()/2;
    for (int i=1; i<autor_x_start; i++)
    {
        cout << " ";
    }
    cout << c_autor_text;
    for (int i=autor_x_start+c_autor_text.length(); i<title_column_w/2; i++)
    {
        cout << " ";
    }
    cout << (char)179;

    // Editura
    int editura_x_start = EDITURA_W/2 - c_editura_text.length()/2;
    for (int i=1; i<editura_x_start; i++)
    {
        cout << " ";
    }
    cout << c_editura_text;
    for (int i=editura_x_start+c_editura_text.length(); i<EDITURA_W; i++)
    {
        cout << " ";
    }
    cout << (char)179;

    // An
    int an_x_start = AN_W/2 - c_an_text.length()/2;
    for (int i=1; i<an_x_start; i++)
    {
        cout << " ";
    }
    cout << c_an_text;
    for (int i=an_x_start+c_an_text.length(); i<AN_W; i++)
    {
        cout << " ";
    }
    cout << (char)179;

    // Imprumutat
    int imprumutat_x_start = IMPRUMUTAT_W/2 - c_imprumutat_text.length()/2;
    for (int i=1; i<imprumutat_x_start; i++)
    {
        cout << " ";
    }
    cout << c_imprumutat_text;
    for (int i=imprumutat_x_start+c_imprumutat_text.length(); i<IMPRUMUTAT_W; i++)
    {
        cout << " ";
    }
    cout << (char)186;

    // Separator
    cout << (char)199;
    for (int i=1; i<title_column_w/2-1; i++)
    {
        cout << (char)196;
    }
    cout << (char)197;
    for (int i=title_column_w/2+1; i<title_column_w; i++)
    {
        cout << (char)196;
    }
    cout << (char)197;
    for (int i=1; i<EDITURA_W; i++)
    {
        cout << (char)196;
    }
    cout << (char)197;
    for (int i=1; i<AN_W; i++)
    {
        cout << (char)196;
    }
    cout << (char)197;
    for (int i=1; i<IMPRUMUTAT_W; i++)
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
    string sql = "SELECT id, titlu, autor, editura, an, imprumutat, pagini, isbn FROM carti ";
    if (filtru.length()) // filtru= 'black' din 'black beauty'
    {
        sql += string(" WHERE (titlu LIKE '%") + filtru + "%')";
        sql += string(" OR (autor LIKE '%") + filtru + "%')";
        sql += string(" OR (editura LIKE '%") + filtru + "%')";
    }
    if (filtrare != toate) // In cazul in care vrem sa vedem cartile disponibile sau cele imprumutate
    {
        if (filtru.length())
        {
            sql += " AND";
        }
        else
        {
            sql += " WHERE";
        }
        switch (filtrare)
        {
        case disponibile:
            sql += " imprumutat='0'";
            break;
        case imprumutate:
            sql += " imprumutat='" + to_string(EcranLogin::current_user.id) + "'";
            break;
        }
    }
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
                Carte carte;
                carte.id = sqlite3_column_int(stmt, 0);
                carte.titlu = (char*)sqlite3_column_text(stmt, 1);
                carte.autor = (char*)sqlite3_column_text(stmt, 2);
                carte.editura = (char*)sqlite3_column_text(stmt, 3);
                carte.an = (char*)sqlite3_column_text(stmt, 4);
                carte.imprumutat = sqlite3_column_int(stmt, 5);
                carte.nr_pag = (char*)sqlite3_column_text(stmt, 6);
                carte.isbn = (char*)sqlite3_column_text(stmt, 7);

                indexuri[idx] = carte;

                // Titlul cartii
                cout << (char)186;
                int titlu_carte_x_start = title_column_w/4 - carte.titlu.length()/2;
                for (int i=1; i<titlu_carte_x_start; i++)
                {
                    cout << " ";
                }
                cout << carte.titlu;
                for (int i=titlu_carte_x_start+carte.titlu.length(); i<title_column_w/2-1; i++)
                {
                    cout << " ";
                }
                cout << (char)179;

                // Autorul cartii
                int autor_carte_x_start = title_column_w/4 - carte.autor.length()/2;
                for (int i=1; i<autor_carte_x_start; i++)
                {
                    cout << " ";
                }
                cout << carte.autor;
                for (int i=autor_carte_x_start+carte.autor.length(); i<title_column_w/2; i++)
                {
                    cout << " ";
                }
                cout << (char)179;

                // Editura cartii
                int editura_carte_x_start = EDITURA_W/2 - carte.editura.length()/2;
                for (int i=1; i<editura_carte_x_start; i++)
                {
                    cout << " ";
                }
                cout << carte.editura;
                for (int i=editura_carte_x_start+carte.editura.length(); i<EDITURA_W; i++)
                {
                    cout << " ";
                }
                cout << (char)179;

                // Anul cartii
                int an_carte_x_start = AN_W/2 - carte.an.length()/2;
                for (int i=1; i<an_carte_x_start; i++)
                {
                    cout << " ";
                }
                cout << carte.an;
                for (int i=an_carte_x_start+carte.an.length(); i<AN_W; i++)
                {
                    cout << " ";
                }
                cout << (char)179;

                // Imprumutat
                string imprumutat_carte = (carte.imprumutat == 0) ? "NU" : "DA";
                int imprumutat_carte_x_start = IMPRUMUTAT_W/2 - imprumutat_carte.length()/2;
                for (int i=1; i<imprumutat_carte_x_start; i++)
                {
                    cout << " ";
                }
                cout << imprumutat_carte;
                for (int i=imprumutat_carte_x_start+imprumutat_carte.length(); i<IMPRUMUTAT_W; i++)
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
        for (int j=1; j<title_column_w/2-1; j++)
        {
            cout << " ";
        }
        cout << (char)179;
        for (int j=1; j<title_column_w/2; j++)
        {
            cout << " ";
        }
        cout << (char)179;
        for (int j=1; j<EDITURA_W; j++)
        {
            cout << " ";
        }
        cout << (char)179;
        for (int j=1; j<AN_W; j++)
        {
            cout << " ";
        }
        cout << (char)179;
        for (int j=1; j<IMPRUMUTAT_W; j++)
        {
            cout << " ";
        }
        cout << (char)186;
    }

    // Deseneaza latura de jos a chenarului
    cout << (char)200;
    for (int j=1; j<title_column_w/2-1; j++)
    {
        cout << char(205);
    }
    cout << (char)207;
    for (int j=1; j<title_column_w/2; j++)
    {
        cout << char(205);
    }
    cout << (char)207;
    for (int j=1; j<EDITURA_W; j++)
    {
        cout << char(205);
    }
    cout << (char)207;
    for (int j=1; j<AN_W; j++)
    {
        cout << char(205);
    }
    cout << (char)207;
    for (int j=1; j<IMPRUMUTAT_W; j++)
    {
        cout << char(205);
    }
    cout << (char)188;

    // Afiseaza tastele pentru modificarea listei
    if (editare != NULL)
    {
        string taste = "F2: Modifica | F8: Sterge";
        cout << taste;
        for (int i=taste.length(); i<columns; i++)
        {
            cout << " ";
        }
    }
    else
    {
        string taste;
        if (filtrare == disponibile)
        {
            taste = "F2: Imprumuta";
        }
        else if (filtrare == imprumutate)
        {
            taste = "F2: Returneaza";
        }
        cout << taste;
        for (int i=taste.length(); i<columns; i++)
        {
            cout << " ";
        }
    }

    current_index = 0;
    highlight_index(current_index);
    if (filtru.length())
    {
        afiseaza_cautare();
    }

    while (TRUE)
    {
        int key = getch();
        if (key == 13)
        {
            if (indexuri.size() == 0) continue;
            if (filtru.length())
            {
                afiseaza();
                break;
            }
            Carte carte = indexuri[current_index];
            selection(carte);
            break;
        }
        else if (key == 224)      // vezi http://stackoverflow.com/questions/10463201/getch-and-arrow-codes
        {
            // empty the buffer, we don't want arrow keys to appear as letters
            int ch = getch();
            if (indexuri.size() == 0) continue;
            if (ch == 80)
            {
                clear_selection(current_index);
                current_index += 1;
                if (current_index > indexuri.size()-1) current_index = 0;
                highlight_index(current_index);
            }
            else if (ch == 72)
            {
                clear_selection(current_index);
                current_index -= 1;
                if (current_index < 0) current_index = indexuri.size() - 1;
                highlight_index(current_index);
            }
        }
        else if (key >= 32 && key <= 126)
        {
            if (filtru.length() == 0) afiseaza_cautare();
            if (filtru.length() > MAX_FILTRU_LENGTH) continue;
            filtru += (char)key;
            cout << (char)key;
        }
        else if (key == 8)      // backspace
        {
            filtru = filtru.substr(0, filtru.size()-1);
            cout << (char)key;
            cout << " ";
            cout << (char)key;
            if (filtru.length() == 0)
            {
                ascunde_cautare();
                afiseaza();
                continue;
            }
        }
        else if (key == 255)
        {
            afiseaza();
            break;
        }
        else if (key == 27)
        {
            if (filtru.length())
            {
                filtru = "";
                afiseaza();
                ascunde_cautare();
                continue;
            }
            cancel();
            break;
        }
        else if (key == 0)      // S-a apasat una din tastele F1, F2, F3, etc...
        {
            int f = getch();
            if (indexuri.size() == 0) continue;
            if (editare != NULL)    // Utilizatorul este admin
            {
                if (f == 60)    // F2
                {
                    editeaza_carte(current_index);
                }
                else if (f == 66)      // F8
                {
                    sterge_carte(current_index);
                }
            }
            else      // Utilizatorul este student
            {
                if (filtrare == disponibile)
                {
                    imprumuta_carte(current_index);
                }
                else if (filtrare == imprumutate)
                {
                    returneaza_carte(current_index);
                }
            }
        }
    }
}

ListaCarti::~ListaCarti()
{
    //dtor
}
