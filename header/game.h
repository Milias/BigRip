#pragma once
#include <iostream>
#include <cstring>
#include "config.h"
#include "window.h"
#include "render.h"
#include "assets.h"
#include "levels.h"

class GameManager
{
private:
  Json::Value *cfg;
  WindowManager *window;
  RenderManager *render;
  AssetsManager *assets;

public:
  bool StopMainLoop;

  GameManager();
  ~GameManager() {
    delete window;
    delete render;
    IMG_Quit();
    SDL_Quit();
  }

  int Initialize();
  int RenderAsset(char const * name, uint32_t x, uint32_t y);

  void MainLoop();
};
