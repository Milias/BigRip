#include "movement.h"

MovementManager::MovementManager()
{
  cfg = SingletonConfig::config();
}

int MovementManager::Initialize()
{
  map = new MoveEventMap;

  MoveEvent * ev;
  std::string key;

  ev = new MoveEvent;
  key = (*cfg)["Keys"]["Left"].asString();
  ev->uuid = SingletonEvents::Register(SDL_KEYDOWN, std::bind<void>([this, key](SDL_Event &e) {
    if (strcmp(key.c_str(),SDL_GetKeyName(e.key.keysym.sym)) == 0) { this->CenterX -= 100; }
  }, std::placeholders::_1));
  map->emplace(key, ev);

  ev = new MoveEvent;
  key = (*cfg)["Keys"]["Right"].asString();
  ev->uuid = SingletonEvents::Register(SDL_KEYDOWN, std::bind<void>([this, key](SDL_Event &e) {
    if (strcmp(key.c_str(),SDL_GetKeyName(e.key.keysym.sym)) == 0) { this->CenterX += 100; }
  }, std::placeholders::_1));
  map->emplace(key, ev);

  return 0;
}
