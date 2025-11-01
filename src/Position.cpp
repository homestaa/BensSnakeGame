#include "Position.hpp"

bool Position::operator==(Position const & other) const
{
  return (this->x == other.x) && (this->y == other.y);
}