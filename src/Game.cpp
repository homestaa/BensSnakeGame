#include "Game.hpp"
#include "Engine.hpp"
#include "Entity.hpp"
#include "Position.hpp"
#include "version.hpp"
#include <SDL.h>
#include <SDL_mixer.h>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>

static constexpr SDL_Color black = { 0U, 0U, 0U };
static constexpr SDL_Color white = { 255U, 255U, 255U };
static constexpr SDL_Color red = { 180U, 0U, 0U };
static constexpr SDL_Color gold = { 255, 215U, 0U };
static constexpr SDL_Color darkblue = { 0U, 0U, 50U };
static constexpr SDL_Color darkerblue = { 0U, 0U, 40U };

Game::Game(Position const & res)
: engine("Ben's Snake Game", res)
, resolution(engine.GetResolution())
, state(State::Init)
, checkedOnePlayer(true)
, singlePlayer(checkedOnePlayer)
, quit(false)
, scoreCount(0U)
, numberOfMoves(0UL)
, field{ { Field::Free }, { Field::Free } }
, fieldPosition(ConvertFullHd({ 140, 100 }))
, fieldScale(ConvertFullHd({ 980, 980 }))
, fieldGridScale{ fieldScale.x / FIELD_WIDTH, fieldScale.y / FIELD_HEIGHT }
, currentTick(0UL)
, lastGameHandleTick(0UL)
, lastHighScoreHandleTick(0UL)
, highscoresStr()
, newHighscoreName()
, highscoreEntries{ HighscoreEntry{"-", 0}, HighscoreEntry{"-", 0}, HighscoreEntry{"-", 0} }
, players{ Player(engine.CreatePicTexture("./res/gfx/snakeHead0.png"),
                  engine.CreatePicTexture("./res/gfx/snakeHeadDead0.png"),
                  engine.CreatePicTexture("./res/gfx/snakeSkin0.jpg"), fieldGridScale, Field::Snake0),
           Player(engine.CreatePicTexture("./res/gfx/snakeHead1.png"),
                  engine.CreatePicTexture("./res/gfx/snakeHeadDead1.png"),
                  engine.CreatePicTexture("./res/gfx/snakeSkin1.jpg"), fieldGridScale, Field::Snake1) }
