#include "cab202_graphics.h"
#include "cab202_timers.h"

#include "pong.h"

int main() {
  setup_screen();
  
  while (running) {
    process();
    show_screen();
    timer_pause(DELAY);
  }

  cleanup_screen();
  return 0;
}

void process() {


}
