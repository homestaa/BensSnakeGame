#pragma once

#include "Position.hpp"
#include <vector>

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Color SDL_Color;
typedef struct _TTF_Font TTF_Font;

class Entity;

class Engine
{
public:
  Engine(char const * const pWindowName = "", Position const & res = { 0, 0 });
  ~Engine(void);

  SDL_Texture* CreatePicTexture(char const * const pFile);
  SDL_Texture* CreateTextTexture(char const * const pText, TTF_Font* const font, SDL_Color const textColor);
  void DestroyTexture(SDL_Texture* const pTexture);
  TTF_Font* CreateFont(char const * const pFile, int const size);
  void DestroyFont(TTF_Font* pFont);
  void Clean(SDL_Color const & color);
  void Render(Entity const & entity);
  void Render(Position const & position, Position const & scale, SDL_Texture* const pTexture, double const angle = 0.0);
  void RenderText(Position const & position,
                  char const * const pText,
                  TTF_Font* const pFont,
                  SDL_Color const & textColor);
  void RenderRect(Position const position, Position const scale, SDL_Color const & color);
  void RenderGeometry(std::vector<Position> const & positions, SDL_Color const & color);
  void UpdateScreen(void);
  Position GetResolution(void) const;

private:
  SDL_Window* pWindow;
  SDL_Renderer* pRenderer;
  Position resolution;
};