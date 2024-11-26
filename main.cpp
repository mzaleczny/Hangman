#include <iostream>
#include <string>
#include <vector>
#include "PhrasesFile.h"
#include "TerminalScreen.h"
#include "Drawings.h"
using namespace std;

enum class EGameState
{
    EHS_Start,
    EHS_Head,
    EHS_Torso,
    EHS_LeftHand,
    EHS_RightHand,
    EHS_LeftLeg,
    EHS_RightLeg,
    EHS_Dead,
    EHS_TopTen,
    EHS_Number
};

EGameState HangmanState = EGameState::EHS_Start;
std::vector<std::string> Phrases;
std::string CurrentPhrase;
std::string CurrentPhraseHint;
std::string UserEnteredPhrase;
std::string UserInput;
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
}

void GetInput()
{
    HANDLE hConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD Buffer;
    DWORD NumberOfEventsRead = 0;
    GetNumberOfConsoleInputEvents(hConsoleInput, &NumberOfEventsRead);
    if (NumberOfEventsRead > 0)
    {
        if (ReadConsoleInput(hConsoleInput, &Buffer, 1, &NumberOfEventsRead))
        {
            if (Buffer.EventType == KEY_EVENT && Buffer.Event.KeyEvent.bKeyDown)
            {
                char ch = Buffer.Event.KeyEvent.uChar.AsciiChar;
                if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == ' ')
                {
                    UserInput = ch;
                }
                else if (Buffer.Event.KeyEvent.wVirtualKeyCode == 27)
                {
                    UserInput = "";
                    DoGameLoop = false;
                }
            }
        }
    }
}

bool Update()
{
    if (UserInput != "")
    {
        char ch = std::tolower(UserInput[0]);
        for (size_t i = 0; i < CurrentPhrase.length(); ++i)
        {
            if (std::tolower(CurrentPhrase[i]) == ch)
            {
                UserEnteredPhrase[i] = ch;
            }
        }
        UserInput = "";
    }
    
    return false;
}

void Render()
{
    Screen.Clear();

    PrintGameState();

    Screen.DrawText(50, 4, "Guess the phrase,  Hint: " + CurrentPhraseHint);
    Screen.DrawText(52, 4, "Your phrase: [ " + CurrentPhrase + " ]");
    Screen.DrawText(52, 5, "Your phrase: [ " + UserEnteredPhrase + " ]");
    Screen.DrawText(52, 6, "Choose letter: ");
    Screen.DrawText(52, 8, "Press ESC to break and quit the game");
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
