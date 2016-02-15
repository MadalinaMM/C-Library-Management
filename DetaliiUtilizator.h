#include <iostream>
#include "utils.h"
#include "Utilizator.h"

#ifndef DETALIIUTILIZATOR_H
#define DETALIIUTILIZATOR_H

using namespace std;

class DetaliiUtilizator
{
public:
    DetaliiUtilizator(Utilizator utilizator, void (*cancel)());
    void afiseaza();
    virtual ~DetaliiUtilizator();
protected:
private:
    Utilizator utilizator;
    void (*cancel)(); // Pointer catre tasta ESCAPE
};

#endif // DETALIIUTILIZATOR_H
