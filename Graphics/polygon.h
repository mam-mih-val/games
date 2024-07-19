#ifndef POLYGON_H
#define POLYGON_H

#include <cmath>
#include <vector>

#include "coordinates.h"

struct Color{
  Color() = default;
  auto Red( unsigned char val ){ red = val; }
  auto Green( unsigned char val ){ green = val; }
  auto Blue( unsigned char val ){ blue = val; }
  unsigned char red{0};
  unsigned char green{0};
  unsigned char blue{0};
};

class Polygon{
public:
  Polygon( std::vector<Point2D> vec_points ) : points_{std::move(vec_points)} { CalculateCM(); }
  Polygon( Point2D center_mass, double width, double hight ) : center_mass_{center_mass} { 
    points_.reserve(4);
    points_.emplace_back( center_mass.x + width/2, center_mass.y + hight/2 );
    points_.emplace_back( center_mass.x + width/2, center_mass.y - hight/2 );
    points_.emplace_back( center_mass.x - width/2, center_mass.y - hight/2 );
    points_.emplace_back( center_mass.x - width/2, center_mass.y + hight/2 );
  }
  Polygon( Point2D center_mass, size_t n_points, double radius ){
    points_.reserve(n_points);
    auto dphi = 2*M_PI / n_points;
    for( size_t i=0; i<n_points; ++i ){
      auto phi = dphi*i;
      auto x = radius*cos(phi);
      auto y = radius*sin(phi);
      points_.push_back( center_mass+Point2D{x, y} );
    }
  }
  auto Translate( const Point2D& new_cm ) noexcept -> void;
  auto Rotate( const Point2D& rot_point, const double angle ) noexcept  -> void;
  auto RotateCM( const double angle ) noexcept  -> void;
  auto SetColor( Color c ) noexcept -> Polygon& { color_ = std::move(c); return *this; }
  
  auto GetPoints() const noexcept -> const std::vector<Point2D>& { return points_; }
  auto GetCenterMass() const noexcept -> const Point2D& { return center_mass_; }
  auto GetColor() const noexcept -> const Color& { return color_; }

private:
  auto CalculateCM() noexcept -> void;
  std::vector<Point2D> points_;
  Point2D center_mass_{};
  Color color_;
};

#endif // POLYGON_H