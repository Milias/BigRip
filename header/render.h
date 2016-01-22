#pragma once
#include "config.h"

class RenderManager
{
  friend class GameManager;
private:
  Json::Value *cfg;
  SDL_Renderer *ren;
  SDL_Texture * world;

public:
  RenderManager();
  ~RenderManager() { SDL_DestroyRenderer(ren); }

  int Initialize(SDL_Window *win);
};
