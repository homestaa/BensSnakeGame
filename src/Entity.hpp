#pragma once

#include "Position.hpp"

typedef struct SDL_Texture SDL_Texture;

class Entity
{
public:
  Entity(SDL_Texture* pTexture, Position const & pos);
  Entity(SDL_Texture* pTexture, Position const & pos, Position const & scale);
  ~Entity(void) = default;

  SDL_Texture* GetTexture() const;
  Position GetTextureSize(void) const;
  Position GetScale(void) const;
  Position GetPosition(void) const;
  void SetPosition(int const x, int const y);
  void SetScale(int const w, int const h);
  void SetAngle(double const angle);
  bool IsOnPosition(Position const & position) const;

private:
  SDL_Texture* pTexture_;
  Position textureSize;
  Position position_;
  Position scale_;
  double angle_;
};