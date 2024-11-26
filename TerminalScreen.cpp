#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include "TerminalScreen.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TTerminalScreen::ClearRect(int Left, int Top, int Right, int Bottom, char BgChar)
{
    int Length = Right - Left + 1;
    for (int y = Top; y <= Bottom; y++)
    {
        Content.replace(y * Width + Left, Length, Length, BgChar);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TTerminalScreen::DrawClippedMultilineText(int Column, int Row, std::string Text)
{
    if (Column >= Width) return;
    int WidthRemained = Width - Column;
    size_t StartPos = 0;
    size_t EndLinePos;
    size_t Len = Text.length();
    size_t LineLen;
    int Line = 0;
    while (StartPos < Len)
    {
        EndLinePos = Text.find('\n', StartPos);
        if (EndLinePos == std::string::npos)
        {
            LineLen = Len - StartPos;
        }
        else
        {
            LineLen = EndLinePos - StartPos;
        }
        DrawText(Column, Row + Line, Text.substr(StartPos, (LineLen <= WidthRemained) ? LineLen : WidthRemained));
        StartPos += LineLen + 1;
        Line++;
    }
}
