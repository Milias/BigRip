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
