#include <asm-generic/ioctls.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/ioctl.h>
#include <unistd.h>

// input
#include <chrono>
#include <fcntl.h>
#include <termios.h>
#include <thread>

#include <ncurses.h>

#include "game.cpp"
#include "shop.cpp"
// #include "graphics.hpp"

const int WIDTH = 80;
const int HEIGHT = 20;

const char *FINISHED = "Field finished! Press SPACE to continue.";
const char *FUEL = "Ran out of fuel! Use the shop to refuel.";
const char *REMINDER = "Tip: Use TAB to open the shop.";

game g = game(WIDTH, HEIGHT);
void drawRect(int x, int y, int w, int h, char c, bool grass) {
  for (int i = 0; i < h; i++) {
    for (int k = 0; k < w; k++) {
      if (grass) {
        if (g.f.get(i, k) == 0) {
          mvaddch(k + x, i + y, '#');
        }
      } else {
          mvaddch(k + x, i + y, c);
      }
    }
  }
}

int odd(int n) { return (n - 1) / 2; }

bool debug = 0;

int main(int argc, char **argv) {
  // setNonBlockingInput();
  // atexit(exiting);
  initscr();
  if (has_colors() == false) {
    endwin();
    printf("yo god damn terminal does not support colors");
    exit(1);
  }
  start_color();
  raw();
  noecho();
  cbreak();
  timeout(0);
  keypad(stdscr, true);
  curs_set(0);

  init_pair(1, COLOR_BLACK, COLOR_GREEN);
  init_pair(2, COLOR_WHITE, COLOR_RED);

  window shop = window();
  shop.createWin(5, 5, WIDTH - 10, HEIGHT - 8, &g);

  // clear();
  while (true) {
    int awidth, aheight;
    getmaxyx(stdscr, awidth, aheight);

    erase();
    int ch = getch();
    switch (ch) {
    case 'q':
    case KEY_F(2):
      goto exit;
      break;

    case KEY_UP:
      if (g.fuel > 0 && !shop.toggled) {
        g.playerY -= 1;
        g.deFuel();
      } else {
        shop.triggerMessage(true);
      }
      break;
    case KEY_DOWN:
      if (g.fuel > 0 && !shop.toggled) {
        g.playerY += 1;
        g.deFuel();
      } else {
        shop.triggerMessage(true);
      }
      break;
    case KEY_LEFT:
      if (g.fuel > 0 && !shop.toggled) {
        g.playerX -= 1;
        g.deFuel();
      } else {
        shop.triggerMessage(true);
      }
      break;
    case KEY_RIGHT:
      if (g.fuel > 0 && !shop.toggled) {
        g.playerX += 1;
        g.deFuel();
      } else {
        shop.triggerMessage(true);
      }
      break;

    case 'w':
      debug = 1;
      break;
    case 'r':
      g.money += 100000;
      break;
    case 't':
      g.lvl += 10;
      break;
    case 'y':
      g.fields += 10;
      break;

    case ' ':
      if (g.f.completed()) {
        g.f.clearField();
        g.fields += 1;
      }
      break;
    case 9:
      shop.toggled = !shop.toggled;
      shop.triggerMessage(false);
      break;

    // shop
    case 49:
      if (shop.toggled) {
        shop.process(1);
      }
      break;
    case 50:
      if (shop.toggled) {
        shop.process(2);
      }
      break;
    case 51:
      if (shop.toggled) {
        shop.process(3);
      }
      break;
    }

    if (g.playerX < 0) {
      g.playerX = 0;
      g.deFuel(-1);
    }
    if (g.playerX > WIDTH - 1) {
      g.playerX = WIDTH - 1;
      g.deFuel(-1);
    }
    if (g.playerY < 1) {
      g.playerY = 1;
      g.deFuel(-1);
    }
    if (g.playerY > HEIGHT) {
      g.playerY = HEIGHT;
      g.deFuel(-1);
    }
    g.cut(g.playerX, g.playerY - 1, g.rad);

    if (aheight < WIDTH || awidth < HEIGHT + 1) {
      endwin();
      printf("get yo small ass window outta here\n");
      printf("h: %d < %d; w: %d < %d\n", aheight, WIDTH, awidth, HEIGHT + 1);
      exit(1);
    }

    attron(COLOR_PAIR(1));
    drawRect(1, 0, HEIGHT, WIDTH, '#', true);
    attroff(COLOR_PAIR(1));

    mvprintw(0, 0, "money: %d", g.money);
    mvprintw(0, 16, "fuel: %d", g.fuel);
    mvprintw(0, 32, "field: %d", g.fields);
    if (debug) {
      mvprintw(0, 64, "%d", ch);
    }

    if (g.f.completed()) {
      mvprintw(HEIGHT / 2, WIDTH / 2 - strlen(FINISHED) / 2, "%s", FINISHED);
      mvprintw(HEIGHT / 2 + 2, WIDTH / 2 - strlen(REMINDER) / 2, "%s",
               REMINDER);
    }
    if (g.fuel <= 0) {
      mvprintw(HEIGHT / 2 + 1, WIDTH / 2 - strlen(FUEL) / 2, "%s", FUEL);
      mvprintw(HEIGHT / 2 + 2, WIDTH / 2 - strlen(REMINDER) / 2, "%s",
               REMINDER);
    }

    attron(COLOR_PAIR(2));
    if (g.rad == 3) {
      for (int i = -odd(g.rad); i <= odd(g.rad); i++) {
        for (int k = -odd(g.rad); k <= odd(g.rad); k++) {
          if ((i == 0 && k != 0) || (k == 0 && i != 0)) {
            mvaddch(k + g.playerY, i + g.playerX, '.');
          }
        }
      }
    }
    if (g.rad == 5) {
      for (int i = -odd(g.rad); i <= odd(g.rad); i++) {
        for (int k = -odd(g.rad); k <= odd(g.rad); k++) {
          if ((i == 0 && k != 0) || (k == 0 && i != 0) || (i == 1 && k != 0) ||
              (i == -1 && k != 0) || (i != 0 && k == 1) ||
              (i != 0 && k == -1)) {
            mvaddch(k + g.playerY, i + g.playerX, '.');
          }
        }
      }
    }
    mvaddch(g.playerY, g.playerX, '@');
    attroff(COLOR_PAIR(2));

    drawRect(HEIGHT + 1, 0, 2, WIDTH + 2, ' ', false);
    drawRect(0, WIDTH, HEIGHT + 1, 2, ' ', false);

    refresh();
    shop.draw();
    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000 / (30 - 15 * debug)));
  }

exit:
  endwin();
  return 0;
}