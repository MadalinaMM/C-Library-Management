#include <string>

#ifndef CARTE_H
#define CARTE_H

using namespace std;

class Carte
{
public:
    Carte();
    virtual ~Carte();
    int id;
    string titlu;
    string autor;
    string editura;
    string nr_pag;
    string an;
    string isbn;
    int imprumutat;
protected:
private:
};

#endif // CARTE_H
