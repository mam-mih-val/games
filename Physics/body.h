#ifndef BODY_H
#define BODY_H

#include <functional>

#include "coordinates.h"

class Body{
public:
  Body() = default;
  Body( double mass, double radius ) : mass_(mass), radius_(radius){  }

  auto SetMass( double mass ) noexcept -> Body& { mass_ = mass; return *this; }
  auto SetRadius( double radius ) noexcept -> Body& { radius_ = radius; return *this; }
  auto SetPosition( Point2D pos ) noexcept -> Body& { position_ = std::move(pos); return *this; }
  auto SetVelocity( Point2D vel ) noexcept -> Body& { velocity_ = std::move(vel); return *this; }

  auto GetPosition() const noexcept -> const Point2D& { return position_; }
  auto GetVelocity() const noexcept -> const Point2D& { return velocity_; }
  auto GetMass() const noexcept -> double { return mass_; }
  auto GetRadius() const noexcept -> double { return radius_; }

private:
  Point2D position_{};
  Point2D velocity_{};
  double mass_{};
  double radius_{};
};

static const Body TheEarth{ 1, 6371 };

static const Body TheMoon{ 0.0123031469 , 1734 };

#endif // BODY_H