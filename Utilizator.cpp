#include "Utilizator.h"

// Initializam id-ul cu 0
Utilizator::Utilizator()
{
    id = 0;
}

Utilizator::Utilizator(int id, string username, string password, string nume, string prenume)
{
    this->id = id;
    this->username = username;
    this->password = password;
    this->nume = nume;
    this->prenume = prenume;
}

Utilizator::~Utilizator()
{
    //dtor
}
