#pragma once
#include "config.h"

struct ObjectData
{
  SDL_Texture * tex;
  SDL_Rect world, local;
  bool Rendered;

  void Initialize(SDL_Texture * t, int32_t x, int32_t y, uint32_t w, uint32_t h) {
    tex = t;
    world.x = x;
    world.y = y;
    world.w = w;
    world.h = h;
    local.x = x;
    local.y = y;
    local.w = w;
    local.h = h;
    Rendered = false;
  }

  void Shift(int32_t x, int32_t y) {
    local.x = world.x - x;
    local.y = world.y - y;
  }

  void Render(SDL_Renderer * ren) {
    if (!Rendered) return;
    SDL_RenderCopy(ren, tex, NULL, &local);
  }
};

class RenderManager
{
  friend class GameManager;
private:
  Json::Value *cfg;
  SDL_Renderer *ren;
  SDL_Texture * world;

  int32_t Width, Height;

public:
  RenderManager();
  ~RenderManager() { SDL_DestroyRenderer(ren); }

  int Initialize(SDL_Window *win);
};
