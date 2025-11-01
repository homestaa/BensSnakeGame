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
  void SetTexture(SDL_Texture* pTexture);
  void SetPosition(Position const & position);
  void SetScale(Position const & scale);
  void SetAngle(double const angle);
  bool IsOnPosition(Position const & position) const;

private:
  SDL_Texture* pTexture_;
  Position textureSize;
  Position position_;
  Position scale_;
  double angle_;
};