#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Window/Event.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <ctime>
#include <limits>
#include <memory>
#include <mutex>
#include <numeric>
#include <queue>
#include <stack>
#include <utility>
#include <vector>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>

#include "coordinates.h"
#include "window.h"

namespace Graphics{

struct Color{
  Color() = default;
  ~Color() = default;
  Color& SetRed( double val ){ red = val; return *this; }
  Color& SetGreen( double val ){ green = val; return *this; }
  Color& SetBlue( double val ){ blue = val; return *this; }
  double red;
  double green;
  double blue;
};

class Polygon{
public:
  Polygon() = default;
  Polygon( Point2D p1, Point2D p2 );
  Polygon( Point2D center, double radius, size_t n_points );
  Polygon( std::vector<Point2D> points );
  ~Polygon() = default;
  void SetPosition(Point2D pos);
  void Translate( const Point2D& translation_vector );
  void Rotate(Point2D rotation_point, double theta);
  const std::vector<Point2D>& GetLabPoints() const { return points_; }
  sf::ConvexShape GetImage() const;
  const Point2D& GetCenterMass(){ return center_mass_; }
  void SetColor( Color color ){ color_ = std::move(color); }
  const Color& GetColor(){ return color_; }
private:
  Point2D center_mass_{};
  std::vector<Point2D> points_{}; // With respect to CM
  Color color_{};
};

class Composition{
public:
  Composition() = default;
  ~Composition() = default;
  Composition& AddPolygon( Polygon p );
  Composition& Translate( Point2D& vec );
  Composition& Rotate( Point2D point, double angle );
  Composition& RotateCM( double angle );
  Composition& Align( Point2D vector );
  Composition& SetPosition( Point2D pos );
  const std::vector<Polygon>& GetPolygons() const;
  std::vector<sf::ConvexShape> GetImages() const;
  Point2D GetCM( );
private:
  template<typename Func>
  void PerformOnEach( Func function ){
    std::for_each( polygons_.begin(), polygons_.end(), function );
  }
  void CalculateCM();
  Point2D center_mass_{};
  Point2D orientation_{0.0, 1.0};
  std::vector<Polygon> polygons_{};
};

Composition MakeRocket( const Point2D& pos );
Composition MakePlanet( const Point2D& pos, double radius );
inline Composition MakeEarth( const Point2D& pos ){ return MakePlanet(pos, 64); }
inline Composition MakeMoon( const Point2D& pos ){ return MakePlanet(pos, 10); }


class Scene{
public:
  Scene() {}
  ~Scene() = default;
  void AddToFrame( Composition comp ){
    auto shapes = comp.GetImages();
    shapes_buffer_.insert(shapes_buffer_.end(), shapes.begin(), shapes.end());
  }
  void AddToBuffer( std::vector<Composition> comp ){
    for( auto& c : comp ){
      AddToFrame(c);
    }
  }
  const std::vector<sf::ConvexShape>& GetBuffer(){    
    return shapes_buffer_;
  }
  void ClearBuffer(){ shapes_buffer_.clear(); }
  sf::View& GetView(){ return camera_; }
  void SetView( const sf::View& camera ){ camera_ = camera; }
private:
  sf::View camera_{};
  std::vector<sf::ConvexShape> shapes_buffer_;
};

class GraphicEngine{
public:
  GraphicEngine() {}
  GraphicEngine& ChangeBackground( Scene s ){ background_ = s; return *this; }
  
  void DrawFrame( Scene& frame );
  void DrawData();
  void DrawBackground();
  void Display(){
    auto lock = std::lock_guard<std::mutex>( WindowManager::window.mutex );
    WindowManager::window.window.display();
  }
  bool RenderFrame();

  void SetActive(){
    auto lock = std::lock_guard<std::mutex>( WindowManager::window.mutex );
    WindowManager::window.window.setActive(true);
  }
  void SetInactive(){
    auto lock = std::lock_guard<std::mutex>( WindowManager::window.mutex );
    WindowManager::window.window.setActive(false);
  }
private:
  Scene background_;
  // sf::RenderWindow& window_;
};
}
#endif