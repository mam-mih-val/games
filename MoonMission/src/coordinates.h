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

  void Print() const {
    std::cout << "(x=" << x << ", y=" << y << " ) ";
  }

  void Rotate( const Point2D& p, double theta ){
    auto rel_x = x - p.x;
    auto rel_y = y - p.y;

    auto cos_theta = cos( theta );
    auto sin_theta = sin( theta );
    
    auto new_x = rel_x * cos_theta - rel_y*sin_theta;
    auto new_y = rel_x * sin_theta + rel_y*cos_theta;

    x = new_x + p.x;
    y = new_y + p.y;
  }

  double Mag() const { 
    auto mag = sqrt(x*x + y*y);
    return mag;
  }

  void operator+=( const Point2D& other ){
    x+=other.x;
    y+=other.y;
  }
  void operator-=( const Point2D& other ){
    x-=other.x;
    y-=other.y;
  }
  void operator*=( double scale ){ 
    x*=scale;
    y*=scale;
  }
  void operator*=( std::array< std::array<double, 2>, 2> matrix ) {
    auto new_x = x*matrix[0][0] + y*matrix[0][1];
    auto new_y = x*matrix[1][0] + y*matrix[1][1];
    x = new_x;
    y = new_y;
  }
  void operator/=( double scale ){ 
    x/=scale;
    y/=scale;
  }
  bool operator==( const Point2D& other ) const {
    if( fabs(x - other.x) > std::numeric_limits<double>::min() )
      return false;
    if( fabs(y - other.y) > std::numeric_limits<double>::min() )
      return false;
    return true;
  }
  friend Point2D operator+(const Point2D& first, const Point2D& second){ 
    auto result = first;
    result+=second;
    return result;
  };
  friend Point2D operator-(const Point2D& first, const Point2D& second){
    auto result = first;
    result-=second;
    return result;
  };
  friend Point2D operator*(const Point2D& first, double scale){
    auto result = first;
    result*=scale;
    return result;
  };
  friend Point2D operator*(const Point2D& first, std::array< std::array<double, 2>, 2> matrix){
    auto result = first;
    result*=matrix;
    return result;
  };
  friend Point2D operator/(const Point2D& first, double scale){
    auto result = first;
    result/=scale;
    return result;
  };
  friend double Distance( const Point2D& first, const Point2D& second ){
    auto dx = first.x - second.x;
    auto dy = first.y - second.y;
    auto r = sqrt( dx*dx + dy*dy );
    return r;
  }
  friend Point2D Direction(const Point2D& first, const Point2D& second){
    auto vec = second - first;
    auto mag = Distance(first, second);
    vec/=mag;
    return vec;
  };
  friend double Dot( Point2D f, Point2D s ){
    return f.x*s.x + f.y*s.y;
  }
  friend double Angle( Point2D f, Point2D s ){
    auto cos_theta = Dot(f, s) / f.Mag() / s.Mag();
    return acos( cos_theta );
  }
};

std::array< std::array<double, 2>, 2> MakeRotationMatrix(double theta){
  auto cos_theta = cos( theta );
    auto sin_theta = sin( theta );
    auto matrix = std::array<std::array<double, 2>, 2>{
      std::array<double, 2>{cos_theta, -1*sin_theta},
      std::array<double, 2>{sin_theta, cos_theta},
    };
  return matrix;
}

#endif // COORDINATES_H
