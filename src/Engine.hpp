#pragma once

#include <SDL.h>

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct _TTF_Font TTF_Font;

class Position;
class Entity;

class Engine
{
public:
  Engine(void);
  ~Engine(void);

  static int constexpr SCREEN_WIDTH = 1920;
  static int constexpr SCREEN_HEIGHT = 1080;

  SDL_Texture* CreatePicTexture(char const * const pFile);
  SDL_Texture* CreateTextTexture(char const * const pText, TTF_Font* font, SDL_Color textColor);
  void DestroyTexture(SDL_Texture* const pTexture);
  TTF_Font* CreateFont(char const * const pFile, int const size);
  void DestroyFont(TTF_Font* pFont);
  void Clean(SDL_Color const & color);
  void Render(Entity const & entity);
  void RenderText(char const * const pText, TTF_Font* font, SDL_Color textColor, Position const & position);
  void RenderRect(Position const position, Position const scale, SDL_Color const & color);
  void UpdateScreen(void);

private:
  SDL_Window* pWindow;
  SDL_Renderer* pRenderer;
};