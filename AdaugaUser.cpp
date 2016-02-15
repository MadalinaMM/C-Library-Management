#include <iostream>
#include <sstream>
#include <conio.h>
#include <iomanip>
#include <windows.h>
#include "AdaugaUser.h"
#include <sqlite3.h>

AdaugaUser::AdaugaUser(string titlu, vector<CampText> campuri, void (*succes)(), void (*cancel)())
{
    this->titlu = titlu;
    this->campuri = campuri;
    this->succes = succes;
    this->cancel = cancel;
}
AdaugaUser::AdaugaUser(Utilizator edit_user, string titlu, vector<CampText> campuri, void (*succes)(), void (*cancel)())
{
    this->titlu = titlu;
    this->campuri = campuri;
    this->succes = succes;
    this->cancel = cancel;
    this->edit_user = edit_user;
}
// Activam capurile
void AdaugaUser::activeaza_camp(CampText camp)
{
    for(vector<CampText>::iterator it = campuri.begin(); it != campuri.end(); ++it)
    {
        int index = it - campuri.begin();
        CampText &ccamp = *it;
        if (ccamp.id == camp.id)
        {
            COORD pos;
            pos.X = 2 + camp.titlu.length() + camp.valoare.length();
            pos.Y = 3 + index;
            SetConsoleCursorPosition(console, pos);
        }
    }
}
// Returnam valaorea campului
string AdaugaUser::valoare_camp(string camp_id)
{
    for(vector<CampText>::iterator it = campuri.begin(); it != campuri.end(); ++it)
    {
        int index = it - campuri.begin();
        CampText &ccamp = *it;
        if (ccamp.id == camp_id)
        {
            return ccamp.valoare;
        }
    }

    return NULL;
}

void AdaugaUser::afiseaza_eroare(string eroare)
{
    COORD last_cursor_pos = getCursorXY();
    int columns = get_console_size().columns;
    int rows = get_console_size().rows;

    int x_start = columns/2 - eroare.length()/2;
    COORD pos;
    pos.X = x_start;
    pos.Y = rows - 5;
    SetConsoleCursorPosition(console, pos);
    cout << eroare;
    SetConsoleCursorPosition(console, last_cursor_pos);
}
void AdaugaUser::afiseaza_erori(vector<string> erori)
{
    COORD last_cursor_pos = getCursorXY();
    int columns = get_console_size().columns;
    int rows = get_console_size().rows;
    int crescendo =10;
    COORD pos;
    pos.X = 0;
    pos.Y = rows/2;
    SetConsoleCursorPosition(console, pos);

    for (int y=0; y<rows/2-3; y++)
    {
        cout << (char)186 << setfill(' ') << setw(columns-1) << (char)186;
    }

    for(int i = 0; i < erori.size(); i++)
    {
        int x_start = columns/2 - erori[i].length()/2;
        pos.X = x_start;
        pos.Y = rows - crescendo;
        SetConsoleCursorPosition(console, pos);
        cout << erori[i];
        SetConsoleCursorPosition(console, last_cursor_pos);
        crescendo--;

    }
}


// Salvarea/ Update in baza de date
bool AdaugaUser::salveaza()
{
    sqlite3 *db; // Pointer la baza de date
    char *err_msg = 0;
    vector<string> erori; // Vector de erori

    // VALIDARI

    // Daca nu introducem nimic sau daca introducem litere sau punct sau daca prima litera e mica
    if(valoare_camp("nume").length() ==0 ||  valoare_camp("nume").find_first_of("0123456789.")!=std::string::npos || islower(valoare_camp("nume")[0] ))
    {
        erori.push_back("Introduceti un nume valid!");
        // return FALSE;
    }

    // Daca nu introducem nimic sau daca introducem litere sau punct sau daca prima litera e mica
    if(valoare_camp("prenume").length() ==0 ||  valoare_camp("prenume").find_first_of("0123456789.")!=std::string::npos || islower(valoare_camp("prenume")[0] ))
    {
        erori.push_back("Introduceti un prenume valid!");
        // return FALSE;
    }

    // Daca nu introducem nimic
    if(valoare_camp("username").length() ==0 )
    {
        erori.push_back("Introduceti USERNAME!");
        // return FALSE;
    }
    // Daca nu introducem nimic
    if(valoare_camp("password").length() ==0 )
    {
        erori.push_back("Introduceti PASSWORD!");
        // return FALSE;
    }

    if(erori.size()) // Daca exista erori
    {
        afiseaza_erori(erori);
        return FALSE;
    }



    // Deschidem baza de date
    int rc = sqlite3_open((ExePath() + "\\biblioteca.sqlite").c_str(), &db);
    if (rc)
    {
        cout << string("Eroare la deschiderea bazei de date: ") + sqlite3_errmsg(db);
        return FALSE;
    }

    stringstream sql;
    //  cout << edit_user.id;
    getch();
    if (edit_user.id != 0) // Update
    {
        sql << "UPDATE studenti SET "
            << "username='" << valoare_camp("username") << "',"
            << "password='" << valoare_camp("password") << "',"
            << "nume='" << valoare_camp("nume") << "',"
            << "prenume='" << valoare_camp("prenume") << "' "
            << "WHERE id='" << edit_user.id << "'";
    }
    else
    {
        // Insert
        sql << "INSERT INTO studenti (username,password,nume,prenume) VALUES"
            << "('" << valoare_camp("username") << "',"
            << "'" << valoare_camp("password") << "',"
            << "'" << valoare_camp("nume") << "',"
            << "'" << valoare_camp("prenume") << "')";
    }
    // Facem modificarile in baza de date
    rc = sqlite3_exec(db, sql.str().c_str(), 0, 0, &err_msg);

    if (rc) // Daca nu merge
    {
        sqlite3_close(db);
        afiseaza_eroare(string("Operatia a esuat: ") + err_msg);
        return FALSE;
    }
    else
    {
        sqlite3_close(db); // Inchidem baza de date
        succes(); // Listare....
        return TRUE;
    }

    return FALSE;
}

