#include <string>
#include <iostream>>

#ifndef MODIFICAREPAROLA_H
#define MODIFICAREPAROLA_H

using namespace std;

class ModificareParola
{
public:
    ModificareParola(string titlu, string dbname, int user_id, void (*cancel)());
    void afiseaza();
    virtual ~ModificareParola();
protected:
private:
    string titlu;
    string dbname;
    int user_id;
    void (*cancel)();
    string val_password;
    string titlu_camp;
    int max_field_length;
    void introducere_pass();
    bool salveaza_parola();
    void afiseaza_eroare(string sql, string eroare);
};

#endif // MODIFICAREPAROLA_H
