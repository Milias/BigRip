#pragma once
// STL
#include <cstring>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <functional>
#include <chrono>

// Boost
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

// ThirdPary
#include "json.h"
#include "LuaBridge.h"

extern "C" {
  #include "lua.h"
  #include "lauxlib.h"
  #include "lualib.h"
}

// SDL
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

void LogError(std::ostream &os, char const * msg);
void LogSDLError(std::ostream &os, char const * msg);

struct SDL_Event_Hash {
  size_t operator()(const SDL_EventType &e) const { return static_cast<size_t>(e); }
};

struct SDL_Event_Compare {
  bool operator()(const SDL_Event &a, const SDL_Event &b) const { return a.type == b.type; }
};

struct EventResponse {
  boost::uuids::uuid uuid;
  std::function<void(SDL_Event&)> response;

  bool operator==(const EventResponse &a) const { return uuid == a.uuid; }
};

typedef std::unordered_multimap<SDL_EventType, EventResponse, SDL_Event_Hash> EventMap;

class SingletonConfig
{
private:
  Json::Value *ConfigRoot;
  static SingletonConfig *Instance;

  SingletonConfig() {
    ConfigRoot = new Json::Value;
    std::ifstream config("data/config.json", std::ifstream::in);
    config >> *ConfigRoot;
    config.close();
    std::cout << "Configuration loaded.\n";
  }

public:
  static Json::Value * config() {
    if (Instance == NULL) { Instance = new SingletonConfig; }
    return Instance->ConfigRoot;
  }

  static SingletonConfig * instance() {
    if (Instance == NULL) { Instance = new SingletonConfig; }
    return Instance;
  }
};

class SingletonEvents
{
private:
  Json::Value * cfg;
  static SingletonEvents *Instance;
  EventMap *map;

  uint32_t MaxProcessedEvents;

  SingletonEvents() : MaxProcessedEvents(32) {
    cfg = SingletonConfig::config();
    map = new EventMap;
  }

  void __Process();
  boost::uuids::uuid __Register(SDL_EventType e, std::function<void(SDL_Event&)> f);
  void __Unregister(boost::uuids::uuid &uuid);

public:
  static void Process();
  static boost::uuids::uuid Register(SDL_EventType e, std::function<void(SDL_Event&)> f);
  static void Unregister(boost::uuids::uuid &uuid);
};
