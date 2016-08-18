#include "cab202_graphics.h"
#include "cab202_timers.h"
#include "cab202_sprites.h"

#include "pong.h"

int main() {
    setup_screen();

    while (running) {
        clear_screen();
        process();
        show_screen();
        timer_pause(DELAY);
    }

    cleanup_screen();
    return 0;
}

void process() {
    int pressed_char = get_char();

    game_time ++;

    // Draw help screen
    if (display_help_screen) {
        for (int i = 0; i < sizeof(help_screen_text) / sizeof(help_screen_text[0]); i++) {
            const char * help_text = help_screen_text[i];

            int x = screen_width()/2 - string_length((char *) help_text) / 2;

            draw_formatted(x, i, help_text);
        }

        if (pressed_char > 0) {
            display_help_screen = false;
        }
        return;
    }

    // Draw the main game screen

    // Draw the border
    draw_line(0, 0, screen_width()-1, 0, '*');
    draw_line(0, 0, 0, screen_height()-1, '*');
    draw_line(0, screen_height()-1, screen_width()-1, screen_height()-1, '*');
    draw_line(screen_width()-1, 0, screen_width()-1, screen_height()-1, '*');

    // Draw the info area.
    draw_line(0, 2, screen_width()-1, 2, '*');

    // Get the third mark, excluding the border
    int third = screen_width()/3 - 6;
    draw_formatted(2, 1, "Lives: %d", lives);
    draw_formatted(2 + third, 1, "* Score: %d", score);
    draw_formatted(2 + third*2, 1, "* Level: %d", level + 1);

    int modified_time = game_time / FPS;
    int seconds = modified_time % 60;
    int minutes = (modified_time - seconds) / 60;

    draw_formatted(2 + third*3, 1, "* Time: %02d:%02d", minutes, seconds);

    if (pressed_char == 'l') {
        int new_level = level + 1;
        if (new_level >= LEVEL_COUNT) {
            new_level = 0;
        }

        start_level(new_level);
    } else if (pressed_char == 'h') {
        display_help_screen = true;
    }
}

void start_level(int new_level) {
    level = new_level;

    score = 0;
    lives = 10;
    game_time = 0;
}

int string_length(char * str) {
    int length = 0;

    // Iterate until it's null terminated.
    while (str[length] != '\0') {
        length++;
    }

    return length;
}