#include "movement.h"

MovementManager::MovementManager() : map(NULL), world(NULL), CenterBody(NULL), ControlBody(NULL), PhysicsDeltaTime(0.00833333), PixelToMeters(0.03125), speedLimitX(10.0), speedLimitY(100.0), vIter(6), pIter(2)
{
  cfg = SingletonConfig::config();
}

int MovementManager::Initialize()
{
  if (map != NULL) { return 1; }
  map = new MoveEventMap;

  MoveEvent * ev;
  SDL_Keycode key;

  ev = new MoveEvent;
  key = SDL_GetKeyFromName((*cfg)["Keys"]["Left"].asCString());

  ev->timer = SingletonEvents::RegisterTimer(PhysicsDeltaTime, std::bind<void>([this](double dt, double elapsed) {
    this->ControlBody->ApplyLinearImpulse(b2Vec2(1.0,0.0), this->ControlBody->GetWorldCenter(), true);
  }, _1, _2));

  SingletonEvents::ToggleTimer(ev->timer, false);

  ev->key_down = SingletonEvents::Register(SDL_KEYDOWN, std::bind<void>([this, key, ev](SDL_Event &e) {
    if (key == e.key.keysym.sym && ControlBody != NULL && !ev->is_down) {
      SingletonEvents::ToggleTimer(ev->timer, true);
      ev->is_down = true;
    }
  }, _1));

  ev->key_up = SingletonEvents::Register(SDL_KEYUP, std::bind<void>([this, key, ev](SDL_Event &e) {
    if (key == e.key.keysym.sym && ControlBody != NULL) {
      SingletonEvents::ToggleTimer(ev->timer, false);
      ev->is_down = false;
    }
  }, _1));

  map->emplace(key, ev);

  ev = new MoveEvent;
  key = SDL_GetKeyFromName((*cfg)["Keys"]["Right"].asCString());

  ev->timer = SingletonEvents::RegisterTimer(PhysicsDeltaTime, std::bind<void>([this](double dt, double elapsed) {
    this->ControlBody->ApplyLinearImpulse(b2Vec2(-1.0,0.0), this->ControlBody->GetWorldCenter(), true);
  }, _1, _2));

  SingletonEvents::ToggleTimer(ev->timer, false);

  ev->key_down = SingletonEvents::Register(SDL_KEYDOWN, std::bind<void>([this, key, ev](SDL_Event &e) {
    if (key == e.key.keysym.sym && ControlBody != NULL && !ev->is_down) {
      SingletonEvents::ToggleTimer(ev->timer, true);
      ev->is_down = true;
    }
  }, _1));

  ev->key_up = SingletonEvents::Register(SDL_KEYUP, std::bind<void>([this, key, ev](SDL_Event &e) {
    if (key == e.key.keysym.sym && ControlBody != NULL) {
      SingletonEvents::ToggleTimer(ev->timer, false);
      ev->is_down = false;
    }
  }, _1));

  map->emplace(key, ev);

  ev = new MoveEvent;
  key = SDL_GetKeyFromName((*cfg)["Keys"]["Jump"].asCString());
  ev->key_down = SingletonEvents::Register(SDL_KEYDOWN, std::bind<void>([this, key, ev](SDL_Event &e) {
    if (key == e.key.keysym.sym && ControlBody != NULL && !ev->is_down) {
      this->ControlBody->ApplyLinearImpulse(b2Vec2(0.0,-22.0), this->ControlBody->GetWorldCenter(), true);
      ev->is_down = true;
    }
  }, _1));

  ev->key_up = SingletonEvents::Register(SDL_KEYUP, std::bind<void>([this, key, ev](SDL_Event &e) {
    if (key == e.key.keysym.sym && ControlBody != NULL) {
      ev->is_down = false;
    }
  }, _1));

  map->emplace(key, ev);

  UpdateTimer = SingletonEvents::RegisterTimer(PhysicsDeltaTime, std::bind(&MovementManager::UpdateDynamicPositions, this, _1, _2));

  return 0;
}

void MovementManager::CreateWorld(double gx, double gy)
{
  if (world != NULL) {
    CenterBody = NULL;
    ControlBody = NULL;
    delete world;
  }
  world = new b2World(b2Vec2(gx, gy));
}

b2Body * MovementManager::AddStaticBody(int32_t x, int32_t y, int32_t sx, int32_t sy)
{
  if (world == NULL) return NULL;
  b2BodyDef def;
	def.position.Set(x * PixelToMeters, y * PixelToMeters);

	b2Body * body = world->CreateBody(&def);

	b2PolygonShape box;
	box.SetAsBox(sx * PixelToMeters / 2.0, sy * PixelToMeters / 2.0);
	body->CreateFixture(&box, 0.0f);
  return body;
}

b2Body * MovementManager::AddDynamicBody(int32_t x, int32_t y, int32_t sx, int32_t sy, float d, float f)
{
  if (world == NULL) return NULL;
  b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;
  bodyDef.position.Set(x * PixelToMeters, y * PixelToMeters);
  bodyDef.fixedRotation = true;
  b2Body * body = world->CreateBody(&bodyDef);

  b2PolygonShape dynamicBox;
  dynamicBox.SetAsBox(sx * PixelToMeters / 2.0, sy * PixelToMeters / 2.0);

  b2FixtureDef fixtureDef;
  fixtureDef.shape = &dynamicBox;
  fixtureDef.density = d;
  fixtureDef.friction = f;
  body->CreateFixture(&fixtureDef);
  return body;
}

void MovementManager::UpdateDynamicPositions(double dt, double elapsed)
{
  if (world == NULL) return;
  if (ControlBody != NULL) {
    if (ControlBody->GetLinearVelocity().x >= speedLimitX) {
      ControlBody->SetLinearVelocity(b2Vec2(speedLimitX,ControlBody->GetLinearVelocity().y));
    } else if (ControlBody->GetLinearVelocity().x < -speedLimitX) {
      ControlBody->SetLinearVelocity(b2Vec2(-speedLimitX,ControlBody->GetLinearVelocity().y));
    }
  }

  world->Step(dt, vIter, pIter);
  world->ClearForces();
}

void MovementManager::SetToCenterInPixels(int32_t &x, int32_t &y)
{
  if (CenterBody == NULL) return;
  b2Vec2 center = CenterBody->GetWorldCenter();
  x = center.x / PixelToMeters;
  y = center.y / PixelToMeters;
}
