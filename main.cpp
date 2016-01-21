#include "game.h"

int main(int argc, char** argv)
{
  GameManager *gm = new GameManager;
  if (gm->Initialize() == 0) { std::cout << "Startup successful.\n"; }
  gm->MainLoop();

  return 0;
}
