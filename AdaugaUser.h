#include <string>
#include <vector>
#include "utils.h"
#include "CampText.h"
#include "Utilizator.h"
#ifndef ADAUGAUSER_H
#define ADAUGAUSER_H

using namespace std;

class AdaugaUser
{
public:
    AdaugaUser(string titlu, vector<CampText> campuri, void (*succes)(), void (*cancel)());
    AdaugaUser(Utilizator edit_user, string titlu, vector<CampText> campuri, void (*succes)(), void (*cancel)());
    void afiseaza();
    virtual ~AdaugaUser();
protected:
private:
    string titlu;
    vector<CampText> campuri;
    void (*succes)();
    void (*cancel)();
    void activeaza_camp(CampText camp);
    string valoare_camp(string camp_id);
    bool salveaza();
    void afiseaza_eroare(string eroare);
    void afiseaza_erori(vector<string> erori);
    int current_index;
    Utilizator edit_user;
};

#endif // ADAUGAUSER_H
