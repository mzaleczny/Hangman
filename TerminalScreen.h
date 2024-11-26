#pragma once
#include <Windows.h>

class TTerminalScreen
{
    friend class TPixels;
public:
    std::string Content;
    char BgChar;
    int Width;
    int Height;
public:
    TTerminalScreen(int W = 120, int H = 30) : BgChar(' '), Width(W), Height(H)
    {
        Content.resize(Height * Width, BgChar);
        Clear();
    }

    void Resize(int W, int H)
    {
        Width = W;
        Height = H;
        Content.resize(Height * Width, BgChar);
        Clear();
    }

    //--------------------
    void ClearRect(int Left, int Top, int Right, int Bottom, char BgChar);

    //--------------------
    void PresentScreen()
    {
        COORD coord;
        coord.X = 0;
        coord.Y = 0;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
        std::cout << Content << std::flush;
    }

    //--------------------
    void Clear()
    {
        ClearRect(0, 0, Width - 1, Height - 1, BgChar);
    }

    //-------------------
    void DrawText(int Column, int Row, std::string Text)
    {
        Content.replace(Width * Row + Column, Text.length(), Text);
    }

    //-------------------
    void DrawClippedMultilineText(int Column, int Row, std::string Text);

    //-------------------
    char GetCharAt(int Column, int Row)
    {
        if (Column < 0 || Column >= Width) return -1;
        if (Row < 0 || Row >= Height) return -1;
        int i = Width * Row + Column;
        if (i >= 0 && i < Content.length())
            return Content[i];
        return -1;
    }

    //-------------------
    void PutCharAt(int Column, int Row, char ch)
    {
        if (Column < 0 || Column >= Width - 1) return;
        if (Row < 0 || Row >= Height) return;
        int i = Width * Row + Column;
        if (i >= 0 && i < Content.length())
            Content[i] = ch;
    }
};
