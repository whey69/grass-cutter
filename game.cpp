#pragma once

#include <vector>

class field {
public:
  // 2d array, containing either 0 for untouched and 1 for touched
  std::vector<std::vector<int>> field;

  void init(int w, int h) {
    this->w = w;
    this->h = h;
    for (int i = 0; i < w; i++) {
      field.push_back(std::vector<int>());
      for (int k = 0; k < h; k++) {
        field[i].push_back(0);
      }
    }
  }
  void clearField() {
    for (int i = 0; i < w; i++) {
      for (int k = 0; k < h; k++) {
        field[i][k] = 0;
      }
    }
    cutted = 0;
  }
  int cutted = 0;
  void cut(int x, int y) {
    field[x][y] = 1;
    cutted += 1;
  }
  int get(int x, int y) { return field[x][y]; }

  bool completed() { return cutted == w * h; };

private:
  int w, h;
};

class game {
public:
  // basic
  int money;
  int fuel = 1000;
  int fuelLvl;
  int lvl;
  int rad = 1;

  field f;

  // stats
  int fields;

  // plr
  int playerX = 1;
  int playerY = 2;

  int price = 1;

  game(int w, int h) {
    f.init(w, h);
    this->w = w;
    this->h = h;
  }

  void cut(int x, int y, int r) {
    if (r == 1) {
      if (f.get(x, y) == 0) {
        money += price;
        f.cut(x, y);
      }
    }
    if (r == 3) {
      for (int i = -1; i <= 1; i++) {
        for (int k = -1; k <= 1; k++) {
          if (x + i >= w || x + i < 0)
            continue;
          if (y + k >= h || y + k < 0)
            continue;
          if (((i == 0 && k != 0) || (k == 0 && i != 0)) &&
              f.get(x + i, y + k) == 0) {
            money += price;
            f.cut(x + i, y + k);
          }
        }
      }
    }
    if (r == 5) {
      for (int i = -2; i <= 2; i++) {
        for (int k = -2; k <= 2; k++) {
          if (x + i >= w || x + i < 0)
            continue;
          if (y + k >= h || y + k < 0)
            continue;
          if (((i == 0 && k != 0) || (k == 0 && i != 0) || (i == 1 && k != 0) ||
               (i == -1 && k != 0) || (i != 0 && k == 1) ||
               (i != 0 && k == -1)) &&
              f.get(x + i, y + k) == 0) {
            money += price;
            f.cut(x + i, y + k);
          }
        }
      }
    }
  }

  int fuelCounter = 0;
  void deFuel(int mod) {
    if (fuelLvl == 0) {
      fuel -= mod * 1;
    }
    if (fuelLvl == 1) {
      fuelCounter += mod * 1;
      if (fuelCounter % 2 == 1) {
        fuel -= mod * 1;
      }
    }
    if (fuelLvl == 2) {
      fuelCounter += mod * 1;
      if (fuelCounter % 3 == 1) {
        fuel -= mod * 1;
      }
    }
    if (fuelLvl == 3) {
      fuelCounter += mod * 1;
      if (fuelCounter % 4 == 1) {
        fuel -= mod * 1;
      }
    }
    if (fuelLvl == 4) {
      fuelCounter += mod * 1;
      if (fuelCounter % 8 == 1) {
        fuel -= mod * 1;
      }
    }
  }

  void deFuel() { deFuel(1); }

private:
  int w, h;
};