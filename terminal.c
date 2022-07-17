#include "terminal.h"

// System headers
#include <windows.h>
#include <conio.h>

// Standard library C-style
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define ESC "\x1b"
#define CSI "\x1b["
#define OSC "\x1b]"

static COORD s_Size;

bool EnableVTMode()
{
    // Set output mode to handle virtual terminal sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
    {
        return false;
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode))
    {
        return false;
    }

    /*
    CONSOLE_FONT_INFOEX font_info = {0};
    COORD font_size = {16,16};

	font_info.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	font_info.dwFontSize = font_size;
	font_info.FontWeight = FW_NORMAL;
	font_info.FontFamily = FF_DONTCARE;

	if (!SetCurrentConsoleFontEx(hOut, false, &font_info))
    {
        return false;
    }
    */

    return true;
}

void SetTitle(const char *a_title)
{
    printf(OSC "0;%s\x07", a_title);
}

void PrintChar(int a_x, int a_y, char a_sym, bool a_line_set, const char *a_colors)
{
    if (a_x >= 0 && a_x <= s_Size.X &&
        a_y >= 0 && a_y <= s_Size.Y)
    {
        if (a_colors != NULL)
        {
            printf(CSI "%sm", a_colors); // set color
        }
        if (a_line_set)
        {
            printf(ESC "(0"); // Enter Line drawing mode
        }
        printf(CSI "%d;%dH%c", a_y+2, a_x+1, a_sym);
        if (a_line_set)
        {
            printf(ESC "(B"); // exit line drawing mode
        }
        if (a_colors != NULL)
        {
            printf(CSI "0m"); // restore color
        }
    }
}


void PrintVerticalBorder()
{
    printf(ESC "(0"); // Enter Line drawing mode
    printf(CSI "104;93m"); // bright yellow on bright blue
    printf("x"); // in line drawing mode, \x78 -> \u2502 "Vertical Bar"
    printf(CSI "0m"); // restore color
    printf(ESC "(B"); // exit line drawing mode
}

void PrintHorizontalBorder(COORD const Size, bool fIsTop)
{
    printf(ESC "(0"); // Enter Line drawing mode
    printf(CSI "104;93m"); // Make the border bright yellow on bright blue
    printf(fIsTop ? "l" : "m"); // print left corner 

    for (int i = 1; i < Size.X - 1; i++)
        printf("q"); // in line drawing mode, \x71 -> \u2500 "HORIZONTAL SCAN LINE-5"

    printf(fIsTop ? "k" : "j"); // print right corner
    printf(CSI "0m");
    printf(ESC "(B"); // exit line drawing mode
}

void Terminal_PrintTitle(const char *a_line)
{
    printf(CSI "1;1H" CSI "102;30m");
    printf(a_line);
    printf(CSI "0m");
}

void Terminal_PrintStatus(const char *a_line)
{
    printf(CSI "%d;1H", s_Size.Y);
    printf(CSI "K"); // clear the line
    printf(a_line);
}

int Terminal_Setup(void)
{
    //First, enable VT mode
    bool fSuccess = EnableVTMode();
    if (!fSuccess)
    {
        printf("Unable to enter VT processing mode. Quitting.\n");
        return -1;
    }
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        printf("Couldn't get the console handle. Quitting.\n");
        return -1;
    }

    CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
    GetConsoleScreenBufferInfo(hOut, &ScreenBufferInfo);

    // Determine the size of the window
    s_Size.X = ScreenBufferInfo.srWindow.Right - ScreenBufferInfo.srWindow.Left + 1;
    s_Size.Y = ScreenBufferInfo.srWindow.Bottom - ScreenBufferInfo.srWindow.Top + 1;

    // Enter the alternate buffer
    printf(CSI "?1049h");

    // Clear screen
    printf(CSI "1;1H");
    printf(CSI "2J"); // Clear screen

    SetTitle("Game");

    // Hide the cursor
    printf(CSI "?25l");

    // Set scrolling margins to 3, h-2
    printf(CSI "3;%dr", s_Size.Y - 2);

    // Print a top border - Yellow
    printf(CSI "2;1H");
    PrintHorizontalBorder(s_Size, true);

    for (int i = 3; i < s_Size.Y - 1; i++)
    {
        printf(CSI "%d;1H", i);
        PrintVerticalBorder();
        printf(CSI "%d;%dH", i, s_Size.X);
        PrintVerticalBorder();
    }

    printf(CSI "%d;1H", s_Size.Y-1);
    PrintHorizontalBorder(s_Size, false);

    return 0;
}

void Terminal_Cleanup(void)
{
    // Exit the alternate buffer
    printf(CSI "?1049l");
    printf("\n\n");
}

int Terminal_BoardWidth()
{
    // Console is from 1 to X, inclusive.
    // Border is at 1 and X
    // Thus, internal board width is (X - 2)
    return s_Size.X - 2;
}

int Terminal_BoardHeight()
{
    // Console is from 1 to Y, inclusive.
    // Line 1 is heading
    // Line 2 is top border
    // Line Y-1 is bottom border
    // Line Y is status
    // Thus, internal board width is (Y - 4)
    return s_Size.Y - 4;
}

void Terminal_BoardDraw(int a_x, int a_y, char a_sym, bool a_line_set, const char *a_colors)
{
    if (a_x > 0 && a_x <= Terminal_BoardWidth() &&
        a_y > 0 && a_y <= Terminal_BoardHeight())
    {
        PrintChar(a_x, a_y, a_sym, a_line_set, a_colors);
    }
}

void Terminal_Sleep(int a_millisec)
{
    Sleep(a_millisec);
}

int Terminal_GetInput(bool a_wait_for_input)
{
    int ch = 0;

    if (kbhit() || a_wait_for_input)
    {
        ch = getch();
        if (ch == 224)
        {
            ch = (ch << 8) | getch();
        }
    }

    return ch;
}
