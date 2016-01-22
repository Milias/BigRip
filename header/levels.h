#pragma once
#include "config.h"

struct LevelData
{
  Json::Value * json;
  bool IsLoaded;

  LevelData() : IsLoaded(false) {}
  ~LevelData() { delete json; }

  void Initialize(char const * f) {
    json = new Json::Value;
    std::ifstream file(f, std::ifstream::in);
    file >> *json;
    file.close();
  }
};

typedef std::unordered_map<std::string, LevelData*> LevelsMap;

struct ObjectData
{
  SDL_Texture * tex;
  SDL_Rect world, local;

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
  }

  void Shift(int32_t x, int32_t y) {
    local.x = world.x - x;
    local.y = world.y - y;
  }

  void Render(SDL_Renderer * ren) {
    SDL_RenderCopy(ren, tex, NULL, &local);
  }
};

class LevelsManager
{
  friend class GameManager;
private:
  Json::Value * cfg;
  Json::Value * lLib;
  LevelsMap * levels;

  uint32_t nLevels;

public:
  LevelsManager();
  ~LevelsManager() {
    for (LevelsMap::iterator it = levels->begin(); it != levels->end(); it++) { delete it->second; }
    delete levels;
    delete lLib;
  }

  int Initialize();
  void PreLoadLevels();
};
