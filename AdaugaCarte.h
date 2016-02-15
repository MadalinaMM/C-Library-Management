#include <string>
#include <vector>
#include "utils.h"
#include "CampText.h"
#include "Carte.h"

#ifndef ADAUGACARTE_H
#define ADAUGACARTE_H

class AdaugaCarte
{
public:
    AdaugaCarte(string titlu, vector<CampText> campuri, void (*succes)(), void (*cancel)());
    AdaugaCarte(Carte edit_carte, string titlu, vector<CampText> campuri, void (*succes)(), void (*cancel)());
    void afiseaza();
    virtual ~AdaugaCarte();
protected:
private:
    string titlu;
    vector<CampText> campuri;
    void (*succes)();
    void (*cancel)();
    void activeaza_camp(CampText camp);
    string valoare_camp(string camp_id);
    bool salveaza();
    void afiseaza_eroare(string sql, string eroare);
    void afiseaza_erori(vector<string> erori);
    int current_index;
    Carte edit_carte;
};

#endif // ADAUGACARTE_H
