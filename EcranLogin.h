#include <string>
#include "Utilizator.h"
#include "utils.h"

#ifndef ECRANLOGIN_H
#define ECRANLOGIN_H

using namespace std;

class EcranLogin
{
public:
    EcranLogin(string titlu, string dbname, void (*succes)(), void (*cancel)());
    void afiseaza();
    static Utilizator current_user;
    virtual ~EcranLogin();
protected:
private:
    string titlu;
    string dbname;
    void (*succes)();
    void (*cancel)();
    void introducere_user();
    void introducere_pass();
    bool verifica_auth();
    void afiseaza_eroare(string eroare);
  //  int verifica_auth_cb(void *data, int argc, char **argv, char **azColName);
};

#endif // ECRANLOGIN_H
