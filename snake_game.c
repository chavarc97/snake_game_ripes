#include "ripes_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// DEFINES
//============================================================
#define SW0 (0x01)             // switch 0
#define GREEN 0x00FF00         // green color for fruit
#define BLACK 0x000000         // black color for background
#define WHITE 0xFFFFFF         // white color for border
#define RED 0xFF0000           // RED color for snake
#define INITIAL_SNAKE_LENGTH 4 // initial length of the snake
#define IS_ALIVE 1             // snake is alive
#define IS_DEAD 0              // snake is dead
//============================================================

// GLOBAL VARIABLES TO PERIPHERALS
//============================================================
volatile unsigned int *sw_base = SWITCHES_0_BASE;
volatile unsigned int *led_base = LED_MATRIX_0_BASE;
volatile unsigned int *pad_up = D_PAD_0_UP;
volatile unsigned int *pad_down = D_PAD_0_DOWN;
volatile unsigned int *pad_right = D_PAD_0_RIGHT;
volatile unsigned int *pad_left = D_PAD_0_LEFT;
//============================================================

// CONSTANTS
//============================================================
const int led_width = LED_MATRIX_0_WIDTH;
const int led_height = LED_MATRIX_0_HEIGHT;
const int led_size = LED_MATRIX_0_SIZE;
//============================================================

// OFFSET MOVEMENT
//============================================================
const int right_offset = 1;        // right offset
const int left_offset = -1;        // left offset
const int up_offset = -led_width;  // up offset
const int down_offset = led_width; // down offset
//============================================================

// TIME SIMULATION
int rand_seed = 0; // random seed

// SNAKE STRUCTURE
int tail_idx;                                         // tail index
int *snake[LED_MATRIX_0_WIDTH * LED_MATRIX_0_HEIGHT][LED_MATRIX_0_WIDTH * LED_MATRIX_0_HEIGHT]; // snake array

// GLOBAL STATE
int game_state; // game state

// FUNCTION DECLARATIONS
//============================================================
void innit_snake();
void move_snake(int direction);
void spawn_fruit();
void print_border();
//============================================================

void main()
{
    for (int i = 0; i < 10000; i++)
        ;           // create a delay
    print_border(); // print the border
    innit_snake();  // innit the snake
    spawn_fruit();  // spawn the fruit
    game_state = IS_ALIVE;
    int direction = right_offset;
    while (game_state)
    {
        for (int i = 0; i < 100000; i++)
            ; // delay
        int prev_dir = direction;
        if (*pad_up)
            direction = up_offset;
        if (*pad_down)
            direction = down_offset;
        if (*pad_right)
            direction = right_offset;
        if (*pad_left)
            direction = left_offset;
        // if the direction is opposite to the previous direction
        if (direction + prev_dir == 0)
            direction = prev_dir;
        move_snake(direction);
    }
}

void innit_snake()
{
    // SET THE HEAD OF THE SNAKE
    int *head = led_base + ((led_width) * (led_height >> 1) + (led_width >> 1));
    snake[0][0] = head; // set the head of the snake
    // snake should be 4 x 2 leds
    int i = 0;
    for (i = 0; i < INITIAL_SNAKE_LENGTH; i++)
    {
        for (int dx = 0; dx < 4; dx++)
        {
            for (int dy = 0; dy < 2; dy++)
            {
                int *led = head + dx + dy * led_width;
                *led = RED;
                snake[i] = led;
            }
        }
    }
}

void spawn_fruit()
{
    // generate a random number
    int valid_position = 0;
    rand_seed += 5;
    while (!valid_position)
    {
        srand(rand_seed);
        int rand_x = rand() % (led_width - 2);
        int rand_y = rand() % (led_height - 2);
        int *rand_pos = led_base + (rand_y * led_width + rand_x);
        if (*rand_pos != RED)
        {
            valid_position = 1;
            /* *rand_pos = GREEN; */
            for (int i = 0; i < 2; i++)
            {
                for (int j = 0; j < 2; j++)
                {
                    *(rand_pos + i + j * led_width) = GREEN;
                }
            }
        }
    }
}

void move_snake(int direction)
{
   int *old_part = snake[0][0];
   snake[0][0] += direction;
   int *head_ptr = snake[0][0];

    // if snake hits the border or itself then it is dead
    if(head_ptr == RED || head_ptr == WHITE){
        game_state = IS_DEAD;
        return;
    }

    // if snake eats the fruit
    if(head_ptr == GREEN)
    {
        tail_idx++;
        snake[tail_idx][tail_idx] = snake[tail_idx - 1][tail_idx - 1] + 1;
        int *tail = snake[tail_idx][tail_idx];
        *tail = RED;
        spawn_fruit();
    }

    // move the snake
    for(int i = 1; i <= tail_idx; i++)
    {
        for(int j = 0; j < 2; j++)
        {
            if(i == tail_idx)
            {
                int *tail = snake[i][j];
                *tail = BLACK;
            }
            int curr_snake = snake[i][j];
            snake[i][j] = old_part;
            old_part = curr_snake;
        }
    }
    // fill the array with the new snake
    for(int i = 0; i < tail_idx; i++)
    {
        for(int j = 0; j < 2; j++)
        int *ptr = snake[i][j];
        *ptr = RED;
    }
}

void print_border()
{
    for (int i = 0; i < led_width; i++)
    {
        led_base[i] = WHITE;
        led_base[i + (led_height - 1) * led_width] = WHITE;
    }
    for (int i = 0; i < led_height; i++)
    {
        led_base[i * led_width] = WHITE;
        led_base[i * led_width + led_width - 1] = WHITE;
    }
}