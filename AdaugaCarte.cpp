#include <iostream>
#include <sstream>
#include <conio.h>
#include <iomanip>
#include <windows.h>
#include "AdaugaCarte.h"
#include "sqlite3.h"

AdaugaCarte::AdaugaCarte(string titlu, vector<CampText> campuri, void (*succes)(), void (*cancel)())
{
    this->titlu = titlu;
    this->campuri = campuri;
    this->succes = succes;
    this->cancel = cancel;
}

AdaugaCarte::AdaugaCarte(Carte edit_carte, string titlu, vector<CampText> campuri, void (*succes)(), void (*cancel)())
{
    this->titlu = titlu;
    this->campuri = campuri;
    this->succes = succes;
    this->cancel = cancel;
    this->edit_carte = edit_carte;
}

// Pozitioneaza cursorul
void AdaugaCarte::activeaza_camp(CampText camp)
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

// Returneaza stringul introdus la un anume camp
string AdaugaCarte::valoare_camp(string camp_id)
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

// Afiseaza o eroare
void AdaugaCarte::afiseaza_eroare(string sql, string eroare)
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
// In caz ca afisam mai multe erori
void AdaugaCarte::afiseaza_erori(vector<string> erori)
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
        cout << (char)186 /*pt chenar*/ << setfill(' ')/*umplem cu spatii*/ << setw(columns-1) << (char)186; //pt chenar fine
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

