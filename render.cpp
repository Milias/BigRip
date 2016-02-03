#include "render.h"

RenderManager::RenderManager() : Width(0), Height(0)
{
  cfg = SingletonConfig::config();
}

int RenderManager::Initialize(SDL_Window *win)
{
  uint32_t flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;

  ren = SDL_CreateRenderer(win, -1, flags);
  if (ren == NULL) {
    LogSDLError(std::cout, "SDL_CreateRenderer");
  	SDL_Quit();
  	return 1;
  }

  SDL_GetRendererOutputSize(ren, &Width, &Height);

  return 0;
}
