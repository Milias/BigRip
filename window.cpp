#include "window.h"

WindowManager::WindowManager()
{
  cfg = SingletonConfig::config();
}

int WindowManager::Initialize()
{
  uint32_t flags = SDL_WINDOW_MOUSE_CAPTURE | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;
  flags |= ((*cfg)["Display"]["Fullscreen"].asBool() ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0) | ((*cfg)["Display"]["Borderless"].asBool() ? SDL_WINDOW_BORDERLESS : 0);

  char str[80];
  strcat(str, (*cfg)["Game"]["Name"].asCString());
  strcat(str, " ");
  strcat(str, (*cfg)["Game"]["Version"].asCString());

  win = SDL_CreateWindow(
    str, 0, 0,
    (*cfg)["Display"]["Width"].asInt(),
    (*cfg)["Display"]["Height"].asInt(),
    flags
  );
  if (win == NULL) {
    LogSDLError(std::cout, "SDL_CreateWindow");
	  SDL_Quit();
	  return 1;
  }

  if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
	  LogSDLError(std::cout, "IMG_Init");
	  SDL_Quit();
	  return 1;
  }

  return 0;
}
