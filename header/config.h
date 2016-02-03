#pragma once
// STL
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>

// Boost
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/functional/hash.hpp>

// ThirdPary
#include "json.h"
#include "LuaBridge.h"
#include <Box2D/Box2D.h>

extern "C" {
  #include "lua.h"
  #include "lauxlib.h"
  #include "lualib.h"
}

// SDL
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

using namespace std::placeholders;

typedef std::chrono::high_resolution_clock DefaultClock;
typedef std::chrono::time_point<DefaultClock> t_point;

void LogError(std::ostream &os, char const * msg);
void LogSDLError(std::ostream &os, char const * msg);

struct SDL_Event_Hash {
  size_t operator()(const SDL_EventType &e) const { return static_cast<size_t>(e); }
};

struct EventResponse {
  bool active;
  boost::uuids::uuid uuid;
  std::function<void(SDL_Event&)> response;

  EventResponse() : active(true), uuid(boost::uuids::random_generator()()) {}

  bool operator==(const EventResponse &a) const { return uuid == a.uuid; }
};

typedef std::vector<std::function<void(double,double)>> TimerResponse;

struct TimedEvent {
  bool active;
  boost::uuids::uuid uuid;
  TimerResponse response;

  double acc, period, wait;

  TimedEvent() : active(true), uuid(boost::uuids::random_generator()()), acc(0.0), period(0.0), wait(0.0) {}

  bool operator==(const EventResponse &a) const { return uuid == a.uuid; }

  void update(double dt, double elapsed) {
    if (!active) return;
    if (wait > 0.0) {
      wait -= dt;
      return;
    }

    if (period > 0.0) {
      acc += dt;
      for (;acc >= period; acc -= period) {
        for (auto it : response) it(period, elapsed);
      }
    } else { for (auto it : response) it(dt, elapsed); }
  }
};

typedef std::unordered_multimap<SDL_EventType, EventResponse *, SDL_Event_Hash> EventMap;
typedef std::unordered_map<boost::uuids::uuid, TimedEvent *, boost::hash<boost::uuids::uuid>> TimersMap;

class SingletonConfig
{
private:
  Json::Value *ConfigRoot;
  static SingletonConfig * Instance;

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
  static DefaultClock GlobalClock;
  static SingletonEvents * Instance;
  EventMap * events;
  TimersMap * timers;

  uint32_t MaxProcessedEvents;

  t_point t0;
  double dt, elapsed;

  SingletonEvents() : MaxProcessedEvents(32), dt(0.0), elapsed(0.0) {
    cfg = SingletonConfig::config();
    events = new EventMap;
    timers = new TimersMap;
  }

  void __Process();
  void __ProcessTimers();

  boost::uuids::uuid __Register(SDL_EventType e, std::function<void(SDL_Event&)> f);
  void __Unregister(boost::uuids::uuid &uuid);

  boost::uuids::uuid __RegisterTimer(double T, std::function<void(double,double)> f, double wait);
  void __AddToTimer(boost::uuids::uuid &uuid, std::function<void(double,double)> f);
  void __RemoveFromTimer(boost::uuids::uuid &uuid, uint32_t N);
  void __UnregisterTimer(boost::uuids::uuid &uuid);

  void __Toggle(boost::uuids::uuid &uuid, bool s);
  void __ToggleTimer(boost::uuids::uuid &uuid, bool s);

public:
  static void Process();
  static void ProcessTimers();

  static boost::uuids::uuid Register(SDL_EventType e, std::function<void(SDL_Event&)> f);
  static void Unregister(boost::uuids::uuid &uuid);

  static boost::uuids::uuid RegisterTimer(double T, std::function<void(double,double)> f, double wait = 0.0);
  static void AddToTimer(boost::uuids::uuid &uuid, std::function<void(double,double)> f);
  static void RemoveFromTimer(boost::uuids::uuid &uuid, uint32_t N = 1);
  static void UnregisterTimer(boost::uuids::uuid &uuid);

  static void Toggle(boost::uuids::uuid &uuid, bool s);
  static void ToggleTimer(boost::uuids::uuid &uuid, bool s);

  static void StartTimer();
};
