#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include "PhrasesFile.h"
#include "TerminalScreen.h"
#include "Drawings.h"
using namespace std;

enum class EGameState
{
    EHS_Start,
    EHS_ScaffoldRoot,
    EHS_ScaffoldHorizontal,
    EHS_ScaffoldRope,
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

EGameState GameState = EGameState::EHS_Start;
std::vector<std::string> Phrases;
std::string CurrentPhrase;
std::string CurrentPhraseHint;
std::string UserEnteredPhrase;
std::string UserInput;
std::string GuessResultMessage;
TTerminalScreen Screen;
bool DoGameLoop;
int WrongAttempts;
int TotalAttempts;

bool Initialize();
void PrintGameState();
void GetInput();
bool Update(uint64_t LastFrameDuration);
void Render();
void Deinitialize();
void GetRandomPhraseAndRestartGame();

#define CUR_TIME_IN_MILLISEC std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()
int main()
{
    
    if (!(DoGameLoop = Initialize()))
    {
        cerr << "ERROR: Initialization error." << endl;
        return -1;
    }

    uint64_t LastFrameTime = CUR_TIME_IN_MILLISEC;
    uint64_t FrameStartTime;
    while (DoGameLoop) // Game Loop
    {
        FrameStartTime = CUR_TIME_IN_MILLISEC;
        GetInput();
        Update(LastFrameTime);
        Render();
        LastFrameTime = CUR_TIME_IN_MILLISEC - FrameStartTime;
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
    if (GameState >= EGameState::EHS_ScaffoldRoot)
        Screen.DrawClippedMultilineText(5, 2, ScaffoldRoot);
    if (GameState >= EGameState::EHS_ScaffoldHorizontal)
        Screen.DrawClippedMultilineText(5, 2, ScaffoldHorizontal);
    if (GameState >= EGameState::EHS_ScaffoldRope)
        Screen.DrawClippedMultilineText(32, 2, ScaffoldRope);
    if (GameState >= EGameState::EHS_Head)
        Screen.DrawClippedMultilineText(30, 5, HangmanHead);
    if (GameState >= EGameState::EHS_Torso)
        Screen.DrawClippedMultilineText(31, 8, HangmanTorso);
    if (GameState >= EGameState::EHS_LeftHand)
        Screen.DrawClippedMultilineText(24, 8, HangmanLeftHand);
    if (GameState >= EGameState::EHS_RightHand)
        Screen.DrawClippedMultilineText(33, 8, HangmanRightHand);
    if (GameState >= EGameState::EHS_LeftLeg)
        Screen.DrawClippedMultilineText(26, 15, HangmanLeftLeg);
    if (GameState >= EGameState::EHS_RightLeg)
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

bool Update(uint64_t LastFrameDuration)
{
    static uint64_t TimePeriod = 0;
    TimePeriod += LastFrameDuration;

    if (TimePeriod > 1200)
    {
        GuessResultMessage = "";
    }

    if (UserInput != "")
    {
        char ch = std::tolower(UserInput[0]);
        bool Hit = false;
        for (size_t i = 0; i < CurrentPhrase.length(); ++i)
        {
            if (std::tolower(CurrentPhrase[i]) == ch)
            {
                Hit = true;
                UserEnteredPhrase[i] = ch;
            }
        }
        UserInput = "";
        ++TotalAttempts;
        if (!Hit)
        {
            ++WrongAttempts;
            GameState = (EGameState)((int)GameState + 1);
            GuessResultMessage = "No such letter!";
            TimePeriod = 0;
        }
        else
        {
            string LowerPhrase;
            transform(CurrentPhrase.begin(), CurrentPhrase.end(), LowerPhrase.begin(), ::tolower);
            if (UserEnteredPhrase == LowerPhrase)
            {
                DoGameLoop = false;
            }
            GuessResultMessage = "Letter hit!";
            TimePeriod = 0;
        }
    }
    
    return false;
}

void Render()
{
    Screen.Clear();

    PrintGameState();

    Screen.DrawText(50, 1, "Total attempts: " + to_string(TotalAttempts) + "  GoodAttempts: " + to_string(TotalAttempts - WrongAttempts) + "  WrongAttempts: " + to_string(WrongAttempts));
    Screen.DrawText(50, 3, GuessResultMessage);
    Screen.DrawText(50, 5, "Guess the phrase,  Hint: " + CurrentPhraseHint);
    Screen.DrawText(52, 7, "Your phrase: [ " + UserEnteredPhrase + " ]");
    Screen.DrawText(52, 9, "Type in a letter or press ESC to break and quit the game");
    Screen.PresentScreen();
}

void Deinitialize()
{
}

void GetRandomPhraseAndRestartGame()
{
    WrongAttempts = 0;
    TotalAttempts = 0;
    int Index = rand() % Phrases.size();
    size_t Pos;
    if ((Pos = Phrases[Index].find('|')) != string::npos)
    {
        CurrentPhrase = Phrases[Index].substr(0, Pos);
        CurrentPhraseHint = Phrases[Index].substr(Pos + 1);
        UserEnteredPhrase = std::string(CurrentPhrase.length(), '_');
    }
}
