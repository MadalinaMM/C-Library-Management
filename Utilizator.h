#include <string>
#ifndef UTILIZATOR_H
#define UTILIZATOR_H

using namespace std;

class Utilizator
{
public:
    Utilizator();
    Utilizator(int id, string username, string password, string nume, string prenume);
    virtual ~Utilizator();
    int id;
    string username;
    string password;
    string nume;
    string prenume;
protected:
private:
};

#endif // UTILIZATOR_H
