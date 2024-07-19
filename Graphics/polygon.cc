#include "polygon.h"
#include "coordinates.h"
#include <algorithm>

auto Polygon::CalculateCM() noexcept -> void {
  center_mass_.x=0;
  center_mass_.y=0;
  std::for_each( points_.begin(), points_.end(), [this]( auto& p){ center_mass_+=p; } );
  center_mass_ /= points_.size();
}

auto Polygon::Translate( const Point2D& new_cm ) noexcept -> void {
  auto diff = new_cm - center_mass_;
  std::for_each( points_.begin(), points_.end(), [diff](auto& p){ p+=diff; } );
  CalculateCM();
}

auto Polygon::Rotate(const Point2D &rot_point, const double angle) noexcept -> void {
  auto new_points = points_;
  std::for_each( new_points.begin(), new_points.end(), [&rot_point]( auto& p){ p-=rot_point; } );
  auto matrix = MakeRotationMatrix(angle);
  std::for_each( new_points.begin(), new_points.end(), [&matrix]( auto& p){ p*=matrix; } );
  std::for_each( new_points.begin(), new_points.end(), [&rot_point]( auto& p){ p+=rot_point; } );
  points_.swap(new_points);
  CalculateCM();
}

auto Polygon::RotateCM(const double angle) noexcept -> void { Rotate(center_mass_, angle); }