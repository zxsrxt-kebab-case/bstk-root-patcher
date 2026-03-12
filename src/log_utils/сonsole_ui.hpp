//
// Created by zxsrxt on 10.03.2026.
//

#ifndef BSTK_ROOT_PATCHER_СONSOLE_UI_HPP
#define BSTK_ROOT_PATCHER_СONSOLE_UI_HPP

#include <windows.h>
#include <iostream>
#include <string>

//vibecoded

enum class Color : WORD
{
    White = 7,
    Green = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    Red = FOREGROUND_RED | FOREGROUND_INTENSITY,
    Cyan = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    Yellow = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    Gray = 8
};

class console_ui
{
    HANDLE h_out;
public:
    console_ui() : h_out(GetStdHandle(STD_OUTPUT_HANDLE)) {}

    void set_color(Color color)
    {
        SetConsoleTextAttribute(h_out, static_cast<WORD>(color));
    }

    void reset_color()
    {
        set_color(Color::White);
    }

    void set_cursor(short x, short y)
    {
        SetConsoleCursorPosition(h_out, { x, y });
    }

    COORD get_cursor_pos()
    {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(h_out, &csbi);
        return csbi.dwCursorPosition;
    }

    void clear_line(short y)
    {
        set_cursor(0, y);
        std::cout << std::string(80, ' ') << "\r";
    }

    void draw_progress(float progress)
    {
        int width = 40;
        int pos = static_cast<int>(width * progress);

        std::cout << "\r[";
        set_color(Color::Cyan);
        for (int i = 0; i < width; ++i)
        {
            if (i < pos) std::cout << "■";
            else if (i == pos) std::cout << "■";
            else std::cout << " ";
        }
        reset_color();
        std::cout << "] " << (int)(progress * 100.0f) << "% " << std::flush;
    }
};

#endif //BSTK_ROOT_PATCHER_СONSOLE_UI_HPP