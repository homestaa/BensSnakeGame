#pragma once

#include "Engine.hpp"
#include "Entity.hpp"
#include "Position.hpp"
#include <deque>
#include <string>
#include <array>

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

  enum class State
  {
    Init,
    Running,
    GameOver,
    NewHighscore
  };

  struct HighscoreEntry
  {
    std::string name;
    uint16_t score;
  };

  static int constexpr FIELD_WIDTH = 40;
  static int constexpr FIELD_HEIGHT = 40;
  static Position constexpr FIELD_POSITION = {140, 100};
  static Position constexpr FIELD_SCALE = { Engine::SCREEN_HEIGHT - FIELD_POSITION.y,
                                            Engine::SCREEN_HEIGHT - FIELD_POSITION.y };
  static Position constexpr FIELD_GRID_SCALE = {FIELD_SCALE.x / FIELD_WIDTH, FIELD_SCALE.y / FIELD_HEIGHT};
  static double constexpr SCORE_ANGLE = 10.0;
  static uint64_t constexpr SNAKE_MOVE_PERIOD_MS = 100UL;

  Engine engine;
  State state;
  bool quit;
  uint16_t scoreCount;
  bool field[FIELD_WIDTH][FIELD_HEIGHT];
  std::deque<Position> snake;
  Direction snakeDirection;
  Direction pressedDirection;
  uint64_t currentTick;
  uint64_t lastGameHandleTick;
  uint64_t lastHighScoreHandleTick;
  std::string highscoresStr;
  std::string newHighscoreName;
  std::array<HighscoreEntry, 3> highscoreEntries;

  // Fonts
  TTF_Font* pFontTitle;
  TTF_Font* pFontButton;
  TTF_Font* pFontScore;
  TTF_Font* pFontHighscores;
  TTF_Font* pFontStandard;

  // Textures
  SDL_Texture* pBensGame;
  SDL_Texture* pStart;
  SDL_Texture* pExit;
  SDL_Texture* pScore;
  SDL_Texture* pHighscores;
  SDL_Texture* pTitleBackground;
  SDL_Texture* pApple;
  SDL_Texture* pSnakeHead;
  SDL_Texture* pSnakeHeadDead;
  SDL_Texture* pSnakeSkin;
  SDL_Texture* pGameOver;
  SDL_Texture* pPlane;
  SDL_Texture* pTrophy;

  // Sounds
  Mix_Music* pMusic;
  Mix_Chunk* pBiteSound;
  Mix_Chunk* pPunchSound;
  Mix_Chunk* pHornSound;
  Mix_Chunk* pCheerSound;

  // Entities
  Entity bensGame;
  Entity start;
  Entity exit;
  Entity score;
  Entity titleBackground;
  Entity apple;
  Entity snakeHead;
  Entity gameOver;
  Entity plane;
  Entity highscores;
  Entity trophy;

  void Reset(void);
  void AddSnakeHead(Position const fieldpos);
  void RemoveSnakeTail(void);
  void RenderBackground(void);
  void RandomApplePosition(void);
  void UpdateScoreDisplay(void);
  void HandleEvent(void);
  void HandleGame(void);
  void RenderField(void);
  void Render(void);
  void HandlePlanePosition(void);
  void RenderPlane(void);
  void HandleNewHighscore(void);
  void RenderInputForNewHighscore(void);
  void UpdateHighscoreBanner(void);
  void StoreHighscores();
};