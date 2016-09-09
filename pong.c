#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "cab202_graphics.h"
#include "cab202_timers.h"
#include "cab202_sprites.h"

#include "pong.h"

int main() {
    setup_screen();

    setup();

    while (running) {
        clear_screen();
        process();
        show_screen();
        timer_pause(DELAY);
    }

    cleanup();

    cleanup_screen();
    return 0;
}

void setup() {
    srand((unsigned int) time(NULL));

    paddle_size = MIN(7, (screen_height() - 2 - 1) / 2);
    left_paddle_offset = screen_height()/2 - paddle_size/2;
    right_paddle_offset = screen_height()/2 - paddle_size/2;
}

void cleanup() {
    if (cooldown_timer) {
        destroy_timer(*cooldown_timer);
    }
    if (ball_sprite) {
        sprite_destroy(*ball_sprite);
    }
}

void process() {
    int pressed_char = get_char();

    // Draw the gameover screen
    if (gameover) {
        for (int i = 0; i < sizeof(game_over_screen_text) / sizeof(game_over_screen_text[0]); i++) {
            const char * text = game_over_screen_text[i];

            int x = screen_width()/2 - string_length((char *) text) / 2;

            draw_formatted(x, i, text);
        }

        if (pressed_char == 'y') {
            reset_game();
        } else if (pressed_char == 'n') {
            running = false;
        }
        return;
    }

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

    // Draw left paddle.
    for (int i = 0; i < paddle_size; i++) {
        draw_char(3, left_paddle_offset + i, '|');
    }

    // Draw right paddle.
    if (level != 0) {
        for (int i = 0; i < paddle_size; i++) {
            draw_char(screen_width() - 4, right_paddle_offset + i, '|');
        }
    }

    // Draw the main game screen
    switch (level) {
        case 0:
            break;
        case 1:
            break;
        case 2:
            if (anomoly_visible) {
                sprite_draw(*anomoly);
            }
            break;
        case 3:
            for (int x = 0; x < rail_width-1; x++) {
                if (top_rail[x] == '=') {
                    draw_char(rail_xoffset + x, screen_height() / 3, top_rail[x]);
                }
                if (bottom_rail[x] == '=') {
                    draw_char(rail_xoffset + x, screen_height() - (screen_height() / 3), bottom_rail[x]);
                }
            }
            break;
        default:
            start_level(0);
            break;
    }

    // Draw the main ball sprite after the level has been drawn.
    if (ball_sprite) {
        sprite_draw(*ball_sprite);
    }

    if (cooldown_timer) {
        char *text = "Starting in... %d";
        int x = screen_width()/2 - string_length(text) / 2;

        draw_formatted(x, screen_height()/2, text, cooldown_number);

        if (timer_expired(*cooldown_timer)) {
            cooldown_number --;
            if (cooldown_number < 0) {
                destroy_timer(*cooldown_timer);
                cooldown_timer = NULL;
                if (!ball_sprite) {
                    ball_sprite = malloc(sizeof(sprite_id));
                    *ball_sprite = sprite_create(screen_width()/2, screen_height()/2, 1, 1, "*");
                } else {
                    sprite_move_to(*ball_sprite, screen_width()/2, screen_height()/2);
                }

                double degrees = ((rand() % 91) - 90 - 45);
                double radians = (degrees * PI / 180.0);

                double dx = sin(radians) * ball_starting_velocity;
                double dy = cos(radians) * ball_starting_velocity;

                sprite_turn_to(*ball_sprite, dx, dy);
            } else {
                timer_reset(*cooldown_timer);
            }
        }
        return;
    }

    int sx = ball_sprite ? (int) round(sprite_x(*ball_sprite)) : 0;
    int sy = ball_sprite ? (int) round(sprite_y(*ball_sprite)) : 0;

    if (!ball_sprite || sx < 0 || sx > screen_width()) {
        if (ball_sprite && sy < screen_width() / 2) {
            lives -= 1;
            if (lives < 0) {
                game_over();
            }
        }
        spawn_ball();
        return;
    }

    game_time ++;

    // Bounce when the ball hits the top or bottom.
    if (sy < 3 || sy > screen_height()-2) {
        double dx = sprite_dx(*ball_sprite);
        double dy = -sprite_dy(*ball_sprite);

        sprite_back(*ball_sprite);
        sprite_turn_to(*ball_sprite, dx, dy);
    }

    // Bounce when the ball hits the left paddle
    if (sx == 3 && sy >= left_paddle_offset && sy < left_paddle_offset + paddle_size) {
        double dy = sprite_dy(*ball_sprite);
        double dx = sprite_dx(*ball_sprite);

        if ((sy == left_paddle_offset && dy > 0) || (sy == left_paddle_offset + paddle_size - 1 && dy < 0)) {
            dy = -dy;
        } else {
            dx = -dx;
        }

        sprite_back(*ball_sprite);
        sprite_turn_to(*ball_sprite, dx, dy);

        score ++;
    }

    if (level != 0) {
        // Bounce when the ball hits the right paddle
        if (sx == screen_width() - 4 && sy >= right_paddle_offset && sy < right_paddle_offset + paddle_size) {
            double dy = sprite_dy(*ball_sprite);
            double dx = sprite_dx(*ball_sprite);

            if ((sy == right_paddle_offset && dy > 0) || (sy == right_paddle_offset + paddle_size - 1 && dy < 0)) {
                dy = -dy;
            } else {
                dx = -dx;
            }

            sprite_back(*ball_sprite);
            sprite_turn_to(*ball_sprite, dx, dy);
        }

        right_paddle_offset = sy - paddle_size/2;
        if (right_paddle_offset < 3) {
            right_paddle_offset = 3;
        } else if (right_paddle_offset > screen_height() - paddle_size - 1) {
            right_paddle_offset = screen_height() - paddle_size - 1;
        }
    }

    // Update the main game screen
    switch (level) {
        case 0:
            if (sx >= screen_width() - 2) {
                double dx = -sprite_dx(*ball_sprite);
                double dy = sprite_dy(*ball_sprite);

                sprite_back(*ball_sprite);
                sprite_turn_to(*ball_sprite, dx, dy);
            }
            break;
        case 1:
            break;
        case 2:
            if (!anomoly_visible && timer_expired(*anomoly_timer)) {
                anomoly_visible = true;
            }

            if (anomoly_visible) {
                int x_diff = (int)round(sprite_x(*anomoly)) + anomoly_x_offset - sx - 1;
                int y_diff = (int)round(sprite_y(*anomoly)) + anomoly_y_offset - sy;

                double dist_squared = pow(x_diff, 2) + pow(y_diff, 2);

                if (dist_squared < pow(1, -10)) {
                    dist_squared = pow(1, -10);
                }

                double dist = sqrt(dist_squared);
                if (dist > 2 && dist < 8) {
                    double dx = sprite_dx(*ball_sprite);
                    double dy = sprite_dy(*ball_sprite);

                    double magnitude = (1 / dist_squared) * 0.7;

                    dx += (magnitude * x_diff / dist);
                    dy += (magnitude * y_diff / dist);

                    double normal_dist = sqrt(pow(dx, 2) + pow(dy, 2));

                    if (normal_dist > 1) {
                        dx /= normal_dist;
                        dy /= normal_dist;
                    }

                    sprite_turn_to(*ball_sprite, dx, dy);
                }
            }

            break;
        case 3:
            if (sy == screen_height() / 3 || sy == (screen_height() - screen_height() / 3)) {
                if (sx >= rail_xoffset && sx < rail_xoffset + rail_width - 1) {
                    bool hit = false;
                    bool is_top = sy == screen_height() / 3;
                    if (is_top) {
                        if (top_rail[sx - rail_xoffset] == '=') {
                            top_rail[sx - rail_xoffset] = ' ';
                            if (sx - rail_xoffset > 0)
                                top_rail[sx - rail_xoffset - 1] = ' ';
                            if (sx - rail_xoffset < rail_width)
                                top_rail[sx - rail_xoffset + 1] = ' ';
                            hit = true;
                        }
                    } else {
                        if (bottom_rail[sx - rail_xoffset] == '=') {
                            bottom_rail[sx - rail_xoffset] = ' ';
                            if (sx - rail_xoffset > 0)
                                bottom_rail[sx - rail_xoffset - 1] = ' ';
                            if (sx - rail_xoffset < rail_width)
                                bottom_rail[sx - rail_xoffset + 1] = ' ';
                            hit = true;
                        }
                    }
                    if (hit) {
                        double dx = sprite_dx(*ball_sprite);
                        double dy = sprite_dy(*ball_sprite);

                        if (sx == rail_xoffset || sx == rail_xoffset + rail_width - 2
                            //|| (is_top && (top_rail[sx - rail_xoffset - (int)(dx)] == ' '))
                            //|| (!is_top && (bottom_rail[sx - rail_xoffset - (int)(dx)] == ' '))
                            ) {
                            dx = -dx;
                        } else {
                            dy = -dy;
                        }

                        sprite_back(*ball_sprite);
                        sprite_turn_to(*ball_sprite, dx, dy);
                    }
                }
            }
            break;
        default:
            start_level(0);
            break;
    }

    // Normalize the ball velocity.
    double dx = sprite_dx(*ball_sprite);
    double dy = sprite_dy(*ball_sprite);

    double dist = sqrt(pow(dx, 2) + pow(dy, 2));
    if (dist > 1) {
        dx /= dist;
        dy /= dist;

        sprite_turn_to(*ball_sprite, dx, dy);
    }


    sprite_step(*ball_sprite);

    if (pressed_char == 'l') {
        int new_level = level + 1;
        if (new_level >= LEVEL_COUNT) {
            new_level = 0;
        }

        start_level(new_level);
    } else if (pressed_char == 'h') {
        display_help_screen = true;
    } else if (pressed_char == 'w') {
        left_paddle_offset --;
        if (left_paddle_offset < 3)
            left_paddle_offset = 3;
    } else if (pressed_char == 's') {
        left_paddle_offset ++;
        if (left_paddle_offset > screen_height() - paddle_size - 1)
            left_paddle_offset = screen_height() - paddle_size - 1;
    }
}

