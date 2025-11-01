#include "Entity.hpp"
#include <SDL_render.h>

Entity::Entity(SDL_Texture* const pTexture,
               Position const & position,
               Position const & scale,
               double const angle)
: pTexture_(pTexture)
, textureSize{ 0, 0 }
, position_(position)
, scale_(scale)
, angle_(angle)
{
  SDL_QueryTexture(pTexture, nullptr, nullptr, &textureSize.x, &textureSize.y);
  if (scale == Position{ 0, 0 })
    scale_ = textureSize;
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


double Entity::GetAngle(void) const
{
  return angle_;
}


void Entity::SetTexture(SDL_Texture* const pTexture)
{
  pTexture_ = pTexture;
  SDL_QueryTexture(pTexture, nullptr, nullptr, &textureSize.x, &textureSize.y);
}


void Entity::SetPosition(Position const & position)
{
  position_.x = position.x ;
  position_.y = position.y ;
}


void Entity::SetScale(Position const & scale)
{
  scale_.x = scale.x;
  scale_.y = scale.y;
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