, pFontTitle(engine.CreateFont("./res/font/28DaysLater.ttf", 64))
, pFontButton(engine.CreateFont("./res/font/GretoonHighlight.ttf", 28))
, pFontScore(engine.CreateFont("./res/font/TradingPostBold.ttf", 36))
, pFontHighscores(engine.CreateFont("./res/font/FromCartoonBlocks.ttf", 36))
, pFontGameOver2P(engine.CreateFont("./res/font/FromCartoonBlocks.ttf", 100))
, pFontStandard(engine.CreateFont("./res/font/Montserrat.ttf", 36))
, pFontStandardSmall(engine.CreateFont("./res/font/Montserrat.ttf", 24))
, pBensGame(engine.CreateTextTexture("Bens Snake Game", pFontTitle, black))
, pStart(engine.CreateTextTexture("Start", pFontButton, red))
, pOnePlayer(engine.CreateTextTexture(" 1 P", pFontButton, red))
, pTwoPlayer(engine.CreateTextTexture("2 P", pFontButton, red))
, pGameOverTwoPlayers(engine.CreateTextTexture("", pFontGameOver2P, white))
, pExit(engine.CreateTextTexture("Exit", pFontButton, red))
, pScore(engine.CreateTextTexture("x  0", pFontScore, black))
, pHighscores(engine.CreateTextTexture(highscoresStr.c_str(), pFontHighscores, white))
, pNewHighScore(engine.CreateTextTexture("New highscore!", pFontStandard, white))
, pEnterName(engine.CreateTextTexture("Enter name:", pFontStandard, white))
, pVersion(engine.CreateTextTexture(VERSION, pFontStandardSmall, black))
, pTitleBackground(engine.CreatePicTexture("./res/gfx/titleBackground.jpg"))
, pChecked(engine.CreatePicTexture("./res/gfx/checked.png"))
, pArrows(engine.CreatePicTexture("./res/gfx/arrows.png"))
, pWasd(engine.CreatePicTexture("./res/gfx/wasd.png"))
, pApple(engine.CreatePicTexture("./res/gfx/apple.png"))
, pGameOver(engine.CreatePicTexture("./res/gfx/gameOver.png"))
, pPlane(engine.CreatePicTexture("./res/gfx/plane.png"))
, pTrophy(engine.CreatePicTexture("./res/gfx/trophy.png"))
, pMusic(Mix_LoadMUS("./res/sfx/music.mp3"))
, pBiteSound(Mix_LoadWAV("./res/sfx/bite.wav"))
, pPunchSound(Mix_LoadWAV("./res/sfx/punch.mp3"))
, pHornSound(Mix_LoadWAV("./res/sfx/horn.mp3"))
, pCheerSound(Mix_LoadWAV("./res/sfx/cheering.mp3"))
, pSquashSound(Mix_LoadWAV("./res/sfx/squash.mp3"))
, bensGame(pBensGame, ConvertFullHd({ 20, 20 }))
, start(pStart, ConvertFullHd({ 20, 100 }))
, onePlayer(pOnePlayer, ConvertFullHd({ 20, 160 }))
, twoPlayer(pTwoPlayer, ConvertFullHd({ 20, 220 }))
, gameOverTwoPlayers(pGameOverTwoPlayers, fieldPosition + ConvertFullHd({ 180, 400 }))
, exit(pExit, ConvertFullHd({ 20, 320 }))
, score(pScore, ConvertFullHd({ 1700, 712 }), { 0, 0 }, SCORE_ANGLE)
, titleBackground(pTitleBackground, { 0, 0 }, ConvertFullHd({ 1920, 1080 }))
, checked(pChecked, ConvertFullHd(POS_CHECKED_1P), ConvertFullHd({ 80, 80 }))
, arrows(pArrows, ConvertFullHd({ 90, 160 }), ConvertFullHd({ 40, 30 }))
, wasd(pWasd, ConvertFullHd({ 90, 220 }), ConvertFullHd({ 40, 30 }))
, apple(pApple, { 0, 0 }, fieldGridScale)
, gameOver(pGameOver, fieldPosition + ConvertFullHd({ 60, 200 }), ConvertFullHd({ 800, 480 }))
, plane(pPlane, { resolution.x, 0 }, ConvertFullHd({ 219, 102 }))
, highscores(pHighscores)
, trophy(pTrophy, gameOver.GetPosition() + ConvertFullHd({ 20, 50 }), ConvertFullHd({ 220, 242 }))
, newHighscore(pNewHighScore, gameOver.GetPosition() + ConvertFullHd({ 300, 50 }))
, enterName(pEnterName, gameOver.GetPosition() + ConvertFullHd({ 300, 200 }))
, version(pVersion, ConvertFullHd({ 1845, 1050 }))
{
  // use current time as seed for random generator
  std::srand(std::time({}));

  ApplyStoredHighscores();

  Mix_MasterVolume(MIX_MAX_VOLUME);
  Mix_VolumeChunk(pBiteSound, MIX_MAX_VOLUME);
  Mix_VolumeChunk(pPunchSound, MIX_MAX_VOLUME / 2);
  Mix_VolumeChunk(pHornSound, MIX_MAX_VOLUME / 2);
  Mix_VolumeChunk(pCheerSound, MIX_MAX_VOLUME);
  Mix_VolumeChunk(pSquashSound, MIX_MAX_VOLUME);
  Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
  Mix_PlayMusic(pMusic, -1);

  // Adjust font scale relating to resolution
  std::cout << "resolution: " << resolution.x << "x" << resolution.y << "\n";
  bensGame.SetScale(ConvertFullHd(bensGame.GetTextureSize()));
  start.SetScale(ConvertFullHd(start.GetTextureSize()));
  onePlayer.SetScale(ConvertFullHd(onePlayer.GetTextureSize()));
  twoPlayer.SetScale(ConvertFullHd(twoPlayer.GetTextureSize()));
  exit.SetScale(ConvertFullHd(exit.GetTextureSize()));
  score.SetScale(ConvertFullHd(score.GetTextureSize()));
  newHighscore.SetScale(ConvertFullHd(newHighscore.GetTextureSize()));
  enterName.SetScale(ConvertFullHd(enterName.GetTextureSize()));
  version.SetScale(ConvertFullHd(version.GetTextureSize()));
}


