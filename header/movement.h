#pragma once
#include "config.h"

struct MoveEvent
{
  boost::uuids::uuid uuid;
};

typedef std::unordered_multimap<std::string, MoveEvent*> MoveEventMap;

class MovementManager
{
  friend class GameManager;
private:
  Json::Value * cfg;
  MoveEventMap * map;

  int32_t CenterX, CenterY;

public:
  MovementManager();
  ~MovementManager() {}

  int Initialize();
};
