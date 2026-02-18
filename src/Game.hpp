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

  enum class Field
  {
    Free,
    Snake0,
    Snake1
  };

  struct HighscoreEntry
  {
    std::string name;
    uint16_t score;
  };

  struct Player
  {
    Player(SDL_Texture* const pSnakeHead, SDL_Texture* const pSnakeHeadDead, SDL_Texture* const pSnakeSkin,
           Position const & fieldGridScale, Field const fieldMarker)
    : snake()
    , snakeDirection(Direction::Up)
    , pressedDirection(Direction::Up)
    , pSnakeHead(pSnakeHead)
    , pSnakeHeadDead(pSnakeHeadDead)
    , pSnakeSkin(pSnakeSkin)
    , snakeHead(pSnakeHead, { 0, 0 }, { fieldGridScale.x, static_cast<int>(fieldGridScale.y * 163.0 / 104.0) })
    , fieldMarker(fieldMarker)
    {
    }

    std::deque<Position> snake;
    Direction snakeDirection;
    Direction pressedDirection;
    SDL_Texture* pSnakeHead;
    SDL_Texture* pSnakeHeadDead;
    SDL_Texture* pSnakeSkin;
    Entity snakeHead;
    Field fieldMarker;
  };

  static int constexpr FIELD_WIDTH = 19;
  static int constexpr FIELD_HEIGHT = 19;
  static double constexpr SCORE_ANGLE = 10.0;
  static uint64_t constexpr SNAKE_MOVE_PERIOD_MS = 100UL;
  static char constexpr HIGHSCORE_PATH[] = "./highscores.txt";
  static Position constexpr POS_CHECKED_1P = { 25, 140 };
  static Position constexpr POS_CHECKED_2P = { 25, 200 };

  Engine engine;
  Position resolution;
  State state;
  bool checkedOnePlayer;
  bool singlePlayer;
  bool quit;
  uint16_t scoreCount;
  size_t numberOfMoves;
  Field field[FIELD_WIDTH][FIELD_HEIGHT];
  Position fieldPosition;
  Position fieldScale;
  Position fieldGridScale;

  uint64_t currentTick;
  uint64_t lastGameHandleTick;
  uint64_t lastHighScoreHandleTick;
  std::string highscoresStr;
  std::string newHighscoreName;
  std::array<HighscoreEntry, 3> highscoreEntries;

  std::array<Player, 2> players;

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
  SDL_Texture* pOnePlayer;
  SDL_Texture* pTwoPlayer;
  SDL_Texture* pExit;
  SDL_Texture* pScore;
  SDL_Texture* pHighscores;
  SDL_Texture* pNewHighScore;
  SDL_Texture* pEnterName;
  SDL_Texture* pVersion;

  // Pic Textures
  SDL_Texture* pTitleBackground;
  SDL_Texture* pChecked;
  SDL_Texture* pArrows;
  SDL_Texture* pWasd;
  SDL_Texture* pApple;
  SDL_Texture* pGameOver;
  SDL_Texture* pPlane;
  SDL_Texture* pTrophy;

  // Sounds
  Mix_Music* pMusic;
  Mix_Chunk* pBiteSound;
  Mix_Chunk* pPunchSound;
  Mix_Chunk* pHornSound;
  Mix_Chunk* pCheerSound;
  Mix_Chunk* pSquashSound;

  // Entities
  Entity bensGame;
  Entity start;
  Entity onePlayer;
  Entity twoPlayer;
  Entity exit;
  Entity score;
  Entity titleBackground;
  Entity checked;
  Entity arrows;
  Entity wasd;
  Entity apple;
  Entity gameOver;
  Entity plane;
  Entity highscores;
  Entity trophy;
  Entity newHighscore;
  Entity enterName;
  Entity version;

  void Restart(void);
  void AddSnakeHead(Player & player, Position const fieldpos);
  void RemoveSnakeTail(Player & player);
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