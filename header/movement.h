#pragma once
#include "config.h"

struct SDL_Keycode_Hash {
  size_t operator()(const SDL_Keycode &e) const { return static_cast<size_t>(e); }
};

struct MoveEvent
{
  bool is_down;
  boost::uuids::uuid key_up;
  boost::uuids::uuid key_down;

  MoveEvent() : is_down(false), key_up(boost::uuids::nil_uuid()), key_down(boost::uuids::nil_uuid()) {}

  ~MoveEvent() {
    SingletonEvents::Unregister(key_up);
    SingletonEvents::Unregister(key_down);
  }
};

typedef std::unordered_multimap<SDL_Keycode, MoveEvent*, SDL_Keycode_Hash> MoveEventMap;

class MovementManager
{
  friend class GameManager;
private:
  Json::Value * cfg;
  MoveEventMap * map;

  b2World * world;
  b2Body * CenterBody, * ControlBody;

  double PhysicsDeltaTime;
  double PixelToMeters;
  double speedLimitX, speedLimitY;

  uint32_t vIter, pIter;

  boost::uuids::uuid UpdateTimer;

public:
  MovementManager();
  ~MovementManager() {
    SingletonEvents::UnregisterTimer(UpdateTimer);
    delete world;
    delete map;
  }

  int Initialize();
  void CreateWorld(double gx, double gy);
  b2Body * AddStaticBody(int32_t x, int32_t y, int32_t sx, int32_t sy);
  b2Body * AddStaticBody(SDL_Rect &r) { return AddStaticBody(r.x,r.y,r.w,r.h); }
  b2Body * AddDynamicBody(int32_t x, int32_t y, int32_t sx, int32_t sy, float d = 1.0, float f = 0.3);
  void SetFollowBody(b2Body * b) { CenterBody = b; }
  void SetControlBody(b2Body * b) { ControlBody = b; }
  void UpdateDynamicPositions(double dt, double elapsed);
  void SetToCenterInPixels(int32_t &x, int32_t &y);
};
