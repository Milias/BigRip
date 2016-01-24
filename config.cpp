#include "config.h"

SingletonConfig *SingletonConfig::Instance = NULL;
SingletonEvents *SingletonEvents::Instance = NULL;

void LogError(std::ostream &os, char const * msg)
{
  os << "Error: " << msg << std::endl;
}

void LogSDLError(std::ostream &os, char const * msg)
{
  os << msg << " error: " << SDL_GetError() << std::endl;
}

void SingletonEvents::__Process()
{
  SDL_Event e;
  for (uint32_t i = 0; i < MaxProcessedEvents && SDL_PollEvent(&e); i++) {
    auto range = map->equal_range(static_cast<SDL_EventType>(e.type));
    for (auto it = range.first; it != range.second; it++) {
      it->second.response(e);
    }
  }
}

boost::uuids::uuid SingletonEvents::__Register(SDL_EventType e, std::function<void(SDL_Event&)> f)
{
  EventResponse r;
  r.uuid = boost::uuids::random_generator()();
  r.response = f;
  map->emplace(e, r);
  return r.uuid;
}

void SingletonEvents::__Unregister(boost::uuids::uuid &uuid)
{
  for (auto it : *map) {
    if (it.second.uuid == uuid) {
      map->erase(it.first);
      break;
    }
  }
}

void SingletonEvents::Process()
{
  if (Instance == NULL) { Instance = new SingletonEvents; }
  Instance->__Process();
}

boost::uuids::uuid SingletonEvents::Register(SDL_EventType e, std::function<void(SDL_Event&)> f)
{
  if (Instance == NULL) { Instance = new SingletonEvents; }
  return Instance->__Register(e, f);
}

void SingletonEvents::Unregister(boost::uuids::uuid &uuid)
{
  if (Instance == NULL) { Instance = new SingletonEvents; }
  Instance->__Unregister(uuid);
}
