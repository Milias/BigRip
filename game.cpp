#include "game.h"

GameManager::GameManager() : StopMainLoop(false)
{
  cfg = SingletonConfig::config();
}

int GameManager::Initialize()
{
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    LogSDLError(std::cout, "SDL_Init");
    return 0;
	}

  window = new WindowManager;
  if (window->Initialize()) { return 1; }

  render = new RenderManager;
  if (render->Initialize(window->win)) { return 1; }

  assets = new AssetsManager;
  if (assets->Initialize(render->ren)) {  return 1; }

  assets->PreLoadAssets();

  SingletonEvents::Register(SDL_QUIT, std::bind<void>([this](SDL_Event &e) { this->StopMainLoop = true; }, std::placeholders::_1));

  return 0;
}

int GameManager::RenderAsset(char const * name, uint32_t x, uint32_t y)
{
  SDL_Rect dst;
	dst.x = x;
	dst.y = y;
  AssetData * asset = assets->LoadAsset(name);
  dst.w = (*asset->json)["Size"][2].asInt();
  dst.h = (*asset->json)["Size"][3].asInt();
	SDL_RenderCopy(render->ren, asset->tex, NULL, &dst);
  return 0;
}

void GameManager::MainLoop()
{
  double t0 = SDL_GetTicks() * 0.001;
  double dt;
  double elapsed = 0.0;
  uint32_t frames = 0;
  while(!StopMainLoop) {
    dt = SDL_GetTicks() * 0.001 - t0;
    t0 = SDL_GetTicks() * 0.001;
    elapsed += dt;
    SingletonEvents::Process();
    SDL_RenderClear(render->ren);
    RenderAsset("Floor01",960 + 200*cos(elapsed),540.0 - 200*sin(elapsed));
  	SDL_RenderPresent(render->ren);
    frames++;
  }
  std::cout << (frames/elapsed) << std::endl;
}
