#ifndef CIRCLE_H
#define CIRCLE_H

#include "vector2.h"

template<class Numeric>
struct Circle
{
  Vector2<Numeric> centre;
  Numeric radius;
};

#endif