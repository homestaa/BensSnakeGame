#pragma once

#include "Engine.hpp"
#include "Entity.hpp"
#include "Position.hpp"
#include <deque>

typedef struct _Mix_Music Mix_Music;
typedef struct Mix_Chunk Mix_Chunk;

class Game
{
public:
  Game(void);
  ~Game(void);

  void Run(void);

private:
  enum class Direction
  {
    Up,
    Down,
    Left,
    Right
  };

  static int constexpr FIELD_WIDTH = 50;
  static int constexpr FIELD_HEIGHT = 50;
  static Position constexpr FIELD_POSITION = {200, 200};
  static Position constexpr FIELD_SCALE = {800, 800};
  static Position constexpr FIELD_PART_SCALE = {FIELD_SCALE.x / FIELD_WIDTH, FIELD_SCALE.y / FIELD_HEIGHT};
  static uint64_t constexpr SNAKE_MOVE_PERIOD_MS = 100UL;

  Engine engine;
  bool running;
  bool field[FIELD_WIDTH][FIELD_HEIGHT];
  std::deque<Position> snake;
  Direction snakeDirection;
  Direction pressedDirection;
  uint64_t currentTick;
  uint64_t lastTick;

  // Fonts
  TTF_Font* pFont64;
  TTF_Font* pFont32;

  // Textures
  SDL_Texture* pBensGame;
  SDL_Texture* pStart;
  SDL_Texture* pExit;
  SDL_Texture* pTitleBackground;
  SDL_Texture* pApple;
  SDL_Texture* pGameOver;

  // Sounds
  Mix_Music* pMusic;
  Mix_Chunk* pBiteSound;
  Mix_Chunk* pGameOverSound;
  Mix_Chunk* pSquashSound;

  // Entities
  Entity bensGame;
  Entity start;
  Entity exit;
  Entity titleBackground;
  Entity apple;
  Entity gameOver;

  void Reset(void);
  void AddSnakeHead(Position const fieldpos);
  void RemoveSnakeTail(void);
  void RenderBackground(void);
  void RandomApplePosition(void);
};