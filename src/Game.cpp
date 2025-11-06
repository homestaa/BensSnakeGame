#include "Game.hpp"
#include "Engine.hpp"
#include "Position.hpp"
#include <SDL.h>
#include <SDL_events.h>
#include <SDL_keyboard.h>
#include <SDL_keycode.h>
#include <SDL_mixer.h>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

static constexpr SDL_Color black = { 0U, 0U, 0U };
static constexpr SDL_Color white = { 255U, 255U, 255U };
static constexpr SDL_Color red = { 180U, 0U, 0U };
static constexpr SDL_Color gold = { 255, 215U, 0U };
static constexpr SDL_Color darkred = { 50U, 0U, 0U };
static constexpr SDL_Color darkblue = { 0U, 0U, 50U };
static constexpr SDL_Color darkerblue = { 0U, 0U, 40U };

Game::Game(void)
: engine()
, state(State::Init)
, quit(false)
, scoreCount(0U)
, field{ {0}, {0} }
, snake()
, snakeDirection(Direction::Up)
, pressedDirection(Direction::Up)
, currentTick(0UL)
, lastGameHandleTick(0UL)
, lastHighScoreHandleTick(0UL)
, highscoresStr()
, newHighscoreName()
, highscoreEntries{ HighscoreEntry{"-", 0}, HighscoreEntry{"-", 0}, HighscoreEntry{"-", 0} }
, pFontTitle(engine.CreateFont("../res/font/28DaysLater.ttf", 64))
, pFontButton(engine.CreateFont("../res/font/GretoonHighlight.ttf", 28))
, pFontScore(engine.CreateFont("../res/font/TradingPostBold.ttf", 36))
, pFontHighscores(engine.CreateFont("../res/font/FromCartoonBlocks.ttf", 36))
, pFontStandard(engine.CreateFont("../res/font/Montserrat.ttf", 36))
, pBensGame(engine.CreateTextTexture("Bens Snake Game", pFontTitle, black))
, pStart(engine.CreateTextTexture("Start", pFontButton, red))
, pExit(engine.CreateTextTexture("Exit", pFontButton, red))
, pScore(engine.CreateTextTexture("x  0", pFontScore, black))
, pHighscores(engine.CreateTextTexture(highscoresStr.c_str(), pFontHighscores, white))
, pTitleBackground(engine.CreatePicTexture("../res/gfx/titleBackground.jpg"))
, pApple(engine.CreatePicTexture("../res/gfx/apple.png"))
, pSnakeHead(engine.CreatePicTexture("../res/gfx/snakeHead.png"))
, pSnakeHeadDead(engine.CreatePicTexture("../res/gfx/snakeHeadDead.png"))
, pSnakeSkin(engine.CreatePicTexture("../res/gfx/snakeSkin.jpg"))
, pGameOver(engine.CreatePicTexture("../res/gfx/gameOver.png"))
, pPlane(engine.CreatePicTexture("../res/gfx/plane.png"))
, pTrophy(engine.CreatePicTexture("../res/gfx/trophy.png"))
, pMusic(Mix_LoadMUS("../res/sfx/music.mp3"))
, pBiteSound(Mix_LoadWAV("../res/sfx/bite.wav"))
, pPunchSound(Mix_LoadWAV("../res/sfx/punch.mp3"))
, pHornSound(Mix_LoadWAV("../res/sfx/horn.mp3"))
, pCheerSound(Mix_LoadWAV("../res/sfx/cheering.mp3"))
, bensGame(pBensGame, { 20, 20 })
, start(pStart, { 20, 100 })
, exit(pExit, { 20, 160 })
, score(pScore, { 1700, 712 }, { 0, 0 }, SCORE_ANGLE)
, titleBackground(pTitleBackground, { 0, 0 })
, apple(pApple, { 0, 0 }, FIELD_GRID_SCALE)
, snakeHead(pSnakeHead, { 0, 0 }, { FIELD_GRID_SCALE.x, static_cast<int>(FIELD_GRID_SCALE.y * 163.0 / 104.0) })
, gameOver(pGameOver, FIELD_POSITION + Position{ 60, 200 }, { 800, 480 })
, plane(pPlane, { Engine::SCREEN_WIDTH, 0 }, { 219, 102 })
, highscores(pHighscores)
, trophy(pTrophy, gameOver.GetPosition() + Position{ 20, 50 }, { 220, 242 })
{
  // use current time as seed for random generator
  std::srand(std::time({}));

  std::ifstream file("../highscores.txt");
  if (file.is_open())
  {
    for (HighscoreEntry & highscoreEntry : highscoreEntries)
    {
      std::getline(file, highscoreEntry.name, ';');
      std::string score;
      std::getline(file, score, '\n');
      highscoreEntry.score = std::stoul(score);
    }
    file.close();
  }
  else
  {
    // No highscores present, so store default highscores
    StoreHighscores();
  }
  UpdateHighscoreBanner();

  Mix_MasterVolume(MIX_MAX_VOLUME);
  Mix_VolumeChunk(pBiteSound, MIX_MAX_VOLUME);
  Mix_VolumeChunk(pPunchSound, MIX_MAX_VOLUME / 2);
  Mix_VolumeChunk(pHornSound, MIX_MAX_VOLUME / 2);
  Mix_VolumeChunk(pCheerSound, MIX_MAX_VOLUME);
  Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
  Mix_PlayMusic(pMusic, -1);
}


