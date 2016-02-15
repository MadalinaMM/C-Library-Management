#include "utils.h"
#include "Carte.h"

#ifndef DETALIICARTE_H
#define DETALIICARTE_H

using namespace std;

class DetaliiCarte
{
public:
    DetaliiCarte(Carte carte, void (*cancel)());
    void afiseaza();
    virtual ~DetaliiCarte();
protected:
private:
    Carte carte;
    void (*cancel)(); // Cancel catre ESCAPE
};

#endif // DETALIICARTE_H