Game::~Game(void)
{
  Mix_FreeChunk(pCheerSound);
  Mix_FreeChunk(pHornSound);
  Mix_FreeChunk(pPunchSound);
  Mix_FreeChunk(pBiteSound);
  Mix_FreeMusic(pMusic);

  engine.DestroyTexture(pTrophy);
  engine.DestroyTexture(pPlane);
  engine.DestroyTexture(pGameOver);
  engine.DestroyTexture(pApple);
  engine.DestroyTexture(pChecked);
  engine.DestroyTexture(pTitleBackground);
  engine.DestroyTexture(pVersion);
  engine.DestroyTexture(pEnterName);
  engine.DestroyTexture(pNewHighScore);
  engine.DestroyTexture(pHighscores);
  engine.DestroyTexture(pScore);
  engine.DestroyTexture(pExit);
  engine.DestroyTexture(pTwoPlayer);
  engine.DestroyTexture(pOnePlayer);
  engine.DestroyTexture(pStart);
  engine.DestroyTexture(pBensGame);

  engine.DestroyFont(pFontStandardSmall);
  engine.DestroyFont(pFontStandard);
  engine.DestroyFont(pFontGameOver2P);
  engine.DestroyFont(pFontHighscores);
  engine.DestroyFont(pFontScore);
  engine.DestroyFont(pFontButton);
  engine.DestroyFont(pFontTitle);

  for (Player const & player : players)
  {
    engine.DestroyTexture(player.pSnakeSkin);
    engine.DestroyTexture(player.pSnakeHeadDead);
    engine.DestroyTexture(player.pSnakeHead);
  }
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


void Game::Restart(void)
{
  singlePlayer = checkedOnePlayer;
  memset(field, 0, sizeof(field));
  for (Player & player : players)
  {
    player.snake.clear();
    player.snakeHead.SetTexture(player.pSnakeHead);
    player.snakeDirection = Direction::Up;
    player.pressedDirection = Direction::Up;
  }

  if (singlePlayer)
  {
    // Start with 3 parts sized snake
    AddSnakeHead(players[0], {FIELD_WIDTH / 2, FIELD_HEIGHT - 1});
    AddSnakeHead(players[0], {FIELD_WIDTH / 2, FIELD_HEIGHT - 2});
    AddSnakeHead(players[0], {FIELD_WIDTH / 2, FIELD_HEIGHT - 3});

    RandomApplePosition();
  }
  else
  {
    // Player 1
    AddSnakeHead(players[0], {FIELD_WIDTH / 2 + 2, FIELD_HEIGHT - 1});
    AddSnakeHead(players[0], {FIELD_WIDTH / 2 + 2, FIELD_HEIGHT - 2});
    AddSnakeHead(players[0], {FIELD_WIDTH / 2 + 2, FIELD_HEIGHT - 3});

    // Payer 2
    AddSnakeHead(players[1], {FIELD_WIDTH / 2 - 2, FIELD_HEIGHT - 1});
    AddSnakeHead(players[1], {FIELD_WIDTH / 2 - 2, FIELD_HEIGHT - 2});
    AddSnakeHead(players[1], {FIELD_WIDTH / 2 - 2, FIELD_HEIGHT - 3});
  }

  scoreCount = 0U;
  UpdateScoreDisplay();

  currentTick = SDL_GetPerformanceCounter();
  lastGameHandleTick = currentTick;
  numberOfMoves = 0UL;

  state = State::Running;
  (void)Mix_PlayChannel(-1, pHornSound, 0);
}


void Game::AddSnakeHead(Player & player, Position const fieldpos)
{
  player.snake.push_front(fieldpos);
  player.snakeHead.SetPosition({ fieldPosition.x + fieldpos.x * fieldGridScale.x,
                                 fieldPosition.y + fieldpos.y * fieldGridScale.y });
  field[fieldpos.x][fieldpos.y] = player.fieldMarker;
}


void Game::RemoveSnakeTail(Player & player)
{
  field[player.snake.back().x][player.snake.back().y] = Field::Free;
  player.snake.pop_back();
}


void Game::RenderBackground(void)
{
  engine.Render(titleBackground);
  RenderPlane();
  engine.Render(start);
  engine.Render(checked);
  engine.Render(onePlayer);
  engine.Render(arrows);
  engine.Render(twoPlayer);
  engine.Render(wasd);
  engine.Render(exit);
  engine.Render(bensGame);
  engine.Render(ConvertFullHd({ 1640, 695 }), ConvertFullHd({ 50, 50 }), pApple, SCORE_ANGLE);
  engine.Render(score);
  engine.Render(version);
}


void Game::RandomApplePosition(void)
{
  int xRandom = std::rand() % FIELD_WIDTH;
  int yRandom = std::rand() % FIELD_HEIGHT;
  Position const randomPosition = {xRandom, yRandom};
  while (field[xRandom][yRandom] != Field::Free)
  {
    // Avoid apple position inside snake, so find next free position
    xRandom = (xRandom < FIELD_WIDTH - 1) ? xRandom + 1
                                          : 0;
    yRandom = (xRandom != 0)               ? yRandom
            : (yRandom < FIELD_HEIGHT - 1) ? yRandom + 1
                                           : 0;

    if (randomPosition == Position{xRandom, yRandom})
    {
      // No free position found, snake everywhere
      break;
    }
  }

  apple.SetPosition({ fieldPosition.x + (xRandom * fieldGridScale.x),
                      fieldPosition.y + (yRandom * fieldGridScale.y) });
}


void Game::UpdateScoreDisplay(void)
{
  std::string const scoreString = "x  " + std::to_string(scoreCount);
  engine.DestroyTexture(pScore);
  pScore = engine.CreateTextTexture(scoreString.c_str(), pFontScore, black);
  score.SetTexture(pScore);
  score.SetScale(ConvertFullHd(score.GetTextureSize()));
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
        if ((state == State::NewHighscore) && (!newHighscoreName.empty()))
        {
          ApplyNewHighscore();
        }
        quit = true;
      }
      else if (start.IsOnPosition(mousePos))
      {
        if ((state == State::NewHighscore) && (!newHighscoreName.empty()))
        {
          ApplyNewHighscore();
        }
        Restart();
      }
      else if (onePlayer.IsOnPosition(mousePos))
      {
        if ((state != State::Running) && (checkedOnePlayer == false))
        {
          (void)Mix_PlayChannel(-1, pSquashSound, 0);
          checkedOnePlayer = true;
          checked.SetPosition(POS_CHECKED_1P);
        }
      }
      else if (twoPlayer.IsOnPosition(mousePos))
      {
        if ((state != State::Running) && (checkedOnePlayer == true))
        {
          (void)Mix_PlayChannel(-1, pSquashSound, 0);
          checkedOnePlayer = false;
          checked.SetPosition(POS_CHECKED_2P);
        }
      }
      break;
    }

    case SDL_TEXTINPUT:
      if ((state == State::NewHighscore) && (newHighscoreName.length() < 16U))
      {
        newHighscoreName.append(event.text.text);
      }
      break;

    case SDL_KEYDOWN:
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
            ApplyNewHighscore();
            state = State::GameOver;
          }
          break;

        case SDLK_UP:
          if ((players[0].snakeDirection == Direction::Left) || (players[0].snakeDirection == Direction::Right))
          {
            players[0].pressedDirection = Direction::Up;
          }
          break;

        case SDLK_DOWN:
          if ((players[0].snakeDirection == Direction::Left) || (players[0].snakeDirection == Direction::Right))
          {
            players[0].pressedDirection = Direction::Down;
          }
          break;

        case SDLK_LEFT:
          if ((players[0].snakeDirection == Direction::Up) || (players[0].snakeDirection == Direction::Down))
          {
            players[0].pressedDirection = Direction::Left;
          }
          break;

        case SDLK_RIGHT:
          if ((players[0].snakeDirection == Direction::Up) || (players[0].snakeDirection == Direction::Down))
          {
            players[0].pressedDirection = Direction::Right;
          }
          break;

        case SDLK_w:
          if ((players[1].snakeDirection == Direction::Left) || (players[1].snakeDirection == Direction::Right))
          {
            players[1].pressedDirection = Direction::Up;
          }
          break;

        case SDLK_s:
          if ((players[1].snakeDirection == Direction::Left) || (players[1].snakeDirection == Direction::Right))
          {
            players[1].pressedDirection = Direction::Down;
          }
          break;

        case SDLK_a:
          if ((players[1].snakeDirection == Direction::Up) || (players[1].snakeDirection == Direction::Down))
          {
            players[1].pressedDirection = Direction::Left;
          }
          break;

        case SDLK_d:
          if ((players[1].snakeDirection == Direction::Up) || (players[1].snakeDirection == Direction::Down))
          {
            players[1].pressedDirection = Direction::Right;
          }
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
    ++numberOfMoves;

    // Update and validate new snake's head positions
    std::array<Position, 2> snakeHeadpos = { Position{ 0 }, Position{ 0 } };
    for (size_t i = 0UL; i < players.size(); ++i)
    {
      snakeHeadpos[i] = players[i].snake.front();

      players[i].snakeDirection = players[i].pressedDirection;

      switch (players[i].snakeDirection)
      {
        case Direction::Up:
          snakeHeadpos[i].y -= 1;
          break;

        case Direction::Down:
          snakeHeadpos[i].y += 1;
          break;

        case Direction::Left:
          snakeHeadpos[i].x -= 1;
          break;

        case Direction::Right:
          snakeHeadpos[i].x += 1;
          break;
      }

      if (   (snakeHeadpos[i].x >= FIELD_WIDTH)
          || (snakeHeadpos[i].x < 0)
          || (snakeHeadpos[i].y >= FIELD_HEIGHT)
          || (snakeHeadpos[i].y < 0)
          || (field[snakeHeadpos[i].x][snakeHeadpos[i].y] != Field::Free))
      {
        (void)Mix_PlayChannel(-1, pPunchSound, 0);
        players[i].snakeHead.SetTexture(players[i].pSnakeHeadDead);
        if (snakeHeadpos[0] == snakeHeadpos[1])
        {
          // Kill also player 0 if both players hit themself with their head
          players[0].snakeHead.SetTexture(players[0].pSnakeHeadDead);
        }
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
        AddSnakeHead(players[i], snakeHeadpos[i]);
      }

      if (singlePlayer)
      {
        // Only handle player 0 for single player mode
        break;
      }
    }

    // At least one snake died
    if (state != State::Running)
    {
      // Update game over text for two player game
      engine.DestroyTexture(pGameOverTwoPlayers);
      char const * const pGameOverText =
        (   (players[0].snakeHead.GetTexture() == players[0].pSnakeHead)
         && (players[1].snakeHead.GetTexture() == players[1].pSnakeHeadDead)) ? "Player 1 wins!" :
        (   (players[0].snakeHead.GetTexture() == players[0].pSnakeHeadDead)
         && (players[1].snakeHead.GetTexture() == players[1].pSnakeHead))     ? "Player 2 wins!" :
                                                                                "   Draw Game!";
      pGameOverTwoPlayers = engine.CreateTextTexture(pGameOverText, pFontGameOver2P, white);
      gameOverTwoPlayers.SetTexture(pGameOverTwoPlayers);
      gameOverTwoPlayers.SetScale(ConvertFullHd(gameOverTwoPlayers.GetTextureSize()));

      return;
    }

    // Valid new positions, so handle snake's tail
    for (size_t i = 0UL; i < players.size(); ++i)
    {
      if (   (singlePlayer)
          && (apple.IsOnPosition({ fieldPosition.x + (snakeHeadpos[i].x * fieldGridScale.x) + (fieldGridScale.x / 2),
                                    fieldPosition.y + (snakeHeadpos[i].y * fieldGridScale.y) + (fieldGridScale.x / 2) })))
      {
        // Eat apple
        (void)Mix_PlayChannel(-1, pBiteSound, 0);
        RandomApplePosition();
        ++scoreCount;
        UpdateScoreDisplay();
      }
      else if (singlePlayer || ((numberOfMoves % 3UL) != 0UL))
      {
        RemoveSnakeTail(players[i]);
      }

      if (singlePlayer)
      {
        // Only handle player 0 for single player mode
        break;
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
      if (field[column][line] != Field::Free)
      {
        // Draw Snake
        engine.Render({ fieldPosition.x + (column * fieldGridScale.x),
                        fieldPosition.y + (line * fieldGridScale.y)},
                      fieldGridScale,
                      players[(field[column][line] == Field::Snake0) ? 0 : 1].pSnakeSkin);
      }
      else
      {
        // Draw Grid
        engine.RenderRect({ fieldPosition.x + (column * fieldGridScale.x),
                            fieldPosition.y + (line * fieldGridScale.y)},
                          fieldGridScale,
                          (((line + column) % 2) == 0) ? darkblue : darkerblue);
      }
    }
  }

  engine.Render(players[0].snakeHead);
  if (singlePlayer)
    engine.Render(apple);
  else
   engine.Render(players[1].snakeHead);
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
    if (singlePlayer)
    {
      engine.Render(gameOver);
    }
    else
    {
      engine.Render(gameOverTwoPlayers);
    }
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
                          : resolution.x,
                        plane.GetPosition().y });

    highscores.SetPosition({ plane.GetPosition().x + plane.GetScale().x,
                             plane.GetPosition().y + ConvertFullHdHeight(45) });
  }
}


