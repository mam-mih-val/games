#ifndef COORDINATES_H
#define COORDINATES_H

#include <array>
#include <iostream>
#include <limits>
#include <math.h>

struct Point2D{
  double x{};
  double y{};
  Point2D() = default;
  Point2D( double _x, double _y ) : x(_x), y(_y) {}

  void Print();

  void Rotate( const Point2D& p, double theta );

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
