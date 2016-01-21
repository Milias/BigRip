#pragma once
#include <iostream>
#include "config.h"

class WindowManager
{
  friend class GameManager;
private:
  Json::Value *cfg;
  SDL_Window *win;

public:
  WindowManager();
  ~WindowManager() {
    SDL_DestroyWindow(win);
  }

  int Initialize();
};