void AdaugaUser::afiseaza()
{
    int columns = get_console_size().columns; // Nr. de coloane
    int rows = get_console_size().rows; // Nr. de linii
    current_index = 0; // Indexul curent

    system("cls"); // Stergem ecranul
    SetConsoleTextAttribute(console, FOREGROUND_GREEN); // Verde

    CONSOLE_CURSOR_INFO cursorInfo; // Setam vizibilitatea cursorului
    GetConsoleCursorInfo(console, &cursorInfo);
    cursorInfo.bVisible = TRUE;  // set the cursor visibility, http://stackoverflow.com/questions/18028808/blinking-underscore-with-console
    SetConsoleCursorInfo(console, &cursorInfo);

    // Deseneaza partea de sus a chenarului
    cout << (char)201; // Colt
    for (int i=0; i<columns-2; i++)
    {
        cout << (char)205; // "="
    }
    cout << (char)187; //Colt

    // Deseneaza randul cu titlul
    int hspace = columns/2 - titlu.length()/2; // Horizontal space
    cout << (char)186; // "=" culcat
    for (int i=1; i<hspace; i++)
    {
        cout << " "; // Umplem cu spatii
    }
    cout << titlu; // Afisam titlul
    for (int i=hspace+titlu.length(); i<columns-1; i++)
    {
        cout << " "; // Umplem cu spatii pana la capatul randului
    }
    cout << (char)186;
    cout << setfill(' ') << (char)186 << setw(columns-1) << (char)186;  // Lasam un rand gol, de estetica

    for(vector<CampText>::iterator it = campuri.begin(); it != campuri.end(); ++it)
    {
        int index = it - campuri.begin();
        CampText &camp = *it;
        if (camp.valoare.length()) // Daca scriem ceva, afisam
        {
            cout << (char)186 << " " << camp.titlu << camp.valoare << setw(columns-camp.titlu.length()-camp.valoare.length()-2) << (char)186;
        }
        else
        {
            cout << (char)186 << " " << camp.titlu << setw(columns-camp.titlu.length()-2) << (char)186;
        }
    }

    // Deseneaza restul chenarului
    for (int i=6; i<rows-campuri.size(); i++)
    {
        cout << setfill(' ') << (char)186 << setw(columns-1) << (char)186; // Umplem cu spatii
    }
    cout << setfill((char)205) << (char)200 << (char)205 << setw(columns-2) << (char)188;
    string butoane = "F2: Salveaza | Esc: Renunta";
    cout << setfill(' ') << butoane << setw(columns-butoane.length()) << " ";

    activeaza_camp(campuri[0]); // Activam primul camp

    while (TRUE)
    {
        int key = getch();
        if (key == 0)    // F key...
        {
            if (getch() == 60) // F2
            {
                if (salveaza())
                {
                    break;
                }
            }
        }
        else if (key == 224)      // vezi http://stackoverflow.com/questions/10463201/getch-and-arrow-codes
        {
            // empty the buffer
            getch();
        }
        else if (key == 8)      // backspace
        {
            CampText &camp = campuri[current_index];
            if (camp.valoare.length() == 0) continue; // Avem grija sa nu stergem titlu
            camp.valoare = camp.valoare.substr(0, camp.valoare.size()-1);
            cout << (char)key; // Pt functionarea BackSpace
            cout << " ";
            cout << (char)key; // Pt functionarea BackSpece
        }
        else if (key == 9)      // tab
        {
            current_index++;
            if (current_index >= campuri.size()) current_index = 0;
            activeaza_camp(campuri[current_index]);
        }
        else if (key >= 32 && key <= 126)
        {
            CampText &camp = campuri[current_index];  // daca nu folosim &camp obtinem o copie a obiectului
            if (camp.valoare.length() > camp.lungime) continue;
            if (camp.tip == numeric && (key < 48 || key > 57)) continue;
            camp.valoare += (char)key;
            cout << (char)key;
        }
        else if (key == 13) //ENTER
        {
            if (current_index == campuri.size()-1)
            {
                if (salveaza())
                {
                    break;
                }

            }
            else
            {
                current_index++;
                if (current_index >= campuri.size()) current_index = 0;
                activeaza_camp(campuri[current_index]);
            }
        }
        else if (key == 27) // ESCAPE
        {
            cancel();
        }
        else if (key == 255) // Callback
        {
            afiseaza(); // Afisam din nou
            break;
        }
    }
}

AdaugaUser::~AdaugaUser()
{
    //dtor
}