Game::~Game(void)
{
  Mix_FreeChunk(pCheerSound);
  Mix_FreeChunk(pHornSound);
  Mix_FreeChunk(pPunchSound);
  Mix_FreeChunk(pBiteSound);
  Mix_FreeMusic(pMusic);

  engine.DestroyTexture(pGameOver);
  engine.DestroyTexture(pSnakeSkin);
  engine.DestroyTexture(pSnakeHeadDead);
  engine.DestroyTexture(pSnakeHead);
  engine.DestroyTexture(pApple);
  engine.DestroyTexture(pTitleBackground);
  engine.DestroyTexture(pScore);
  engine.DestroyTexture(pExit);
  engine.DestroyTexture(pStart);
  engine.DestroyTexture(pBensGame);

  engine.DestroyFont(pFontScore);
  engine.DestroyFont(pFontButton);
  engine.DestroyFont(pFontTitle);
}


void Game::Run(void)
{
  while (!quit)
  {
    currentTick = SDL_GetPerformanceCounter();

    HandlePlanePosition();

    HandleEvent();

    HandleGame();

    Render();

    engine.UpdateScreen();

    // Just to relax the cpu
    SDL_Delay(1UL);
  }
}


void Game::Reset(void)
{
  memset(field, 0, sizeof(field));
  snake.clear();
  snakeHead.SetTexture(pSnakeHead);
  snakeDirection = Direction::Up;
  pressedDirection = Direction::Up;
  scoreCount = 0U;
  UpdateScoreDisplay();

  // Start with 3 parts sized snake
  AddSnakeHead({FIELD_WIDTH / 2, FIELD_HEIGHT - 1});
  AddSnakeHead({FIELD_WIDTH / 2, FIELD_HEIGHT - 2});
  AddSnakeHead({FIELD_WIDTH / 2, FIELD_HEIGHT - 3});

  RandomApplePosition();

  currentTick = SDL_GetPerformanceCounter();
  lastGameHandleTick = currentTick;
}


void Game::AddSnakeHead(Position const fieldpos)
{
  snake.push_front(fieldpos);
  snakeHead.SetPosition({ FIELD_POSITION.x + fieldpos.x * FIELD_GRID_SCALE.x,
                          FIELD_POSITION.y + fieldpos.y * FIELD_GRID_SCALE.y });
  field[fieldpos.x][fieldpos.y] = true;
}


void Game::RemoveSnakeTail(void)
{
  field[snake.back().x][snake.back().y] = false;
  snake.pop_back();
}


void Game::RenderBackground(void)
{
  engine.Render(titleBackground);
  RenderPlane();
  engine.Render(start);
  engine.Render(exit);
  engine.Render(bensGame);
  engine.Render({ 1640, 695 }, { 50, 50 }, pApple, SCORE_ANGLE);
  engine.Render(score);
}


void Game::RandomApplePosition(void)
{
  int const xRandom = std::rand() % FIELD_WIDTH;
  int const yRandom = std::rand() % FIELD_HEIGHT;
  apple.SetPosition({ FIELD_POSITION.x + (xRandom * FIELD_GRID_SCALE.x),
                      FIELD_POSITION.y + (yRandom * FIELD_GRID_SCALE.y) });
}


void Game::UpdateScoreDisplay(void)
{
  std::string const scoreString = "x  " + std::to_string(scoreCount);
  engine.DestroyTexture(pScore);
  pScore = engine.CreateTextTexture(scoreString.c_str(), pFontScore, black);
  score.SetTexture(pScore);
  score.SetScale(score.GetTextureSize());
}


