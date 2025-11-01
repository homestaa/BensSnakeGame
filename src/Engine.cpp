#include "Engine.hpp"
#include "Position.hpp"
#include "Entity.hpp"

#include <SDL_error.h>
#include <SDL_image.h>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <stdexcept>

Engine::Engine(void)
: pWindow(nullptr)
, pRenderer(nullptr)
{
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    throw std::runtime_error("Game::Game: SDL could not be initialized.");

#if defined linux && SDL_VERSION_ATLEAST(2, 0, 8)
  // Disable compositor bypass
  if (!SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0"))
    throw std::runtime_error("Game::Game: SDL can not disable compositor bypass.");
#endif

  if (TTF_Init() < 0)
    throw std::runtime_error("Game::Game: TTF could not be initialized.");

  // Create window
  pWindow= SDL_CreateWindow("Ben's Game",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            SCREEN_WIDTH,
                            SCREEN_HEIGHT,
                            SDL_WINDOW_SHOWN);
  if (pWindow == nullptr)
    throw std::runtime_error("Game::Game: Window could not be created.");

  if (SDL_SetWindowFullscreen(pWindow, SDL_WINDOW_FULLSCREEN) < 0)
    throw std::runtime_error("Game::Game: Window Fullscreen failed.");

  // Create renderer
  pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED);
  if (pRenderer == nullptr)
    throw std::runtime_error("Game::Game: Renderer could not be created.");

  if (Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    throw std::runtime_error("Game::Game: Audio could not be opened.");
}


Engine::~Engine(void)
{
  // Destroy renderer
  SDL_DestroyRenderer(pRenderer);

  // Destroy window
  SDL_DestroyWindow(pWindow);

  // Quit SDL
  SDL_Quit();
}


SDL_Texture* Engine::CreatePicTexture(char const * const pFile)
{
  SDL_Texture* pTexture = IMG_LoadTexture(pRenderer, pFile);
  if (pTexture == nullptr)
    throw std::runtime_error("Engine::CreateTexture: Failed to load.");

  return pTexture;
}


SDL_Texture* Engine::CreateTextTexture(char const * const pText, TTF_Font* font, SDL_Color textColor)
{
  SDL_Surface* pSurface = TTF_RenderText_Blended( font, pText, textColor);
  SDL_Texture* pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
  SDL_FreeSurface(pSurface);
  return pTexture;
}


void Engine::DestroyTexture(SDL_Texture* pTexture)
{
  SDL_DestroyTexture(pTexture);
}


TTF_Font* Engine::CreateFont(char const * const pFile, int const size)
{
  TTF_Font* pFont = TTF_OpenFont(pFile, size);
  if (pFont == nullptr)
  {
    std::cout << "SDL-Error: " << SDL_GetError() << "\n";
    throw std::runtime_error("Engine::CreateFont: Failed to load.");
  }

  return pFont;
}


void Engine::DestroyFont(TTF_Font* pFont)
{
  TTF_CloseFont(pFont);
}


void Engine::Clean(SDL_Color const & color)
{
  // Initialize renderer color blue for the background
  SDL_SetRenderDrawColor(pRenderer, color.r, color.g, color.b, 255U);
  // Clear screen
  SDL_RenderClear(pRenderer);
}


void Engine::Render(Entity const & entity)
{
  SDL_Rect src = {
    .x = 0,
    .y = 0,
    .w = entity.GetTextureSize().x,
    .h = entity.GetTextureSize().y
   };

  SDL_Rect dst = {
    .x = entity.GetPosition().x,
    .y = entity.GetPosition().y,
    .w = entity.GetScale().x,
    .h = entity.GetScale().y
  };

  SDL_RenderCopy(pRenderer, entity.GetTexture(), &src, &dst);
}


void Engine::Render(Position const & position, Position const & scale, SDL_Texture* const pTexture)
{
  SDL_Rect src = { 0 };
  SDL_QueryTexture(pTexture, nullptr, nullptr, &src.w, &src.h);

  SDL_Rect dst = {
    .x = position.x,
    .y = position.y,
    .w = scale.x,
    .h = scale.y
  };

  SDL_RenderCopy(pRenderer, pTexture, &src, &dst);
}


void Engine::RenderText(char const * const pText, TTF_Font* font, SDL_Color textColor, Position const & position)
{
  SDL_Surface* surfaceMessage = TTF_RenderText_Blended( font, pText, textColor);
  SDL_Texture* message = SDL_CreateTextureFromSurface(pRenderer, surfaceMessage);

  SDL_Rect const src = {
    .x = 0,
    .y = 0,
    .w = surfaceMessage->w,
    .h = surfaceMessage->h
  };

  SDL_Rect const dst = {
    .x = position.x,
    .y = position.y,
    .w = src.w,
    .h = src.h
  };

  SDL_RenderCopy(pRenderer, message, &src, &dst);
  SDL_FreeSurface(surfaceMessage);
  SDL_DestroyTexture(message);
}


void Engine::RenderRect(Position const position, Position const scale, SDL_Color const & color)
{
  SDL_SetRenderDrawColor(pRenderer, color.r, color.g, color.b, 255U);
  SDL_Rect const rectangle = {
    .x = position.x,
    .y = position.y,
    .w = scale.x,
    .h = scale.y
  };
  SDL_RenderFillRect(pRenderer, &rectangle);
}


void Engine::UpdateScreen(void)
{
  SDL_RenderPresent(pRenderer);
}