void Game::RenderPlane(void)
{
  engine.Render(plane);

  SDL_Color constexpr bannerColor = gold;
  Position const bannerPos = { plane.GetPosition().x + plane.GetScale().x,
                               plane.GetPosition().y + ConvertFullHdHeight(26) };
  Position const bannerScale = { highscores.GetScale().x + ConvertFullHdWidth(20),
                                 ConvertFullHdHeight(72) };
  engine.RenderRect(bannerPos, bannerScale, bannerColor);

  std::vector<Position> const bannerTailTop =
  {{
    { bannerPos.x + bannerScale.x,      bannerPos.y },
    { bannerPos.x + bannerScale.x + ConvertFullHdWidth(50), bannerPos.y },
    { bannerPos.x + bannerScale.x,      bannerPos.y + bannerScale.y / 2 },
  }};
  engine.RenderGeometry(bannerTailTop, bannerColor);

  std::vector<Position> const bannerTailBottom =
  {{
    { bannerPos.x + bannerScale.x,      bannerPos.y + bannerScale.y / 2 },
    { bannerPos.x + bannerScale.x + ConvertFullHdWidth(50), bannerPos.y + bannerScale.y },
    { bannerPos.x + bannerScale.x,      bannerPos.y + bannerScale.y },
  }};
  engine.RenderGeometry(bannerTailBottom, bannerColor);

  engine.Render(highscores);
}


