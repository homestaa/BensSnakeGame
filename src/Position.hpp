#pragma once

struct Position
{
  int x;
  int y;

  bool operator==(Position const & other) const;
  Position operator+(Position const & other) const;
  Position operator-(Position const & other) const;
};