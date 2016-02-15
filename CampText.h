#include <string>

#ifndef CAMPTEXT_H
#define CAMPTEXT_H

using namespace std;

enum tip_camp_text
{
    standard,
    numeric,
    parola
};

class CampText
{
public:
    CampText(string titlu, int lungime, string id);
    string titlu;
    int lungime;
    string id;
    string valoare;
    tip_camp_text tip;
    virtual ~CampText();
protected:
private:
};

#endif // CAMPTEXT_H
