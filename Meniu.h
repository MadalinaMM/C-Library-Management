#include <vector>
#include <string>
#include "utils.h"

#ifndef MENIU_H
#define MENIU_H

using namespace std;

class Meniu
{
public:
    Meniu(string titlu, vector<string> optiuni, void (*cb)(int));
    void afiseaza();
    virtual ~Meniu();
protected:
private:
    int current_index;
    string titlu;
    vector<string> optiuni;
    void (*callback)(int); // Prin callbackA Callback function is a method that is passed into a routine, and called at some point by the routine to which it is passed
    void highlight_index(int index);
    void clear_selection(int index);
    int row_for_index(int index);
};

#endif // MENIU_H
