#include "levels.h"

LevelsManager::LevelsManager() : nLevels(0)
{
  cfg = SingletonConfig::config();
}

int LevelsManager::Initialize()
{
  lLib = new Json::Value;
  std::ifstream file((*cfg)["Levels"]["List"].asCString(), std::ifstream::in);
  file >> *lLib;
  file.close();

  return 0;
}

void LevelsManager::PreLoadLevels()
{
  for (Json::ValueIterator it1 = lLib->begin(); it1 != lLib->end(); it1++) {
    for (Json::ValueIterator it2 = it1->begin(); it2 != it1->end(); it2++) {
      nLevels++;
    }
  }

  levels = new LevelsMap(nLevels);
  LevelData * load;
  for (Json::ValueIterator it1 = lLib->begin(); it1 != lLib->end(); it1++) {
    for (Json::ValueIterator it2 = it1->begin(); it2 != it1->end(); it2++) {
      load = new LevelData;
      load->Initialize(it2->asCString());
      levels->emplace((*load->json)["Name"].asCString(), load);
    }
  }

  std::cout << "Loaded " << nLevels << " levels.\n";
}
