#pragma once
#include "config.h"
#include "window.h"
#include "render.h"
#include "assets.h"
#include "levels.h"
#include "interface.h"
#include "movement.h"

class GameManager
{
private:
  Json::Value *cfg;
  WindowManager *window;
  RenderManager *render;
  AssetsManager *assets;
  LevelsManager *levels;
  MovementManager *movement;

  bool StopMainLoop;
  int32_t CenterX, CenterY;

  std::vector<ObjectData*> CurrentLevel;

  void __CleanCurrentLevel();
  void __RenderCurrentLevel();

public:
  GameManager();
  ~GameManager() {
    __CleanCurrentLevel();
    delete window;
    delete render;
    delete assets;
    delete levels;
    delete movement;
    IMG_Quit();
    SDL_Quit();
  }

  int Initialize();
  int RenderAsset(char const * name, uint32_t x, uint32_t y);
  int LoadLevel(char const * name);

  void MainLoop();
};
