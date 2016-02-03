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
    auto range = events->equal_range(static_cast<SDL_EventType>(e.type));
    for (auto it = range.first; it != range.second; it++) {
      if (!it->second->active) continue;
      it->second->response(e);
    }
  }
}

void SingletonEvents::__ProcessTimers()
{
  dt = std::chrono::duration<double>(GlobalClock.now() - t0).count();
  t0 = GlobalClock.now();
  elapsed += dt;
  for (auto it : *timers) it.second->update(dt,elapsed);
}

boost::uuids::uuid SingletonEvents::__Register(SDL_EventType e, std::function<void(SDL_Event&)> f)
{
  EventResponse * r = new EventResponse;;
  r->response = f;
  events->emplace(e, r);
  return r->uuid;
}

void SingletonEvents::__Unregister(boost::uuids::uuid &uuid)
{
  if (uuid.is_nil()) return;
  for (auto it : *events) {
    if (it.second->uuid == uuid) {
      events->erase(it.first);
      break;
    }
  }
}

boost::uuids::uuid SingletonEvents::__RegisterTimer(double T, std::function<void(double,double)> f, double wait)
{
  TimedEvent * r = new TimedEvent;
  r->period = T;
  r->wait = wait;
  r->response.push_back(f);
  timers->emplace(r->uuid, r);
  return r->uuid;
}

void SingletonEvents::__AddToTimer(boost::uuids::uuid &uuid, std::function<void(double,double)> f)
{
  if (uuid.is_nil()) return;
  try {
    timers->at(uuid)->response.push_back(f);
  } catch (std::out_of_range &e) {
    LogError(std::cout, "Error appending to timer.");
  }
}

void SingletonEvents::__RemoveFromTimer(boost::uuids::uuid &uuid, uint32_t N)
{
  if (uuid.is_nil()) return;
  try {
    TimedEvent * it = timers->at(uuid);
    N = (N > it->response.size() ? it->response.size() : N);
    for (uint32_t i = 0; i < N; i++) it->response.pop_back();
  } catch (std::out_of_range &e) {
    LogError(std::cout, "Error removing from timer.");
  }
}

void SingletonEvents::__UnregisterTimer(boost::uuids::uuid &uuid)
{
  if (uuid.is_nil()) return;
  timers->erase(uuid);
}

void SingletonEvents::Process()
{
  if (Instance == NULL) { Instance = new SingletonEvents; }
  Instance->__Process();
}

void SingletonEvents::ProcessTimers()
{
  if (Instance == NULL) { Instance = new SingletonEvents; }
  Instance->__ProcessTimers();
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

boost::uuids::uuid SingletonEvents::RegisterTimer(double T, std::function<void(double,double)> f, double wait)
{
  if (Instance == NULL) { Instance = new SingletonEvents; }
  return Instance->__RegisterTimer(T, f, wait);
}

void SingletonEvents::AddToTimer(boost::uuids::uuid &uuid, std::function<void(double,double)> f)
{
  if (Instance == NULL) { Instance = new SingletonEvents; }
  Instance->__AddToTimer(uuid, f);
}

void SingletonEvents::RemoveFromTimer(boost::uuids::uuid &uuid, uint32_t N)
{
  if (Instance == NULL) { Instance = new SingletonEvents; }
  Instance->__RemoveFromTimer(uuid, N);
}

void SingletonEvents::UnregisterTimer(boost::uuids::uuid &uuid)
{
  if (Instance == NULL) { Instance = new SingletonEvents; }
  Instance->__UnregisterTimer(uuid);
}

void SingletonEvents::StartTimer()
{
  if (Instance == NULL) { Instance = new SingletonEvents; }
  Instance->t0 = GlobalClock.now();
}