void Game::RenderInputForNewHighscore(void)
{
  engine.RenderRect(gameOver.GetPosition() - ConvertFullHd({ 10, 10 }),
                    gameOver.GetScale() + ConvertFullHd({ 20, 20 }),
                    gold);
  engine.RenderRect(gameOver.GetPosition(), gameOver.GetScale(), black);
  engine.Render(trophy);
  engine.Render(newHighscore);
  engine.Render(enterName);

  // Render player input
  SDL_Texture* const pNewHSName(engine.CreateTextTexture(newHighscoreName.c_str(), pFontStandard, gold));
  Entity newHSName(pNewHSName, gameOver.GetPosition() + ConvertFullHd({ 300, 300 }));
  newHSName.SetScale(ConvertFullHd(newHSName.GetTextureSize()));
  engine.Render(newHSName);
  engine.DestroyTexture(pNewHSName);
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
  highscores.SetScale(ConvertFullHd(highscores.GetTextureSize()));
}


void Game::StoreHighscores()
{
  std::ofstream file(HIGHSCORE_PATH);
  for (auto highscoreEntry : highscoreEntries)
  {
    file << highscoreEntry.name << ";" << highscoreEntry.score << "\n";
  }
  file.close();
}

void Game::ApplyNewHighscore(void)
{
  highscoreEntries.back().name = newHighscoreName;
  highscoreEntries.back().score = scoreCount;
  std::sort(highscoreEntries.begin(),
            highscoreEntries.end(),
            [](HighscoreEntry const & a, HighscoreEntry const & b){ return a.score > b.score; });
  StoreHighscores();
  UpdateHighscoreBanner();
}


