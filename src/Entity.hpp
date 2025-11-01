#pragma once

#include "Position.hpp"

typedef struct SDL_Texture SDL_Texture;

class Entity
{
public:
  Entity(SDL_Texture* const pTexture,
         Position const & position,
         Position const & scale = { 0, 0 },
         double const angle = 0.0);
  ~Entity(void) = default;

  SDL_Texture* GetTexture() const;
  Position GetTextureSize(void) const;
  Position GetScale(void) const;
  Position GetPosition(void) const;
  double GetAngle(void) const;
  void SetTexture(SDL_Texture* const pTexture);
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