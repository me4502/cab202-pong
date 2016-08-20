#pragma once
#include "cab202_timers.h"
#include "cab202_sprites.h"

// A few utilities due to C lacking certain functions in the standard library
#define PI 3.14159265358979323846
#define MIN(a,b) a < b ? a : b

// Program management variables
#define FPS 60
#define DELAY MILLISECONDS / FPS

bool running = true;
bool display_help_screen = true;

// Level management variables.
int level = 0;
int score = 0;
int lives = 10;
int game_time = 0;
const int LEVEL_COUNT = 4;

timer_id *cooldown_timer;
int cooldown_number;

sprite_id *ball_sprite;
double ball_starting_velocity = 0.3;

// Paddle variables
int left_paddle_offset;
int right_paddle_offset;
int paddle_size;

// Help Screen
const char * help_screen_text[] = {
        "Pong",
        "Created by Madeline Miller - n9342401",
        "",
        "Controls",
        "L for next level - H for help",
        "",
        "Press any key to exit"

};

void setup();

void cleanup();

void process();

void spawn_ball();

void start_level(int new_level);

int string_length(char * str);