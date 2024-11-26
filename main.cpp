#include <iostream>
#include <string>
#include <vector>
#include "PhrasesFile.h"
using namespace std;

std::vector<std::string> Phrases;

bool Initialize();

int main()
{
    if (!Initialize())
    {
        cerr << "ERROR: Initialization error." << endl;
        return -1;
    }

    for (size_t i = 0; i < Phrases.size(); ++i)
    {
        cout << Phrases[i] << endl;
    }

    return 0;
}


bool Initialize()
{
    {
        PhrasesFile pf;
        pf.ReadPhrases(Phrases);
    }
    return Phrases.size() > 0;
}