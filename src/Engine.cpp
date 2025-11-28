#include "Engine.hpp"
#include "Entity.hpp"
#include "Position.hpp"

#include <SDL_error.h>
#include <SDL_image.h>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_video.h>
#include <stdexcept>

Engine::Engine(char const * const pWindowName, Position const & res)
: pWindow(nullptr)
, pRenderer(nullptr)
, resolution(res)
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
  pWindow= SDL_CreateWindow(pWindowName,
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            resolution.x,
                            resolution.y,
                            (resolution == Position{ 0, 0 }) ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
  if (pWindow == nullptr)
    throw std::runtime_error("Game::Game: Window could not be created.");

  if (resolution == Position{ 0, 0 })
  {
    SDL_DisplayMode mode;
    SDL_GetCurrentDisplayMode(0, &mode);
    resolution = Position{ mode.w, mode.h };
  }

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

  // Quit SDL_ttf
  TTF_Quit();

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


SDL_Texture* Engine::CreateTextTexture(char const * const pText, TTF_Font* const font, SDL_Color const textColor)
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

  SDL_RenderCopyEx(pRenderer, entity.GetTexture(), &src, &dst, entity.GetAngle(), nullptr, SDL_FLIP_NONE);
}


void Engine::Render(Position const & position, Position const & scale, SDL_Texture* const pTexture, double angle)
{
  SDL_Rect src = { 0 };
  SDL_QueryTexture(pTexture, nullptr, nullptr, &src.w, &src.h);

  SDL_Rect dst = {
    .x = position.x,
    .y = position.y,
    .w = scale.x,
    .h = scale.y
  };

  SDL_RenderCopyEx(pRenderer, pTexture, &src, &dst, angle, nullptr, SDL_FLIP_NONE);
}


void Engine::RenderText(Position const & position,
                        char const * const pText,
                        TTF_Font* const pFont,
                        SDL_Color const & textColor)
{
  SDL_Texture* pTexture = CreateTextTexture(pText, pFont, textColor);
  Position scale = { 0 };
  SDL_QueryTexture(pTexture, nullptr, nullptr, &scale.x, &scale.y);
  Render(position, scale, pTexture);
  SDL_DestroyTexture(pTexture);
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


void Engine::RenderGeometry(std::vector<Position> const & positions, SDL_Color const & color)
{
  std::vector<SDL_Vertex> verts(positions.size());
  for (Position const & position : positions)
  {
    verts.push_back(SDL_Vertex{ SDL_FPoint{ static_cast<float>(position.x), static_cast<float>(position.y) },
                                color,
                                SDL_FPoint{ 0 } });
  }

  SDL_RenderGeometry( pRenderer, nullptr, verts.data(), verts.size(), nullptr, 0 );
}


void Engine::UpdateScreen(void)
{
  SDL_RenderPresent(pRenderer);
}


Position Engine::GetResolution(void) const
{
  return resolution;
}