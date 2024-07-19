#ifndef SHAPE_H
#define SHAPE_H

#include <algorithm>
#include <vector>

#include "coordinates.h"
#include "polygon.h"

class Shape{
public:
  Shape() = default;
  auto AddPolygon( Polygon p ) noexcept -> Shape&  { polygons_.push_back( std::move(p) ); CalculateCM(); return *this; }
  const auto& GetPolygons() const noexcept { return polygons_; }
  auto Translate( const Point2D& point ){ Perform( [&point]( auto& p ){ p.Translate(point); } ); }
  auto Rotate( const Point2D& point, double angle ){ Perform( [&point, angle]( auto& p ){ p.Rotate(point, angle); } ); }
  auto RotateCM( double angle ){ Perform( [this, angle]( auto& p ){ p.Rotate(center_mass_, angle); } ); }
  template<typename F>
  auto Perform( const F& func ) noexcept -> void { std::for_each( polygons_.begin(), polygons_.end(), func ); }
private:
  auto CalculateCM() -> void { 
    center_mass_.x = 0;
    center_mass_.y = 0;
    std::for_each( polygons_.begin(), polygons_.end(), [this]( const auto& p ){ center_mass_+=p.GetCenterMass(); } );
    center_mass_ /= polygons_.size();
  }
  std::vector<Polygon> polygons_{};
  Point2D center_mass_{0.0, 0.0};
};

#endif // SHAPE_H