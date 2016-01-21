#include "render.h"

RenderManager::RenderManager()
{
  cfg = SingletonConfig::config();
}

int RenderManager::Initialize(SDL_Window *win)
{
  uint32_t flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
  flags |= ((*cfg)["Display"]["VSync"].asBool() ? SDL_RENDERER_PRESENTVSYNC : 0);
  ren = SDL_CreateRenderer(win, -1, flags);
  if (ren == NULL){
    LogSDLError(std::cout, "SDL_CreateRenderer");
  	SDL_Quit();
  	return 1;
  }
  return 0;
}
