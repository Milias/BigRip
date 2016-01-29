#include "movement.h"

MovementManager::MovementManager() : PixelToMeters(0.03125), CenterX(0), CenterY(0), speedLimitX(10.0), speedLimitY(10.0), vIter(6), pIter(2)
{
  cfg = SingletonConfig::config();
}

void MovementManager::SetCenter(int32_t x, int32_t y)
{
  CenterX = x * PixelToMeters;
  CenterY = y * PixelToMeters;
}

int MovementManager::Initialize()
{
  map = new MoveEventMap;

  MoveEvent * ev;
  std::string key;

  ev = new MoveEvent;
  key = (*cfg)["Keys"]["Left"].asString();
  ev->uuid = SingletonEvents::Register(SDL_KEYDOWN, std::bind<void>([this, key](SDL_Event &e) {
    static b2Vec2 f = b2Vec2(3.0,0.0);
    if (strcmp(key.c_str(),SDL_GetKeyName(e.key.keysym.sym)) == 0 && CenterBody != NULL) {
      this->CenterBody->ApplyLinearImpulse(f, this->CenterBody->GetWorldCenter(), true);
    }
  }, std::placeholders::_1));
  map->emplace(key, ev);

  ev = new MoveEvent;
  key = (*cfg)["Keys"]["Right"].asString();
  ev->uuid = SingletonEvents::Register(SDL_KEYDOWN, std::bind<void>([this, key](SDL_Event &e) {
    static b2Vec2 f = b2Vec2(-3.0,0.0);
    if (strcmp(key.c_str(),SDL_GetKeyName(e.key.keysym.sym)) == 0 && CenterBody != NULL) {
      this->CenterBody->ApplyLinearImpulse(f, this->CenterBody->GetWorldCenter(), true);
    }
  }, std::placeholders::_1));
  map->emplace(key, ev);

  ev = new MoveEvent;
  key = (*cfg)["Keys"]["Jump"].asString();
  ev->uuid = SingletonEvents::Register(SDL_KEYDOWN, std::bind<void>([this, key](SDL_Event &e) {
    static b2Vec2 f = b2Vec2(0.0,-10.0);
    if (strcmp(key.c_str(),SDL_GetKeyName(e.key.keysym.sym)) == 0 && CenterBody != NULL) {
      this->CenterBody->ApplyLinearImpulse(f, this->CenterBody->GetWorldCenter(), true);
    }
  }, std::placeholders::_1));
  map->emplace(key, ev);

  return 0;
}

void MovementManager::CreateWorld()
{
  world = new b2World(b2Vec2(0.0, 10.0));
  b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(CenterX, CenterY + 0.5f);

  // Call the body factory which allocates memory for the ground body
	// from a pool and creates the ground box shape (also from a pool).
	// The body is also added to the world.
	b2Body* groundBody = world->CreateBody(&groundBodyDef);

  // Define the ground box shape.
	b2PolygonShape groundBox;

  // The extents are the half-widths of the box.
	groundBox.SetAsBox(50.0f, 0.5f);

  // Add the ground fixture to the ground body.
	groundBody->CreateFixture(&groundBox, 0.0f);

  // Define the dynamic body. We set its position and call the body factory.
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(CenterX, CenterY - 4.0);
	CenterBody = world->CreateBody(&bodyDef);

  // Define another box shape for our dynamic body.
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(0.5f, 0.5f);

  // Define the dynamic body fixture.
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;

  // Set the box density to be non-zero, so it will be dynamic.
	fixtureDef.density = 1.0f;

	// Override the default friction.
	fixtureDef.friction = 0.3f;

	// Add the shape to the body.
	CenterBody->CreateFixture(&fixtureDef);
}

void MovementManager::UpdateDynamicPositions(double dt)
{
  dt = 1/60.0;
  if (CenterBody->GetLinearVelocity().x >= speedLimitX) {
    CenterBody->SetLinearVelocity(b2Vec2(speedLimitX,CenterBody->GetLinearVelocity().y));
  } else if (CenterBody->GetLinearVelocity().x < -speedLimitX) {
    CenterBody->SetLinearVelocity(b2Vec2(-speedLimitX,CenterBody->GetLinearVelocity().y));
  }
  world->Step(dt, vIter, pIter);
  CenterX = CenterBody->GetPosition().x + 0.5;
  CenterY = CenterBody->GetPosition().y + 0.5;
  world->ClearForces();
}
