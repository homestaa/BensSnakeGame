#include "Game.hpp"
#include "Engine.hpp"
#include "Position.hpp"
#include <SDL.h>
#include <SDL_events.h>
#include <SDL_mixer.h>
#include <ctime>
#include <iostream>

static constexpr SDL_Color green = { 0U, 255U, 0U };
static constexpr SDL_Color black = { 0U, 0U, 0U };
static constexpr SDL_Color red = { 255U, 0U, 0U };
static constexpr SDL_Color darkblue = { 0U, 0U, 50U };
static constexpr SDL_Color darkerblue = { 0U, 0U, 40U };

Game::Game(void)
: engine()
, running(false)
, field{ {0}, {0} }
, snake()
, snakeDirection(Direction::Up)
, pressedDirection(Direction::Up)
, currentTick(0UL)
, lastTick(0UL)
, pFont64(engine.CreateFont("../res/font/28DaysLater.ttf", 64))
, pFont32(engine.CreateFont("../res/font/28DaysLater.ttf", 32))
, pBensGame(engine.CreateTextTexture("Bens Snake Game", pFont64, black))
, pStart(engine.CreateTextTexture("Start", pFont32, red))
, pExit(engine.CreateTextTexture("Exit", pFont32, red))
, pTitleBackground(engine.CreatePicTexture("../res/gfx/titleBackground.jpg"))
, pApple(engine.CreatePicTexture("../res/gfx/apple.png"))
, pSnakeHead(engine.CreatePicTexture("../res/gfx/snakeHead.png"))
, pGameOver(engine.CreatePicTexture("../res/gfx/gameOver.png"))
, pMusic(Mix_LoadMUS("../res/sfx/music.mp3"))
, pBiteSound(Mix_LoadWAV("../res/sfx/bite.wav"))
, pGameOverSound(Mix_LoadWAV("../res/sfx/gameOver.wav"))
, pSquashSound(Mix_LoadWAV("../res/sfx/squash.wav"))
, bensGame(pBensGame, { 50, 50 })
, start(pStart, { 50, 200 })
, exit(pExit, { 50, 300 })
, titleBackground(pTitleBackground, { 0, 0 })
, apple(pApple, { 0, 0 }, FIELD_GRID_SCALE)
, snakeHead(pSnakeHead, { 0, 0 }, { FIELD_GRID_SCALE.x, static_cast<int>(FIELD_GRID_SCALE.y * 163.0 / 104.0) })
, gameOver(pGameOver, { FIELD_POSITION.x, FIELD_POSITION.y + 120 }, { 800, 480 })
{
  // use current time as seed for random generator
  std::srand(std::time({}));

  RenderBackground();
  engine.UpdateScreen();

  Mix_MasterVolume(MIX_MAX_VOLUME);
  Mix_VolumeChunk(pBiteSound, MIX_MAX_VOLUME);
  Mix_VolumeChunk(pGameOverSound, MIX_MAX_VOLUME / 2);
  Mix_VolumeChunk(pSquashSound, MIX_MAX_VOLUME);
  Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
  Mix_PlayMusic(pMusic, -1);
}


Game::~Game(void)
{
  Mix_FreeChunk(pSquashSound);
  Mix_FreeChunk(pGameOverSound);
  Mix_FreeChunk(pBiteSound);
  Mix_FreeMusic(pMusic);

  engine.DestroyTexture(pGameOver);
  engine.DestroyTexture(pSnakeHead);
  engine.DestroyTexture(pApple);
  engine.DestroyTexture(pTitleBackground);
  engine.DestroyTexture(pExit);
  engine.DestroyTexture(pStart);
  engine.DestroyTexture(pBensGame);

  engine.DestroyFont(pFont32);
  engine.DestroyFont(pFont64);
}


void Game::Run(void)
{
  bool quit = false;
  while (!quit)
  {
    // Just to relax the cpu
    SDL_Delay(1UL);

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
          Reset();
          running = true;
          break;
        }
      }

      case SDL_KEYDOWN:
        switch (event.key.keysym.sym)
        {
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

    if (running)
    {
      currentTick = SDL_GetPerformanceCounter();
      uint64_t const deltaTime_ms = ((currentTick - lastTick) * 1000UL) / SDL_GetPerformanceFrequency();
      if (deltaTime_ms >= SNAKE_MOVE_PERIOD_MS)
      {
        lastTick = currentTick;
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
          (void)Mix_PlayChannel(-1, pSquashSound, 0);
          running = false;
        }
        else
        {
          AddSnakeHead(snakeHeadpos);
          if (apple.IsOnPosition({ FIELD_POSITION.x + (snakeHeadpos.x * FIELD_GRID_SCALE.x) + (FIELD_GRID_SCALE.x / 2),
                                   FIELD_POSITION.y + (snakeHeadpos.y * FIELD_GRID_SCALE.y) + (FIELD_GRID_SCALE.x / 2),}))
          {
            (void)Mix_PlayChannel(-1, pBiteSound, 0);
            RandomApplePosition();
          }
          else
          {
            RemoveSnakeTail();
          }
        }
      }

      RenderBackground();

      // Draw field
      for (int line = 0; line < FIELD_WIDTH; ++line)
      {
        for (int column = 0; column < FIELD_HEIGHT; ++column)
        {
          engine.RenderRect({ FIELD_POSITION.x + (column * FIELD_GRID_SCALE.x),
                              FIELD_POSITION.y + (line * FIELD_GRID_SCALE.y)},
                            FIELD_GRID_SCALE,
                            (field[column][line] == true)    ? green  // Snake
                              : (((line + column) % 2) == 0) ? darkblue
                              :                                darkerblue);
        }
      }

      engine.Render(snakeHead);
      engine.Render(apple);

      if (!running)
      {
        engine.Render(gameOver);
      }

      engine.UpdateScreen();
    }
  }
}


void Game::Reset(void)
{
  memset(field, 0, sizeof(field));
  snake.clear();
  snakeDirection = Direction::Up;
  pressedDirection = Direction::Up;

  // Start with 3 parts sized snake
  AddSnakeHead({FIELD_WIDTH / 2, FIELD_HEIGHT - 1});
  AddSnakeHead({FIELD_WIDTH / 2, FIELD_HEIGHT - 2});
  AddSnakeHead({FIELD_WIDTH / 2, FIELD_HEIGHT - 3});

  RandomApplePosition();

  currentTick = SDL_GetPerformanceCounter();
  lastTick = currentTick;
}


void Game::AddSnakeHead(Position const fieldpos)
{
  snake.push_front(fieldpos);
  snakeHead.SetPosition(FIELD_POSITION.x + fieldpos.x * FIELD_GRID_SCALE.x,
                        FIELD_POSITION.y + fieldpos.y * FIELD_GRID_SCALE.y);
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
  engine.Render(start);
  engine.Render(exit);
  engine.Render(bensGame);
}


void Game::RandomApplePosition(void)
{
  int const xRandom = std::rand() % FIELD_WIDTH;
  int const yRandom = std::rand() % FIELD_HEIGHT;
  apple.SetPosition(FIELD_POSITION.x + (xRandom * FIELD_GRID_SCALE.x),
                    FIELD_POSITION.y + (yRandom * FIELD_GRID_SCALE.y));
}