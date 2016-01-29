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

  b2World * world;

  double PixelToMeters;
  double CenterX, CenterY;
  double speedLimitX, speedLimitY;

  uint32_t vIter, pIter;

  b2Body * CenterBody;

public:
  MovementManager();
  ~MovementManager() {}

  int Initialize();
  void CreateWorld();
  void SetCenter(int32_t x, int32_t y);
  void UpdateDynamicPositions(double dt);
};