void Game::HandleEvent(void)
{
  SDL_Event event;
  SDL_PollEvent(&event);
  switch (event.type)
  {
    case SDL_QUIT:
      quit = true;
      break;

    case SDL_MOUSEBUTTONDOWN:
    {
      Position const mousePos = {
        .x = event.button.x,
        .y = event.button.y
      };
      if (exit.IsOnPosition(mousePos))
      {
        quit = true;
        break;
      }
      if (start.IsOnPosition(mousePos))
      {
        (void)Mix_PlayChannel(-1, pHornSound, 0);
        Reset();
        state = State::Running;
        break;
      }
    }

    case SDL_TEXTINPUT:
      if ((state == State::NewHighscore) && (newHighscoreName.length() < 16U))
      {
        newHighscoreName.append(event.text.text);
      }
      break;

    case SDL_KEYDOWN:
      std::cout << event.key.keysym.sym << "\n";
      switch (event.key.keysym.sym)
      {
        case SDLK_BACKSPACE:
          if ((state == State::NewHighscore) && (!newHighscoreName.empty()))
          {
            newHighscoreName.pop_back();
          }
          break;

        case SDLK_RETURN:
          if ((state == State::NewHighscore) && (!newHighscoreName.empty()))
          {
            highscoreEntries.back().name = newHighscoreName;
            highscoreEntries.back().score = scoreCount;
            std::sort(highscoreEntries.begin(),
                      highscoreEntries.end(),
                      [](HighscoreEntry const & a, HighscoreEntry const & b){ return a.score > b.score; });
            StoreHighscores();
            UpdateHighscoreBanner();
            state = State::GameOver;
          }
          break;

        case SDLK_UP:
          pressedDirection = Direction::Up;
          break;

        case SDLK_DOWN:
          pressedDirection = Direction::Down;
          break;

        case SDLK_LEFT:
          pressedDirection = Direction::Left;
          break;

        case SDLK_RIGHT:
          pressedDirection = Direction::Right;
          break;

        default:
          break;
      }
      break;

    default:
      break;
  }
}


void Game::HandleGame(void)
{
  if (state != State::Running)
  {
    return;
  }

  uint64_t const deltaTime_ms = ((currentTick - lastGameHandleTick) * 1000UL) / SDL_GetPerformanceFrequency();
  if (deltaTime_ms >= SNAKE_MOVE_PERIOD_MS)
  {
    lastGameHandleTick = currentTick;
    Position snakeHeadpos = snake.front();

    if (   (   ((snakeDirection == Direction::Left) || (snakeDirection == Direction::Right))
            && ((pressedDirection == Direction::Up) || (pressedDirection == Direction::Down)))
        || (   ((snakeDirection == Direction::Up) || (snakeDirection == Direction::Down))
            && ((pressedDirection == Direction::Left) || (pressedDirection == Direction::Right))))
    {
      // Pressed direction only valid, when orthogonal
      snakeDirection = pressedDirection;
    }

    switch (snakeDirection)
    {
      case Direction::Up:
        snakeHeadpos.y -= 1;
        break;

      case Direction::Down:
        snakeHeadpos.y += 1;
        break;

      case Direction::Left:
        snakeHeadpos.x -= 1;
        break;

      case Direction::Right:
        snakeHeadpos.x += 1;
        break;
    }

    if (   (snakeHeadpos.x >= FIELD_WIDTH)
        || (snakeHeadpos.x < 0)
        || (snakeHeadpos.y >= FIELD_HEIGHT)
        || (snakeHeadpos.y < 0)
        || (field[snakeHeadpos.x][snakeHeadpos.y] == true))
    {
      (void)Mix_PlayChannel(-1, pPunchSound, 0);
      snakeHead.SetTexture(pSnakeHeadDead);
      if (scoreCount > highscoreEntries.back().score)
      {
        newHighscoreName.clear();
        state = State::NewHighscore;
        (void)Mix_PlayChannel(-1, pCheerSound, 0);
      }
      else
      {
        state = State::GameOver;
      }
    }
    else
    {
      AddSnakeHead(snakeHeadpos);
      if (apple.IsOnPosition({ FIELD_POSITION.x + (snakeHeadpos.x * FIELD_GRID_SCALE.x) + (FIELD_GRID_SCALE.x / 2),
                               FIELD_POSITION.y + (snakeHeadpos.y * FIELD_GRID_SCALE.y) + (FIELD_GRID_SCALE.x / 2),}))
      {
        // Eat apple
        (void)Mix_PlayChannel(-1, pBiteSound, 0);
        RandomApplePosition();
        ++scoreCount;
        UpdateScoreDisplay();
      }
      else
      {
        RemoveSnakeTail();
      }
    }
  }
}


