
#include "Game.hpp"
#include "Position.hpp"
#include <cstring>

static Position ParseResolution(char* pResString)
{
  Position resolution;
  char* pArgv = strtok(pResString, "xX");
  try {
    resolution.x = std::stoi(pArgv);
    pArgv = strtok(nullptr, "xX");
    resolution.y = std::stoi(pArgv);
  } catch (...) {
    resolution = { 0, 0 };
  }
  return resolution;
}


int main(int argc, char* argv[])
{
  Position const resolution = (argc > 1) ? ParseResolution(argv[1]) : Position{ 0, 0 };

  Game game(resolution);
  game.Run();

  return 0;
}