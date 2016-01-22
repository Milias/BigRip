#pragma once
#include "config.h"

struct AssetData
{
  Json::Value * json;
  SDL_Texture * tex;
  SDL_Surface * surf;
  bool IsLoaded;

  AssetData() : json(NULL), tex(NULL), surf(NULL), IsLoaded(false) {}
  ~AssetData() {
    delete json;
    Unload();
  }

  void Initialize(char const * f) {
    json = new Json::Value;
    std::ifstream file(f, std::ifstream::in);
    file >> *json;
    file.close();
  }

  void Unload() {
    if (tex != NULL) SDL_DestroyTexture(tex);
    if (surf != NULL) SDL_FreeSurface(surf);
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
    for (AssetsMap::iterator it = assets->begin(); it != assets->end(); it++) { delete it->second; }
    delete assets;
    delete aLib;
  }

  int Initialize(SDL_Renderer * r);
  void PreLoadAssets();
  AssetData * LoadAsset(std::string name);

  void UnloadAssets();
};
