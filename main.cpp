#include <iostream>
#include <process.h> // _beginthread
#include <vector>
#include <conio.h>
#include <windows.h>
#include "Meniu.h"
#include "EcranLogin.h"
#include "ListaCarti.h"
#include "DetaliiCarte.h"
#include "Carte.h"
#include "AdaugaCarte.h"
#include "AdaugaUser.h"
#include "DetaliiUtilizator.h"
#include "CampText.h"
#include "ListaUtilizatori.h"
#include "ModificareParola.h"

using namespace std;

void main_menu();
void login_studenti_succes();
void login_admin_succes();
void check_resize(void* pParams);
void listeaza_carti();
void listeaza_carti_stud();
void listeaza_carti_imprumutate();
void afisare_carte(Carte carte);
void afisare_carte_stud(Carte carte);
void adauga_carte();
void editare_carte(Carte carte);
void afisare_utilizator(Utilizator utilizator);
void afisare_utilizator_stud(Utilizator utilizator);
void editare_utilizator(Utilizator utilizator);
void listeaza_utilizatori();
void adauga_user();
void modificare_parola_admin();
void modificare_parola_stud();

static bool APP_RUNNING;

int main()
{
    APP_RUNNING = TRUE;
    // adresa de start, dimensiunea stivei,pointer la lista de argumente
    _beginthread(check_resize, 0, NULL);  // vezi http://www.codeproject.com/Articles/438/Introduction-to-Multi-threaded-Code
    main_menu();
    APP_RUNNING = FALSE;
    return 0;
}

void main_menu_callback(int index) {
    switch (index) {
        case 0:{
                EcranLogin login("Autentificare studenti", "studenti", &login_studenti_succes, &main_menu);
                login.afiseaza();
        }
        break;

        case 1:{
                EcranLogin login("Autentificare administrator", "admin", &login_admin_succes, &main_menu);
                login.afiseaza();
        }
        break;

        case 2: {
            system("cls");
            exit(0);
        }
        break;


    }
}

void admin_menu_callback(int index) {
    switch (index) {
        case 0:
            listeaza_carti();
            break;

        case 1:
            adauga_carte();
            break;

        case 2:
            listeaza_utilizatori();
            break;

        case 3:
            adauga_user();
            break;

        case 4:
            modificare_parola_admin();
            break;

        case 5:
            main_menu();
            break;
    }
}

void student_menu_callback(int index) {
    switch (index) {
        case 0:
            listeaza_carti_stud();
        break;

        case 1:
            listeaza_carti_imprumutate();
        break;

        case 2:
            afisare_utilizator_stud(EcranLogin::current_user);
        break;

        case 3:
            modificare_parola_stud();
        break;

        case 4:
            main_menu();
        break;
    }
}

void listeaza_carti() {
    ListaCarti lista_carti("Lista carti", toate, &afisare_carte, &editare_carte, &login_admin_succes);
    lista_carti.afiseaza();
}

void listeaza_carti_stud() {
    ListaCarti lista_carti("Lista carti disponibile pt imprumutat", disponibile, &afisare_carte_stud, NULL, &login_studenti_succes);
    lista_carti.afiseaza();
}

void listeaza_carti_imprumutate() {
    ListaCarti lista_carti("Lista carti imprumutate", imprumutate, &afisare_carte_stud, NULL, &login_studenti_succes);
    lista_carti.afiseaza();
}

void adauga_carte() {
    vector<CampText> campuri;
    CampText titlu_carte("Titlu: ", 30, "titlu");
    CampText autor_carte("Autor: ", 30, "autor");
    CampText editura("Editura: ", 15, "editura");
    CampText nr_pagini("Numar pagini: ", 3, "pagini");
    nr_pagini.tip = numeric;
    CampText an_aparitie("Anul publicarii: ", 4, "an");
    an_aparitie.tip = numeric;
    CampText isbn("ISBN: ", 20, "isbn");
    campuri.push_back(titlu_carte);
    campuri.push_back(autor_carte);
    campuri.push_back(editura);
    campuri.push_back(nr_pagini);
    campuri.push_back(an_aparitie);
    campuri.push_back(isbn);
    AdaugaCarte adauga_carte("Adaugare carte", campuri, &listeaza_carti, &login_admin_succes);
    adauga_carte.afiseaza();
}

void listeaza_utilizatori() {
    ListaUtilizatori lista_utilizatori("Lista utilizatori", &afisare_utilizator, &editare_utilizator, &login_admin_succes);
    lista_utilizatori.afiseaza();
}

void adauga_user() {
    vector<CampText> campuri;
    CampText username("Username: ", 20, "username");
    CampText nume("Nume: ", 30, "nume");
    CampText prenume("Prenume: ", 30, "prenume");
    CampText parola("Parola: ", 15, "password");
    campuri.push_back(username);
    campuri.push_back(nume);
    campuri.push_back(prenume);
    campuri.push_back(parola);
    AdaugaUser adauga_user("Adaugare utilizator", campuri, &listeaza_utilizatori, &login_admin_succes);
    adauga_user.afiseaza();
}

void afisare_carte(Carte carte) {
    DetaliiCarte detalii_carte(carte, &listeaza_carti);
    detalii_carte.afiseaza();
}

void afisare_carte_stud(Carte carte) {
    DetaliiCarte detalii_carte(carte, &listeaza_carti_stud);
    detalii_carte.afiseaza();
}

