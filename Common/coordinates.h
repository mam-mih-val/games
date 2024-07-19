#ifndef COORDINATES_H
#define COORDINATES_H

#include <SFML/System/Vector2.hpp>
#include <array>
#include <iostream>
#include <limits>
#include <math.h>

struct Point2D{
  double x{};
  double y{};
  Point2D() = default;
  Point2D( double _x, double _y ) : x(_x), y(_y) {}
  template<typename T>
  Point2D( const sf::Vector2<T>& vec ) : 
    x( static_cast<double>(vec.x) ),
    y( static_cast<double>(vec.y) ){}

  void Print() const;

  void Rotate( const Point2D& p, double theta );

  auto SfVector() const{ return sf::Vector2(static_cast<float>(x), static_cast<float>(y)); }

  double Mag() const;

  void operator+=( const Point2D& other );
  void operator-=( const Point2D& other );
  void operator*=( double scale );
  void operator*=( std::array< std::array<double, 2>, 2> matrix );
  void operator/=( double scale );
  bool operator==( const Point2D& other ) const;
  
  friend Point2D operator+(const Point2D& first, const Point2D& second);
  friend Point2D operator-(const Point2D& first, const Point2D& second);
  friend Point2D operator*(const Point2D& first, double scale);
  friend Point2D operator*(const Point2D& first, std::array< std::array<double, 2>, 2> matrix);
  friend Point2D operator/(const Point2D& first, double scale);
  
  friend double Distance( const Point2D& first, const Point2D& second );
  friend Point2D Direction(const Point2D& first, const Point2D& second);
  friend double Dot( Point2D f, Point2D s );
  friend double Angle( Point2D f, Point2D s );
};

std::array< std::array<double, 2>, 2> MakeRotationMatrix(double theta);

#endif // COORDINATES_H