void Game::RenderField(void)
{
  for (int line = 0; line < FIELD_WIDTH; ++line)
  {
    for (int column = 0; column < FIELD_HEIGHT; ++column)
    {
      if (field[column][line] == true)
      {
        // Draw Snake
        engine.Render({ FIELD_POSITION.x + (column * FIELD_GRID_SCALE.x),
                        FIELD_POSITION.y + (line * FIELD_GRID_SCALE.y)},
                      FIELD_GRID_SCALE,
                      pSnakeSkin);
      }
      else
      {
        // Draw Grid
        engine.RenderRect({ FIELD_POSITION.x + (column * FIELD_GRID_SCALE.x),
                            FIELD_POSITION.y + (line * FIELD_GRID_SCALE.y)},
                          FIELD_GRID_SCALE,
                          (((line + column) % 2) == 0) ? darkblue : darkerblue);
      }
    }
  }

  engine.Render(snakeHead);
  engine.Render(apple);
}


void Game::Render(void)
{
  RenderBackground();

  if (state != State::Init)
  {
    RenderField();
  }

  if (state == State::GameOver)
  {
    engine.Render(gameOver);
  }

  if (state == State::NewHighscore)
  {
    RenderInputForNewHighscore();
  }
}


void Game::HandlePlanePosition(void)
{
  uint64_t const deltaTime_ms = ((currentTick - lastHighScoreHandleTick) * 1000UL) / SDL_GetPerformanceFrequency();
  if (deltaTime_ms >= 10UL)
  {
    lastHighScoreHandleTick = currentTick;

    plane.SetPosition({ (plane.GetPosition().x >= -plane.GetScale().x - highscores.GetScale().x - 500)
                          ? plane.GetPosition().x - 1
                          : Engine::SCREEN_WIDTH,
                        plane.GetPosition().y });

    highscores.SetPosition({ plane.GetPosition().x + plane.GetScale().x, plane.GetPosition().y + 45 });
  }
}


void Game::RenderPlane(void)
{
  engine.Render(plane);

  engine.RenderRect({ plane.GetPosition().x + plane.GetScale().x, plane.GetPosition().y + 26 },
                    { highscores.GetScale().x + 20, 72 },
                    gold);

  engine.Render(highscores);
}


void Game::RenderInputForNewHighscore(void)
{
  engine.RenderRect(gameOver.GetPosition() - Position{ 10, 10 }, gameOver.GetScale() + Position{ 20, 20 }, gold);
  engine.RenderRect(gameOver.GetPosition(), gameOver.GetScale(), black);
  engine.Render(trophy);
  engine.RenderText(gameOver.GetPosition() + Position{300,50}, "New highscore!", pFontStandard, white);
  engine.RenderText(gameOver.GetPosition() + Position{300,200}, "Enter name:", pFontStandard, white);
  engine.RenderText(gameOver.GetPosition() + Position{300,300}, newHighscoreName.c_str(), pFontStandard, gold);
}


void Game::UpdateHighscoreBanner(void)
{
  engine.DestroyTexture(pHighscores);
  highscoresStr = "Highscores:  ";
  for (uint8_t i = 0U; i < highscoreEntries.size(); ++i)
  {
    highscoresStr.append("#");
    highscoresStr.append(std::to_string(i + 1U));
    highscoresStr.append("  ");
    highscoresStr.append(highscoreEntries[i].name);
    highscoresStr.append(" (");
    highscoresStr.append(std::to_string(highscoreEntries[i].score));
    highscoresStr.append(")    ");
  }
  pHighscores = engine.CreateTextTexture(highscoresStr.c_str(), pFontHighscores, red);
  highscores.SetTexture(pHighscores);
  highscores.SetScale(highscores.GetTextureSize());
}


void Game::StoreHighscores()
{
  std::ofstream file("../highscores.txt");
  for (auto highscoreEntry : highscoreEntries)
  {
    file << highscoreEntry.name << ";" << highscoreEntry.score << "\n";
  }
}