// Verifica daca cartea s-a introdus/ updatat cu succes in baza de date
bool AdaugaCarte::salveaza()
{
    sqlite3 *db;
    char *err_msg = 0;

    vector<string> erori;

    // VALIDARI
    if (valoare_camp("titlu").length() == 0)
    {
        erori.push_back("Introduceti un titlu valid!");

    }
    // npos is a static member constant value with the greatest possible value for an element of type size_t.
    // size_t Unsigned integral type ==> Alias of one of the fundamental unsigned integer types.
    // It is a type able to represent the size of any object in bytes: size_t is the type returned by the sizeof operator
    //and is widely used in the standard library to represent sizes and  is widely used in the standard library to represent sizes and counts. counts.

    // Daca nu introducem nimic, sau daca introducem numere sau '.' sau daca numele incepe cu litera mica
    if(valoare_camp("autor").length() ==0 ||  valoare_camp("autor").find_first_of("0123456789.")!=std::string::npos|| valoare_camp("autor")[0]<'A' )
    {
        erori.push_back("Introduceti nume autor valid!");
        //  return FALSE;
    }
    // Daca nu introducem nimic sau daca introducem numere/punct
    if(valoare_camp("editura").length() ==0 ||  valoare_camp("editura").find_first_of("0123456789.")!=std::string::npos )
    {
        erori.push_back("Introduceti editura valida!");
        //  return FALSE;
    }
    // Daca nu introducem nimic sau daca numarul de cifre al numarului este mai mare ca 4 (mai multe pagini de 10000)
    if(valoare_camp("pagini").length() ==0 || valoare_camp("pagini").length()>4 || valoare_camp("pagini").find_first_of(".")!=std::string::npos)
    {
        erori.push_back("Introduceti un numar de pagini nenul si valid");
    }
    // Daca nu introducem nimic sau daca introducem un an format din 5 cifre sau daca introducem punct
    if(valoare_camp("an").length() ==0 || valoare_camp("an").length()>4|| valoare_camp("an").find_first_of(".")!=std::string::npos)
    {
        erori.push_back("Introduceti un an valid");
    }

    // VALIDARE ISBN
    // ISBN-ul trebuie sa contine un numar de 10 sau 13 cifre
    // Nu are importanta numarul de liniute si nici pozitia lor
    // Ultima cifra are o valoare deosebita, fiind o validare importanta, deoarace reprezinta o combinatie a cifrelor anterioare
    int count=0;
    vector<char> a;

    for(int i=0; i<valoare_camp("isbn").length(); i++)
        if(isalpha(valoare_camp("isbn")[i])) // Verificam daca introducem litere
            erori.push_back("ISBN-ul nu contine litere");
    for(int i=0; i<valoare_camp("isbn").length(); i++)
        if(valoare_camp("isbn")[i] == '-') // Numaram liniutele, pentru ca apoi sa le scadem
            count++;

    // Daca nr de cifre ramas(length()-"liniute") este diferit de 10 sau 13 ==> incorect dimension
    if((valoare_camp("isbn").length()-count) !=10 && (valoare_camp("isbn").length()-count) !=13)
        erori.push_back("Dimensiune incorecta ISBN ");
    else
    {

        for(int i=0; i<valoare_camp("isbn").length(); i++)
            if(isdigit(valoare_camp("isbn")[i])) // Verificam daca este cifra
                a.push_back(valoare_camp("isbn")[i]); // O punem in vectorul a pentru a verifica apoi algortimul de calcul al ultimei cifre

        int i, s = 0, t = 0;

        for (i = 0; i < 10; i++)
        {
            t += a[i];
            s += t;
        }
        if(s%11 !=0) // suma trebuie sa fie divizibila cu 11
            erori.push_back("ISBN INVALID! ");
    }



    if(erori.size()) // Verificam daca sunt erori in vector
    {
        afiseaza_erori(erori); // Le afisam
        return FALSE; // Si nu mergem mai departe la afisare
    }

    // Deschidem baza de date
    // ExePAth este calea catre fisier + fisierul propriu-zis de care avem nevoie( convertit la un sir de caractere )si adresa bazei de date
    int rc = sqlite3_open((ExePath() + "\\biblioteca.sqlite").c_str(), &db); // %%
    if (rc)
    {
        cout << string("Eroare la deschiderea bazei de date: ") + sqlite3_errmsg(db);
        return FALSE;
    }

    stringstream sql;

    if (edit_carte.id != 0)
    {

        sql << "UPDATE carti SET "
            << "titlu='" << valoare_camp("titlu") << "',"
            << "autor='" << valoare_camp("autor") << "',"
            << "editura='" << valoare_camp("editura") << "',"
            << "pagini='" << valoare_camp("pagini") << "',"
            << "an='" << valoare_camp("an") << "',"
            << "isbn='" << valoare_camp("isbn") << "' "
            << "WHERE id='" << edit_carte.id << "'"; // Updatam cartea cu id-ul respectiv
    }
    else
    {

        sql << "INSERT INTO carti (titlu,autor,editura,pagini,an,isbn,imprumutat) VALUES"
            << "('" << valoare_camp("titlu") << "',"
            << "'" << valoare_camp("autor") << "',"
            << "'" << valoare_camp("editura") << "',"
            << "'" << valoare_camp("pagini") << "',"
            << "'" << valoare_camp("an") << "',"
            << "'" << valoare_camp("isbn") << "',"
            << "'" << "0" << "')";
    }
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
        succes();
        return TRUE;
    }

    return FALSE;
}


