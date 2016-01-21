#include "assets.h"

AssetsManager::AssetsManager() : nAssets(0)
{
  cfg = SingletonConfig::config();
}

int AssetsManager::Initialize(SDL_Renderer * r)
{
  aLib = new Json::Value;
  std::ifstream file((*cfg)["Assets"]["List"].asCString(), std::ifstream::in);
  file >> *aLib;
  file.close();

  ren = r;

  return 0;
}

void AssetsManager::__Load(AssetData *a)
{
  if (a->IsLoaded) return;
  if ((*a->json)["Surface"].asBool()) {
    a->surf = IMG_Load((*a->json)["File"].asCString());
    if (a->surf == NULL) { LogSDLError(std::cout, "IMG_Load"); }
    else { a->IsLoaded = true; }
  }
  if ((*a->json)["Texture"].asBool()) {
    a->tex = IMG_LoadTexture(ren,(*a->json)["File"].asCString());
    if (a->tex == NULL) { LogSDLError(std::cout, "IMG_LoadTexture"); }
    else { a->IsLoaded = true; }
  }
}

int AssetsManager::PreLoadAssets()
{
  for (Json::ValueIterator it1 = aLib->begin(); it1 != aLib->end(); it1++) {
    for (Json::ValueIterator it2 = it1->begin(); it2 != it1->end(); it2++) {
      nAssets++;
    }
  }

  assets = new AssetsMap(nAssets);
  AssetData * load;
  for (Json::ValueIterator it1 = aLib->begin(); it1 != aLib->end(); it1++) {
    for (Json::ValueIterator it2 = it1->begin(); it2 != it1->end(); it2++) {
      load = new AssetData;
      load->Initialize(it2->asCString());
      assets->emplace((*load->json)["Name"].asCString(), load);
      if ((*load->json)["LoadOnStartup"].asBool()) { __Load(load); }
    }
  }

  SDL_SetCursor(SDL_CreateColorCursor(assets->at("DefaultCursor")->surf,0,0));
  std::cout << "Loaded " << nAssets << " assets.\n";
  return 0;
}

AssetData * AssetsManager::LoadAsset(std::string name)
{
  try {
    if (!assets->at(name)->IsLoaded) { __Load(assets->at(name)); }
    return assets->at(name);
  } catch (const std::out_of_range &e) {
    LogError(std::cout, e.what());
  }
  return NULL;
}
