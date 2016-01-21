#pragma once
#include <iostream>
#include "config.h"

struct AssetData
{
  Json::Value * json;
  SDL_Texture * tex;
  SDL_Surface * surf;
  bool IsLoaded;

  AssetData() : json(NULL), tex(NULL), IsLoaded(false) {}
  ~AssetData() {
    delete json;
    if (tex!=NULL) SDL_DestroyTexture(tex);
    if (surf!=NULL) SDL_FreeSurface(surf);
  }

  void Initialize(char const * f) {
    json = new Json::Value;
    std::ifstream file(f, std::ifstream::in);
    file >> *json;
    file.close();
  }
};

typedef std::unordered_map<std::string, AssetData*> AssetsMap;

class AssetsManager
{
  friend class GameManager;
private:
  Json::Value * cfg;
  Json::Value * aLib;
  AssetsMap * assets;

  SDL_Renderer * ren;

  uint32_t nAssets;

  void __Load(AssetData *a);

public:
  AssetsManager();
  ~AssetsManager() {
    delete assets;
    delete aLib;
  }

  int Initialize(SDL_Renderer * r);
  int PreLoadAssets();
  AssetData * LoadAsset(std::string name);
};
