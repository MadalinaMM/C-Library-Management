#include <vector>
#include <string>
#include <map>
#include "utils.h"
#include "Carte.h"

#ifndef LISTACARTI_H
#define LISTACARTI_H

using namespace std;

enum filtru_carti
{
    toate,
    disponibile,
    imprumutate
};

class ListaCarti
{
public:
    ListaCarti(string titlu, filtru_carti filtrare, void (*selection)(Carte carte), void (*editare)(Carte carte), void (*cancel)());
    void afiseaza();
    virtual ~ListaCarti();
protected:
private:
    int current_index;
    string titlu;
    string filtru;
    filtru_carti filtrare;
    void (*selection)(Carte carte);
    void (*cancel)();
    void (*editare)(Carte carte);
    map<int,Carte> indexuri; // Maps are associative containers that store elements formed by a combination of a key value and a mapped value, following a specific order
    void highlight_index(int index); // HIGHTLIGHT cand se ajunge la optiunea respectiva
    void clear_selection(int index);
    int row_for_index(int index); // Returneaza randul curent in functie de index
    void sterge_carte(int index);
    void editeaza_carte(int index);
    void imprumuta_carte(int index);
    void returneaza_carte(int index);
    void afiseaza_cautare();  // Cautarea cartii din josul paginii
    void ascunde_cautare();
};

#endif // LISTACARTI_H
