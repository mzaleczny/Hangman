#include <iostream>
#include <string>
#include <vector>
#include "PhrasesFile.h"
#include "TerminalScreen.h"
#include "Drawings.h"
using namespace std;

std::vector<std::string> Phrases;
TTerminalScreen Screen;

bool Initialize();
void PrintGameState();

int main()
{
    if (!Initialize())
    {
        cerr << "ERROR: Initialization error." << endl;
        return -1;
    }

    PrintGameState();

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

void PrintGameState()
{
    Screen.DrawClippedMultilineText(5, 2, ScaffoldRoot);
    Screen.DrawClippedMultilineText(5, 2, ScaffoldHorizontal);
    Screen.DrawClippedMultilineText(32, 2, ScaffoldRope);
    Screen.DrawClippedMultilineText(30, 5, HangmanHead);
    Screen.DrawClippedMultilineText(31, 8, HangmanTorso);
    Screen.DrawClippedMultilineText(24, 8, HangmanLeftHand);
    Screen.DrawClippedMultilineText(33, 8, HangmanRightHand);
    Screen.DrawClippedMultilineText(26, 15, HangmanLeftLeg);
    Screen.DrawClippedMultilineText(34, 15, HangmanRightLeg);
    Screen.PresentScreen();
}