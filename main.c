// Standard library C-style
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "terminal.h"

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction_t;

static int s_snake_x[200];
static int s_snake_y[200];
static int s_snake_length;
static Direction_t s_direction;
static int s_fruit_x;
static int s_fruit_y;
static int s_score;

bool IsASnake(int a_x, int a_y)
{
    bool is_a_snake = false;
    int i;
    for (i=0; i < s_snake_length; i++)
    {
        if (s_snake_x[i] == a_x && s_snake_y[i] == a_y)
        {
            is_a_snake = true;
        }
    }

    return is_a_snake;
}

bool IsAFruit(int a_x, int a_y)
{
    return s_fruit_x == a_x && s_fruit_y == a_y;    
}

void NewFruit()
{
    s_fruit_x = 0;
    s_fruit_y = 0;
    while (s_fruit_x == 0 || IsASnake(s_fruit_x, s_fruit_y))
    {
        s_fruit_x = (rand() % Terminal_BoardWidth()) + 1;
        s_fruit_y = (rand() % Terminal_BoardHeight()) + 1;
    }
    Terminal_BoardDraw(s_fruit_x, s_fruit_y, '*', false, "91");
}

void NewSnakeHead(int a_x, int a_y)
{
    // Move everything down by 1
    int i;
    for (i=0; i < s_snake_length; i++)
    {
        s_snake_x[s_snake_length-i] = s_snake_x[s_snake_length-i-1];
        s_snake_y[s_snake_length-i] = s_snake_y[s_snake_length-i-1];
    }

    s_snake_x[0] = a_x;
    s_snake_y[0] = a_y;
    Terminal_BoardDraw(s_snake_x[0], s_snake_y[0], '`', true, "32");
}

int main()
{
    char status_line[128];
    bool game_over = false;
    int ch;

    if (Terminal_Setup() < 0)
    {
        return -1;
    }

    Terminal_PrintTitle("Snake Game");

    Terminal_PrintStatus("Press any key to begin");
    Terminal_GetInput(true);

    s_snake_x[0] = Terminal_BoardWidth() / 2;
    s_snake_y[0] = Terminal_BoardHeight() / 2;
    s_snake_x[1] = s_snake_x[0];
    s_snake_y[1] = s_snake_y[0] + 1;
    s_snake_x[2] = s_snake_x[1];
    s_snake_y[2] = s_snake_y[1] + 1;
    s_snake_length = 3;
    s_direction = UP;
    s_score = 0;

    Terminal_BoardDraw(s_snake_x[0], s_snake_y[0], '`', true, "32");
    Terminal_BoardDraw(s_snake_x[1], s_snake_y[1], '`', true, "32");
    Terminal_BoardDraw(s_snake_x[2], s_snake_y[2], '`', true, "32");
    NewFruit();

    while (!game_over)
    {
        int head_x = s_snake_x[0];
        int head_y = s_snake_y[0];

        Terminal_Sleep(75);

        ch = Terminal_GetInput(false);

        if (ch == TERMINAL_KEY_UP && s_direction != DOWN)
        {
            s_direction = UP;
        }
        else if (ch == TERMINAL_KEY_DOWN && s_direction != UP)
        {
            s_direction = DOWN;
        }
        else if (ch == TERMINAL_KEY_RIGHT && s_direction != LEFT)
        {
            s_direction = RIGHT;
        }
        else if (ch == TERMINAL_KEY_LEFT && s_direction != RIGHT)
        {
            s_direction = LEFT;
        }
        else if (ch == 'q')
        {
            game_over = true;
        }

        switch (s_direction)
        {
            case UP:
                head_y--;
                game_over = head_y == 0;
                break;
            case DOWN:
                head_y++;
                game_over = head_y > Terminal_BoardHeight();
                break;
            case LEFT:
                head_x--;
                game_over = head_x == 0;
                break;
            case RIGHT:
                head_x++;
                game_over = head_x > Terminal_BoardWidth();
                break;
        }

        // Check if we hit something
        if (IsASnake(head_x,head_y))
        {
            game_over = true;
        }
        else
        {
            // Add the head
            NewSnakeHead(head_x, head_y);

            if (IsAFruit(head_x, head_y))
            {
                s_score++;
                s_snake_length++;
                NewFruit();
            }
            else
            {
                Terminal_BoardDraw(s_snake_x[s_snake_length], s_snake_y[s_snake_length], ' ', false, NULL);
                s_snake_x[s_snake_length] = 0;
                s_snake_y[s_snake_length] = 0;
            }
            sprintf(status_line, "Score: %d", s_score);
            Terminal_PrintStatus(status_line);
        }
    }

    sprintf(status_line, "Score: %d -- GAME OVER! -- press 'q' ", s_score);
    Terminal_PrintStatus(status_line);
    while (ch != 'q')
    {
        ch = Terminal_GetInput(true);
    }
}
