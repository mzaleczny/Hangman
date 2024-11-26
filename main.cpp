#include <iostream>
#include <string>
#include <vector>
#include "PhrasesFile.h"
#include "TerminalScreen.h"
#include "Drawings.h"
using namespace std;

enum class EHangmanState
{
    EHS_None,
    EHS_Head,
    EHS_Torso,
    EHS_LeftHand,
    EHS_RightHand,
    EHS_LeftLeg,
    EHS_RightLeg,
    EHS_Dead,
    EHS_Number
};

EHangmanState HangmanState = EHangmanState::EHS_None;
std::vector<std::string> Phrases;
std::string CurrentPhrase;
std::string CurrentPhraseHint;
std::string UserEnteredPhrase;
TTerminalScreen Screen;
bool DoGameLoop;

bool Initialize();
void PrintGameState();
void GetInput();
bool Update();
void Render();
void Deinitialize();
void GetRandomPhraseAndRestartGame();

int main()
{
    
    if (!(DoGameLoop = Initialize()))
    {
        cerr << "ERROR: Initialization error." << endl;
        return -1;
    }

    while (DoGameLoop) // Game Loop
    {
        GetInput();
        Update();
        Render();
        DoGameLoop = false;
    }

    Deinitialize();


    return 0;
}


bool Initialize()
{
    {
        PhrasesFile pf;
        pf.ReadPhrases(Phrases);
    }

    if (Phrases.size() > 0)
    {
        GetRandomPhraseAndRestartGame();
        return true;
    }

    return false;
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

void GetInput()
{
}

bool Update()
{
    return false;
}

void Render()
{
    Screen.Clear();

    PrintGameState();

    Screen.DrawText(50, 4, "Guess the phrase,  Hint: " + CurrentPhraseHint);
    Screen.DrawText(52, 5, "Your phrase: [ " + UserEnteredPhrase + " ]");
    Screen.DrawText(52, 6, "Choose letter: ");
    Screen.PresentScreen();
}

void Deinitialize()
{
}

void GetRandomPhraseAndRestartGame()
{
    int Index = rand() % Phrases.size();
    size_t Pos;
    if ((Pos = Phrases[Index].find('|')) != string::npos)
    {
        CurrentPhrase = Phrases[Index].substr(0, Pos);
        CurrentPhraseHint = Phrases[Index].substr(Pos + 1);
        UserEnteredPhrase = std::string(CurrentPhrase.length(), '_');
    }
}