void spawn_ball() {
    cooldown_timer = malloc(sizeof(timer_id));
    *cooldown_timer = create_timer(300);
    cooldown_number = 3;

    if (level == 2) {
        if (!anomoly_timer) {
            anomoly_timer = malloc(sizeof(timer_id));
            *anomoly_timer = create_timer(5000);
        } else {
            timer_reset(*anomoly_timer);
        }
        anomoly_visible = false;
    }
}

void start_level(int new_level) {
    level = new_level;

    spawn_ball();

    switch (new_level) {
        case 0:
            break;
        case 1:
            break;
        case 2:
            if (!anomoly) {
                anomoly = malloc(sizeof(sprite_id));
                *anomoly = sprite_create(screen_width() / 2 - anomoly_x_offset, screen_height() / 2 - anomoly_y_offset, 5, 5, anomoly_image);
            }

            if (!anomoly_timer) {
                anomoly_timer = malloc(sizeof(timer_id));
                *anomoly_timer = create_timer(5000);
            } else {
                timer_reset(*anomoly_timer);
            }

            anomoly_visible = false;

            break;
        case 3:
            rail_xoffset = screen_width() / 4;
            rail_width = screen_width() / 2;
            top_rail = malloc(rail_width * sizeof(*top_rail));
            bottom_rail = malloc(rail_width * sizeof(*bottom_rail));

            for (int i = 0; i < rail_width; i++) {
                top_rail[i] = '=';
                bottom_rail[i] = '=';
            }
            break;
        default:
            start_level(0);
            break;
    }
}

void reset_game() {
    lives = 3;
    score = 0;
    game_time = 0;

    gameover = false;

    spawn_ball();
}

void game_over() {
    gameover = true;
}

int string_length(char * str) {
    int length = 0;

    // Iterate until it's null terminated.
    while (str[length] != '\0') {
        length++;
    }

    return length;
}