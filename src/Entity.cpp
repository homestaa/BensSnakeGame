#include "Entity.hpp"
#include <SDL_render.h>

Entity::Entity(SDL_Texture* pTexture, Position const & position)
: pTexture_(pTexture)
, textureSize{ 0, 0 }
, position_(position)
, scale_{ 0, 0 }
, angle_(0.0)
{
  SDL_QueryTexture(pTexture, nullptr, nullptr, &textureSize.x, &textureSize.y);
  scale_ = textureSize;
}


Entity::Entity(SDL_Texture* pTexture, Position const & position, Position const & scale)
: pTexture_(pTexture)
, textureSize{0, 0}
, position_(position)
, scale_(scale)
, angle_(0.0)
{
  SDL_QueryTexture(pTexture, nullptr, nullptr, &textureSize.x, &textureSize.y);
}


SDL_Texture* Entity::GetTexture(void) const
{
  return pTexture_;
}


Position Entity::GetTextureSize(void) const
{
  return textureSize;
}


Position Entity::GetScale(void) const
{
  return scale_;
}


Position Entity::GetPosition(void) const
{
  return position_;
}


void Entity::SetPosition(int const x, int const y)
{
  position_.x = x;
  position_.y = y;
}


void Entity::SetScale(int const w, int const h)
{
  scale_.x = w;
  scale_.y = h;
}


void Entity::SetAngle(double const angle)
{
  angle_ = angle;
}

bool Entity::IsOnPosition(Position const & position) const
{
  return    (position.x >= position_.x)
         && (position.y >= position_.y)
         && (position.y <= (position_.y + scale_.y))
         && (position.x <= (position_.x + scale_.x));
}