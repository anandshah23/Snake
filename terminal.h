#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include <stdbool.h>

#define TERMINAL_KEY_UP ((224 << 8) + 72)
#define TERMINAL_KEY_DOWN ((224 << 8) + 80)
#define TERMINAL_KEY_LEFT ((224 << 8) + 75)
#define TERMINAL_KEY_RIGHT ((224 << 8) + 77)

int Terminal_Setup(void);
void Terminal_Cleanup(void);

void Terminal_PrintTitle(const char *a_line);
void Terminal_PrintStatus(const char *a_line);

int Terminal_BoardWidth();
int Terminal_BoardHeight();
void Terminal_BoardDraw(int a_x, int a_y, char a_sym, bool a_line_set, const char *a_colors);

void Terminal_Sleep(int a_millisec);

int Terminal_GetInput(bool a_wait_for_input);

#endif // _TERMINAL_H_
