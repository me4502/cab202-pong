#pragma once

// Program management variables
const int FPS = 60;
const int DELAY = 1000 / FPS;
bool running = true;
bool display_help_screen = true;

// Level management variables.
int level = 0;
int score = 0;
int lives = 10;
int game_time = 0;
const int LEVEL_COUNT = 4;

// Help Screen
const char * help_screen_text[] = {
        "Pong",
        "Created by Madeline Miller - n9342401",
        "",
        "Controls",
        "L for next level",
        "H for help",
        "",
        "Press any key to exit"

};

void process();

void start_level(int new_level);

int string_length(char * str);