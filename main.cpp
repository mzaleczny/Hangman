#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include "PhrasesFile.h"
#include "TerminalScreen.h"
#include "Drawings.h"
#include "Score.h"
using namespace std;

enum class EGameState
{
    EGS_Start,
    EGS_ScaffoldRoot,
    EGS_ScaffoldHorizontal,
    EGS_ScaffoldRope,
    EGS_Head,
    EGS_Torso,
    EGS_LeftHand,
    EGS_RightHand,
    EGS_LeftLeg,
    EGS_RightLeg,
    EGS_GameOver,
    EGS_TopTen,
    EGS_Number
};

EGameState GameState = EGameState::EGS_Start;
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
uint64_t GuessingTimeElapsed, CurTimePlus15m;

bool Initialize();
void PrintGameState();
void GetInput();
void Update(uint64_t LastFrameDuration);
void Render();
void Deinitialize();
void GetRandomPhraseAndRestartGame();
void GameOver();
void TopTen();

#define CUR_TIME_IN_MILLISEC std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()
int main()
{
    
    if (!(DoGameLoop = Initialize()))
    {
        cerr << "ERROR: Initialization error." << endl;
        return -1;
    }

    uint64_t LastFrameTime = 0;
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
    if (GameState >= EGameState::EGS_ScaffoldRoot)
        Screen.DrawClippedMultilineText(5, 2, ScaffoldRoot);
    if (GameState >= EGameState::EGS_ScaffoldHorizontal)
        Screen.DrawClippedMultilineText(5, 2, ScaffoldHorizontal);
    if (GameState >= EGameState::EGS_ScaffoldRope)
        Screen.DrawClippedMultilineText(32, 2, ScaffoldRope);
    if (GameState >= EGameState::EGS_Head)
        Screen.DrawClippedMultilineText(30, 5, HangmanHead);
    if (GameState >= EGameState::EGS_Torso)
        Screen.DrawClippedMultilineText(31, 8, HangmanTorso);
    if (GameState >= EGameState::EGS_LeftHand)
        Screen.DrawClippedMultilineText(24, 8, HangmanLeftHand);
    if (GameState >= EGameState::EGS_RightHand)
        Screen.DrawClippedMultilineText(33, 8, HangmanRightHand);
    if (GameState >= EGameState::EGS_LeftLeg)
        Screen.DrawClippedMultilineText(26, 15, HangmanLeftLeg);
    if (GameState >= EGameState::EGS_RightLeg)
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

void Update(uint64_t LastFrameDuration)
{
    static uint64_t TimePeriod = 0;
    TimePeriod += LastFrameDuration;
    GuessingTimeElapsed += LastFrameDuration;

    if (GameState > EGameState::EGS_RightLeg)
    {
        return;
    }

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
                // if the latter was hit previously, then mark miss
                if (UserEnteredPhrase[i] != ch)
                {
                    Hit = true;
                    UserEnteredPhrase[i] = ch;
                }
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
            if (GameState >= EGameState::EGS_RightLeg)
            {
                GameState = EGameState::EGS_GameOver;
            }
        }
        else
        {
            string LowerPhrase = CurrentPhrase;
            transform(CurrentPhrase.begin(), CurrentPhrase.end(), LowerPhrase.begin(), ::tolower);
            if (UserEnteredPhrase == LowerPhrase)
            {
                GameState = EGameState::EGS_GameOver;
            }
            GuessResultMessage = "Letter hit!";
            TimePeriod = 0;
        }
    }
    
    return;
}

void Render()
{
    if (GameState == EGameState::EGS_GameOver)
    {
        GameOver();
        return;
    }
    else if (GameState == EGameState::EGS_TopTen)
    {
        TopTen();
        return;
    }

//    Screen.Clear();
    PrintGameState();

    Screen.DrawText(40, 1, "Total attempts: " + to_string(TotalAttempts) + "  GoodAttempts: " + to_string(TotalAttempts - WrongAttempts) + "  WrongAttempts: " + to_string(WrongAttempts) + ",  Time elapsed: " + to_string(GuessingTimeElapsed));
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
    GuessingTimeElapsed = 0;
    CurTimePlus15m = 15 * 60 * 1000; // max period in miliseconds 15minutes
    int Index = rand() % Phrases.size();
    size_t Pos;
    if ((Pos = Phrases[Index].find('|')) != string::npos)
    {
        CurrentPhrase = Phrases[Index].substr(0, Pos);
        CurrentPhraseHint = Phrases[Index].substr(Pos + 1);
        UserEnteredPhrase = std::string(CurrentPhrase.length(), '_');
    }
}

void GameOver()
{
    string LowerPhrase = CurrentPhrase;
    string Input;
    transform(CurrentPhrase.begin(), CurrentPhrase.end(), LowerPhrase.begin(), ::tolower);

    Screen.Clear();
    Screen.PresentScreen();

    if (UserEnteredPhrase == LowerPhrase)
    {
        int Points = (9 - WrongAttempts) * 100 * 1000; // max is 900000 that counterparts to max time of 15 min. Each wrong answers costs 10 secondes (as counterpart)
        Points += CurrentPhrase.length() * 1000; // make points depend on phrase length too
        // max allowed time is 15min (in milliseconds), lesser time elapsed - it is better
        Points += CurTimePlus15m - GuessingTimeElapsed; // count of miliseconds that remained
        if (Points < 0)
        {
            Points = 0;
        }
        Points /= 100; // scale down number of points to not to be too large

        cout << "Good job. You guessed phrase correctly. You gained " << Points << " points." << endl << endl;

        TScore score;
        if (score.GetInsertionPosition(Points) >= 0)
        {
            cout << "You got to the top ten list. Type in Your name: ";
            getline(cin, Input);
            score.Insert(Points, Input);
        }
        else
        {
            cout << "Unfortunately You didn't get to the top ten list." << endl;
        }
        cout << endl << "Press Enter to continue." << endl;
        getline(cin, Input);
    }
    else
    {
        cout << "Failure. You missed guessing the phrase. Press Enter to continue." << endl;
        getline(cin, Input);
    }
    GameState = EGameState::EGS_TopTen;
}

void TopTen()
{
    string Input;
    TScore score;

    Screen.Clear();
    Screen.PresentScreen();

    cout << endl << "Top Ten List:" << endl << endl;
    cout << score.GetScoreTableDataAsString() << endl;
    cout << endl << "Press Enter to continue." << endl;
    getline(cin, Input);
    GameState = EGameState::EGS_Start;
    GetRandomPhraseAndRestartGame();
}
