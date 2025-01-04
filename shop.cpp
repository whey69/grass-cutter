#include <cstdio>
#include <cstring>
#include <ncurses.h>

#include "fmt/core.h"
#include "game.cpp"

#define feprice(x) (int)(2499 * 1.2) * (x + 1)

// implicit const
char *MSG = (char*)"Tip: use 1, 2, 3 to purchase. Press TAB to exit.";
char *FAILURE = (char*)"Not enough money or stat is maxed out.";

class window {
public:
  WINDOW *window;
  game *g;
  bool toggled;
  bool showMsg;
  char *showMsg2;
  int width, height;

  void createWin(int x, int y, int w, int h, game *g) {
    WINDOW *win;
    win = newwin(h, w, y, x);
    box(win, 0, 0);

    wrefresh(win);
    this->window = win;
    this->g = g;
    this->width = w;
    this->height = h;
  }

  void destroyWin() {
    wborder(window, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(window);
    delwin(window);
  }

  // fuel    base 500 per 1000 fuel
  // fuellvv base 2499 * 1.2 per level
  // rad     base 15000 * 2 - 1 per level; goes 1, 3, 5
  void draw() {
    if (toggled) {
      box(window, 0, 0);

      mvwprintw(window, 2, 2, "1. fuel: %d ", g->fuel);
      mvwprintw(window, 3, 4, "price: %d (per 1000 units)", 500);

      mvwprintw(window, 5, 2, "2. fuel efficiency: %d (max 4)", g->fuelLvl);
      mvwprintw(window, 6, 4, "price: %d", feprice(g->fuelLvl));

      mvwprintw(window, 8, 2, "3. mower radius: %d (max 5)", g->rad);
      mvwprintw(window, 9, 4, "price: %d", 15000 * g->rad * 2 - 1);

      if (showMsg) {
        mvwprintw(window, height - 1, width / 2 - strlen(showMsg2) / 2, "%s", showMsg2);
      }
      wrefresh(window);
    }
  }

  void triggerMessage(bool show) {
    showMsg = show;
    showMsg2 = MSG;
  }

  void triggerMessage(bool show, char *msg) {
    triggerMessage(show);
    showMsg2 = msg;
  }

  void process(int product) {
    switch (product) {
    case 1:
      if (g->money >= 500) {
        g->money -= 500;
        g->fuel += 1000;
      } else {
        triggerMessage(true, FAILURE);
      }
      break;
    case 2:
      if (g->money >= feprice(g->fuelLvl) && g->fuelLvl < 4) {
        g->money -= feprice(g->fuelLvl);
        g->fuelLvl += 1;
      } else {
        triggerMessage(true, FAILURE);
      }
      break;
    case 3:
      if (g->money >= 15000 * g->rad * 2 - 1 && g->rad <= 5) {
        g->money -= 15000 * g->rad * 2 - 1;
        g->rad += 2;
      } else {
        triggerMessage(true, FAILURE);
      }
    }
  }
};