void AdaugaCarte::afiseaza()
{
    int columns = get_console_size().columns;
    int rows = get_console_size().rows;
    current_index = 0;

    system("cls");
    SetConsoleTextAttribute(console, FOREGROUND_GREEN);

    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(console, &cursorInfo);
    cursorInfo.bVisible = TRUE;  // http://stackoverflow.com/questions/18028808/blinking-underscore-with-console
    SetConsoleCursorInfo(console, &cursorInfo);

    // Deseneaza partea de sus a chenarului
    cout << (char)201; // Afisam coltul
    for (int i=0; i<columns-2; i++)
    {
        cout << (char)205;
    }
    cout << (char)187; // Afisam coltul din dreapta

    // Deseneaza randul cu titlul
    int hspace = columns/2 - titlu.length()/2; // hspace=horizontal space
    cout << (char)186; // Afisam "=" culcat
    for (int i=1; i<hspace; i++)
    {
        cout << " "; // Pana la mesaj umplem cu spatii
    }
    cout << titlu; // Afisam titlu
    for (int i=hspace+titlu.length(); i<columns-1; i++) // Mergem in spatiul de dupa titlu
    {
        cout << " "; // Si umplem cu spatii
    }
    cout << (char)186; // Afisam "=" culcat
    cout << setfill(' ')/*umplem cu spatii*/ << (char)186 << setw(columns-1) << (char)186;  // Lasam un rand gol, de estetica

    for(vector<CampText>::iterator it = campuri.begin(); it != campuri.end(); ++it)
    {
        int index = it - campuri.begin();
        CampText &camp = *it;
        if (camp.valoare.length())
        {
            // setw ==> Sets the field width to be used on output operations.
            cout << (char)186 << " " << camp.titlu << camp.valoare << setw(columns-camp.titlu.length()-camp.valoare.length()-2)/*calculam cat sa lasam liber pana la finalul randului*/ << (char)186;
        }
        else
        {
            cout << (char)186 << " " << camp.titlu << setw(columns-camp.titlu.length()-2) << (char)186;
        }
    }

    // Deseneaza restul chenarului
    for (int i=6; i<rows-campuri.size(); i++)
    {
        cout << setfill(' ') << (char)186 << setw(columns-1) << (char)186;
    }
    cout << setfill((char)205) << (char)200 /*colt stanga jos*/<< (char)205 << setw(columns-2) << (char)188; // Completam randul de jos
    string butoane = "F2: Salveaza | Esc: Renunta";
    cout << setfill(' ') << butoane << setw(columns-butoane.length()) << " ";

    activeaza_camp(campuri[0]); // Incepem cu primul rand ( TITLU: ...)

    while (TRUE)
    {
        int key = getch();
        if (key == 0)    // F key... Pentru tastele F codul mai contine si un 0 in fata, de asta utilizam de doua ori getch()
        {
            if (getch() == 60) // F2
            {
                if (salveaza()) // Salvam
                {
                    break;
                }
            }
        }
        else if (key == 224)      // Vezi http://stackoverflow.com/questions/10463201/getch-and-arrow-codes
        {
            // empty the buffer
            getch();
        }
        else if (key == 8)      // backspace
        {
            CampText &camp = campuri[current_index];
            if (camp.valoare.length() == 0) continue; // Ca sa nu stergem cumva literele din Titlu:
            camp.valoare = camp.valoare.substr(0, camp.valoare.size()-1); // Substring incepand cu 0 pana la valoarea introdusa -1
            cout << (char)key; // Daca apasam backspace, ca sa stergem, mergem inapoi si inlocuim cu spatiu
            cout << " "; // Afisam spatiu
            cout << (char)key; // Si mergem iar unde eram
        }
        else if (key == 9)      // tab
        {
            current_index++;
            if (current_index >= campuri.size()) current_index = 0; // Daca suntem la ultima optiune si iteram , ne reintoarcem in prima
            activeaza_camp(campuri[current_index]); // Activam din nou primul camp
        }
        else if (key >= 32 && key <= 126) // Daca introducem litere si numere
        {
            CampText &camp = campuri[current_index];  // Daca nu folosim &camp obtinem o copie a obiectului
            if (camp.valoare.length() > camp.lungime) continue; // Limita de caractere introduse
            if (camp.tip == numeric && (key < 48 || key > 57)) continue;  // Daca campul e numeric si noi introducem litere
            camp.valoare += (char)key;
            cout << (char)key;
        }
        else if (key == 13) // Daca apasam ENTER
        {
            if (current_index == campuri.size()-1 && salveaza())
            {
                break;
            }
            else
            {
                current_index++;
                if (current_index >= campuri.size()) current_index = 0;
                activeaza_camp(campuri[current_index]);
            }
        }
        else if (key == 27) //ESCAPE
        {
            cancel(); // Iesim
        }
        else if (key == 255) //callback
        {
            afiseaza(); // Afisam din nou tot
            break;
        }
        //cout << key << " ";
    }
}

AdaugaCarte::~AdaugaCarte()
{
    //dtor
}