void Game::ApplyStoredHighscores(void)
{
  std::ifstream file(HIGHSCORE_PATH);
  if (file.is_open())
  {
    for (HighscoreEntry & highscoreEntry : highscoreEntries)
    {
      std::getline(file, highscoreEntry.name, ';');
      std::string score;
      std::getline(file, score, '\n');
      try
      {
        highscoreEntry.score = std::stoul(score);
      }
      catch (...)
      {
        highscoreEntry.score = 0U;
      }
    }
    file.close();
  }
  else
  {
    // No highscores present, so store default highscores
    StoreHighscores();
  }
  UpdateHighscoreBanner();
}


Position Game::ConvertFullHd(Position const & fhdPosition)
{
  return { static_cast<int>(static_cast<float>(resolution.x) / 1920.0 * static_cast<float>(fhdPosition.x)),
           static_cast<int>(static_cast<float>(resolution.y) / 1080.0 * static_cast<float>(fhdPosition.y)) };
}


int Game::ConvertFullHdWidth(int const fhdWidth)
{
  return static_cast<int>(static_cast<float>(resolution.x) / 1920.0 * static_cast<float>(fhdWidth));
}


int Game::ConvertFullHdHeight(int const fhdHeight)
{
  return static_cast<int>(static_cast<float>(resolution.y) / 1080.0 * static_cast<float>(fhdHeight));
}