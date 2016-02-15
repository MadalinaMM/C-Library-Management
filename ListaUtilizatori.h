#include <iostream>
#include <map>
#include <string>
#include "utils.h"
#include "Utilizator.h"

#ifndef LISTAUTILIZATORI_H
#define LISTAUTILIZATORI_H

using namespace std;

class ListaUtilizatori
{
public:
    ListaUtilizatori(string titlu, void (*selection)(Utilizator utilizator), void (*editare)(Utilizator utilizator), void (*cancel)());
    void afiseaza();
    virtual ~ListaUtilizatori();
protected:
private:
    int current_index;
    string titlu;
    map<int,Utilizator> indexuri; //Maps are associative containers that store elements formed by a combination of a key value and a mapped value, following a specific order
    void (*selection)(Utilizator utilizator);
    void (*editare)(Utilizator utilizator);
    void (*cancel)();
    void highlight_index(int index);
    void clear_selection(int index);
    int row_for_index(int index);
    void sterge_utilizator(int index);
    void editeaza_utilizator(int index);
};

#endif // LISTAUTILIZATORI_H