void editare_carte(Carte carte) {
    vector<CampText> campuri;
    CampText titlu_carte("Titlu: ", 30, "titlu");
    titlu_carte.valoare = carte.titlu;
    CampText autor_carte("Autor: ", 30, "autor");
    autor_carte.valoare = carte.autor;
    CampText editura("Editura: ", 15, "editura");
    editura.valoare = carte.editura;
    CampText nr_pagini("Numar pagini: ", 3, "pagini");
    nr_pagini.tip = numeric;
    nr_pagini.valoare = carte.nr_pag;
    CampText an_aparitie("Anul publicarii: ", 4, "an");
    an_aparitie.tip = numeric;
    an_aparitie.valoare = carte.an;
    CampText isbn("ISBN: ", 20, "isbn");
    isbn.valoare = carte.isbn;
    campuri.push_back(titlu_carte);
    campuri.push_back(autor_carte);
    campuri.push_back(editura);
    campuri.push_back(nr_pagini);
    campuri.push_back(an_aparitie);
    campuri.push_back(isbn);

    AdaugaCarte editor_carte(carte, "Editare carte", campuri, &listeaza_carti, &listeaza_carti);
    editor_carte.afiseaza();
}

void afisare_utilizator(Utilizator utilizator) {
    DetaliiUtilizator detalii_utilizator(utilizator, &listeaza_utilizatori);
    detalii_utilizator.afiseaza();
}

void afisare_utilizator_stud(Utilizator utilizator) {
    DetaliiUtilizator detalii_utilizator(utilizator, &login_studenti_succes);
    detalii_utilizator.afiseaza();
}

void editare_utilizator(Utilizator utilizator) {
    vector<CampText> campuri;
    CampText username("Username: ", 20, "username");
    username.valoare = utilizator.username;
    CampText nume("Nume: ", 30, "nume");
    nume.valoare = utilizator.nume;
    CampText prenume("Prenume: ", 30, "prenume");
    prenume.valoare = utilizator.prenume;
    CampText parola("Parola: ", 15, "password");
    parola.valoare = utilizator.password;
    campuri.push_back(username);
    campuri.push_back(nume);
    campuri.push_back(prenume);
    campuri.push_back(parola);
    AdaugaUser adauga_user(utilizator, "Editare utilizator", campuri, &listeaza_utilizatori, &listeaza_utilizatori);
    adauga_user.afiseaza();
}

void modificare_parola_admin() {
    ModificareParola modificare_parola = ModificareParola("Modifica parola", "admin", EcranLogin::current_user.id, &login_admin_succes);
    modificare_parola.afiseaza();
}

void modificare_parola_stud() {
    ModificareParola modificare_parola = ModificareParola("Modifica parola", "studenti", EcranLogin::current_user.id, &login_studenti_succes);
    modificare_parola.afiseaza();
}

void main_menu() {
    vector<string> optiuni_meniu;
    optiuni_meniu.push_back("Acces studenti");
    optiuni_meniu.push_back("Acces administrator");
    optiuni_meniu.push_back("Iesire");
    Meniu meniu_principal("BIBLIOTECA", optiuni_meniu, &main_menu_callback);
    meniu_principal.afiseaza();
}

void login_studenti_succes() {
    vector<string> optiuni_meniu;
    optiuni_meniu.push_back("Carti disponibile");
    optiuni_meniu.push_back("Carti imprumutate");
    optiuni_meniu.push_back("Detalii personale");
    optiuni_meniu.push_back("Modificare parola");
    optiuni_meniu.push_back("Deautentificare");
    Meniu meniu_studenti("Meniu studenti", optiuni_meniu, &student_menu_callback);
    meniu_studenti.afiseaza();
}

void login_admin_succes() {
    vector<string> optiuni_meniu;
    optiuni_meniu.push_back("Lista carti");
    optiuni_meniu.push_back("Adauga carte");
    optiuni_meniu.push_back("Lista utilizatori");
    optiuni_meniu.push_back("Adauga utilizator");
    optiuni_meniu.push_back("Modificare parola");
    optiuni_meniu.push_back("Deautentificare");
    Meniu meniu_studenti("Meniu administrator", optiuni_meniu, &admin_menu_callback);
    meniu_studenti.afiseaza();
}

// https://github.com/rprichard/winpty/blob/master/misc/VkEscapeTest.cc
void check_resize(void* pParams) {
    int c = get_console_size().columns;
    int r = get_console_size().rows;

    while (APP_RUNNING) {
        if (c != get_console_size().columns || r != get_console_size().rows) {
            c = get_console_size().columns;
            r = get_console_size().rows;

            // "Scrie" tasta 255 in consola pentru a declansa getch-urile din meniuri
            INPUT_RECORD ir; // Describes an input event in the console input buffer ; este o structura
            DWORD dwTmp; // Double Word
            // DWORD in the other hand, specifies its own size, which is Double Word. Word are 16 bits so DWORD will be known as 32 bit across all platform ( rather than integers)
            ir.EventType = KEY_EVENT;  // Describes a keyboard input event in a console INPUT_RECORD structure.
            ir.Event.KeyEvent.bKeyDown = TRUE; // If the key is pressed, this member is TRUE. Otherwise, this member is FALSE (the key is released).
            ir.Event.KeyEvent.dwControlKeyState = 0; // The state of the control keys. This member can be one or more of the following values.
            ir.Event.KeyEvent.uChar.AsciiChar = (char)255;
            ir.Event.KeyEvent.wRepeatCount = 1;  // The repeat count, which indicates that a key is being held down.
            // Writes data directly to the console input buffer.
            // A handle to the console input buffer,
            // A pointer to an array of INPUT_RECORD structures that contain data to be written to the input buffer
            // The number of input records to be written.
            // A pointer to a variable that receives the number of input records actually written
            WriteConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &ir, 1, &dwTmp);
        }

        Sleep(100); // Suspend execution for an interval of time
    }
}
 // Pentru callbackuri, functiile au o semnatura, formata din tipul returnat si parametri pe care ii primeste
