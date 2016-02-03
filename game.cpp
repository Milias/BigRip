#include "game.h"

GameManager::GameManager() : StopMainLoop(false), CenterX(0), CenterY(0)
{
  cfg = SingletonConfig::config();
}

void GameManager::__CleanCurrentLevel()
{
  for (std::vector<ObjectData*>::iterator it = CurrentLevel.begin(); it != CurrentLevel.end(); it++) {
    delete *it;
  }
}

void GameManager::__RenderCurrentLevel()
{
  for (std::vector<ObjectData*>::iterator it = CurrentLevel.begin(); it != CurrentLevel.end(); it++) {
    (*it)->Shift(CenterX, CenterY);
    (*it)->Rendered = ((*it)->local.x) < render->Width && ((*it)->local.y) < render->Height && ((*it)->local.x + (*it)->local.w) > 0 && ((*it)->local.y + (*it)->local.h) > 0;
    (*it)->Render(render->ren);
  }
}

int GameManager::Initialize()
{
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    LogSDLError(std::cout, "SDL_Init");
    return 1;
	}

  if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
	  LogSDLError(std::cout, "IMG_Init");
	  SDL_Quit();
	  return 1;
  }

  if(TTF_Init()==-1) {
	  LogSDLError(std::cout, "TTF_Init");
    return 1;
  }

  window = new WindowManager;
  if (window->Initialize()) {
    LogError(std::cout, "Error initializing WindowManager.\n");
    return 1;
  }

  render = new RenderManager;
  if (render->Initialize(window->win)) {
    LogError(std::cout, "Error initializing RenderManager.\n");
    return 1;
  }

  assets = new AssetsManager;
  if (assets->Initialize(render->ren)) {
    LogError(std::cout, "Error initializing AssetsManager.\n");
    return 1;
  }

  levels = new LevelsManager;
  if (levels->Initialize()) {
    LogError(std::cout, "Error initializing LevelsManager.\n");
    return 1;
  }

  movement = new MovementManager;
  if (movement->Initialize()) {
    LogError(std::cout, "Error initializing MovementManager.\n");
    return 1;
  }

  assets->PreLoadAssets();
  levels->PreLoadLevels();

  SingletonEvents::AddToTimer(movement->UpdateTimer, std::bind<void>([this](double dt, double elapsed) {
    this->movement->SetToCenterInPixels(this->CenterX, this->CenterY);
    this->CenterX -= this->render->Width / 2;
    this->CenterY -= this->render->Height / 2;
  }, _1, _2));

  SingletonEvents::RegisterTimer(-1.0, std::bind<void>([this](double dt, double elapsed)
  {
    SDL_RenderClear(this->render->ren);
    this->__RenderCurrentLevel();
    this->RenderAsset("DefaultCursor", this->render->Width / 2, this->render->Height / 2);
    SDL_RenderPresent(this->render->ren);
  }, _1, _2));

  SingletonEvents::Register(SDL_QUIT, std::bind<void>([this](SDL_Event &e)
  {
    this->StopMainLoop = true;
  }, _1));

  return 0;
}

int GameManager::RenderAsset(char const * name, uint32_t x, uint32_t y)
{
  SDL_Rect dst;
	dst.x = x;
	dst.y = y;
  AssetData * asset = assets->LoadAsset(name);
  if (asset == NULL) return 1;
  dst.w = (*asset->json)["Size"][2].asInt();
  dst.h = (*asset->json)["Size"][3].asInt();
	SDL_RenderCopy(render->ren, asset->tex == NULL ? SDL_CreateTextureFromSurface(render->ren,asset->surf) : asset->tex, NULL, &dst);
  return 0;
}

int GameManager::LoadLevel(char const * name)
{
  LevelData * level;
  try {
    level = levels->levels->at(name);
  } catch (const std::out_of_range &e) {
    LogError(std::cout, "Error loading level.\n");
    LogError(std::cout, e.what());
    return 1;
  }

  assets->UnloadAssets();
  CurrentLevel.clear();
  AssetData * asset;
  ObjectData * obj;

  movement->CreateWorld((*level->json)["Gravity"][0].asDouble(),(*level->json)["Gravity"][1].asDouble());

  for (Json::ValueIterator it = (*level->json)["Assets"].begin(); it != (*level->json)["Assets"].end(); it++, asset = NULL) {
    asset = assets->LoadAsset(it->asCString());
    if (asset != NULL) {
      for (Json::ValueIterator it2 = (*level->json)["Tiles"][it->asCString()].begin(); it2 != (*level->json)["Tiles"][it->asCString()].end(); it2++) {
        obj = new ObjectData;
        obj->Initialize(asset->tex, (*it2)[0].asInt(), (*it2)[1].asInt(), (*asset->json)["Size"][2].asUInt(), (*asset->json)["Size"][3].asUInt());
        movement->AddStaticBody(obj->world);
        CurrentLevel.push_back(obj);
      }
    }
  }

  b2Body * b = movement->AddDynamicBody((*level->json)["Spawn"][0].asInt(), (*level->json)["Spawn"][1].asInt(), 64, 64);
  movement->SetFollowBody(b);
  movement->SetControlBody(b);
  return 0;
}

void GameManager::MainLoop()
{
  SingletonEvents::StartTimer();
  while(!StopMainLoop) {
    SingletonEvents::Process();
    SingletonEvents::ProcessTimers();
  }
}
