#pragma once

#include "Engine.hpp"
#include "Entity.hpp"
#include "Position.hpp"
#include <deque>
#include <string>
#include <array>
#include <cstdint>

typedef struct _Mix_Music Mix_Music;
typedef struct Mix_Chunk Mix_Chunk;

class Game
{
public:
  Game(Position const & res = { 0, 0 });
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

  static int constexpr FIELD_WIDTH = 19;
  static int constexpr FIELD_HEIGHT = 19;
  static double constexpr SCORE_ANGLE = 10.0;
  static uint64_t constexpr SNAKE_MOVE_PERIOD_MS = 100UL;
  static char constexpr HIGHSCORE_PATH[] = "./highscores.txt";

  Engine engine;
  Position resolution;
  State state;
  bool quit;
  uint16_t scoreCount;
  bool field[FIELD_WIDTH][FIELD_HEIGHT];
  Position fieldPosition;
  Position fieldScale;
  Position fieldGridScale;
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
  TTF_Font* pFontStandardSmall;

  // Font Textures
  SDL_Texture* pBensGame;
  SDL_Texture* pStart;
  SDL_Texture* pExit;
  SDL_Texture* pScore;
  SDL_Texture* pHighscores;
  SDL_Texture* pNewHighScore;
  SDL_Texture* pEnterName;
  SDL_Texture* pVersion;

  // Pic Textures
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
  Entity newHighscore;
  Entity enterName;
  Entity version;

  void Restart(void);
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
  void ApplyNewHighscore(void);
  void ApplyStoredHighscores(void);
  Position ConvertFullHd(Position const & fhdPosition);
  int ConvertFullHdWidth(int const fhdWidth);
  int ConvertFullHdHeight(int const fhdHeight);
};