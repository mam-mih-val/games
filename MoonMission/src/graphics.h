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
  Polygon( Point2D p1, Point2D p2 ){
    points_.push_back( p1 );
    points_.push_back( {p1.x, p2.y} );
    points_.push_back( p2 );
    points_.push_back( {p2.x, p1.y});
  }
  Polygon( Point2D center, double radius, size_t n_points ){
    auto dphi = 2*M_PI / n_points;
    for( size_t t=0; t<n_points; ++t ){
      auto x = center.x + radius*cos( t*dphi );
      auto y = center.y + radius*sin( t*dphi );
      points_.emplace_back(x, y);
    }
  }
  Polygon( std::vector<Point2D> points )  : points_{std::move(points)} {
    auto sum_vec = std::accumulate( points_.begin(), points_.end(), Point2D{0.0, 0.0} );
    center_mass_ = sum_vec / points.size();
  }
  ~Polygon() = default;
  void SetPosition(Point2D pos){
    auto translation = pos-center_mass_;
    Translate( translation );
    center_mass_ = pos;
  }
  void Translate( const Point2D& translation_vector ){
    std::for_each( points_.begin(), points_.end(), [translation_vector]( auto& p ){ p+=translation_vector; } );
    center_mass_+=translation_vector;
  }
  void Rotate(Point2D rotation_point, double theta){
    std::for_each( points_.begin(), points_.end(), [rotation_point, theta]( auto& p ){ p.Rotate( rotation_point, theta ); } );
    center_mass_.Rotate(rotation_point, theta);
  };
  const std::vector<Point2D>& GetLabPoints() const {
    return points_;
  }
  sf::ConvexShape GetImage() const {
    auto shape = sf::ConvexShape{};
    shape.setPointCount(points_.size());
    std::for_each( points_.begin(), points_.end(), [i=0, &shape]( const auto& p ) mutable {
      shape.setPoint(i, sf::Vector2f{ static_cast<float>(p.x), static_cast<float>(p.y) });
      ++i;
    } );
    return shape;
  }
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
  Composition& AddPolygon( Polygon p ){ 
    polygons_.push_back( p ); 
    CalculateCM();
    return *this;
  }
  Composition& Translate( Point2D& vec ){
    PerformOnEach( [vec]( auto& p ){ p.Translate(vec); } );
    center_mass_+=vec;
    return *this;
  }
  Composition& Rotate( Point2D point, double angle ){
    PerformOnEach([point, angle]( auto& p ){ p.Rotate(point, angle); } );
    auto matrix = MakeRotationMatrix(angle);
    center_mass_ = (center_mass_ - point) * matrix;
    center_mass_ += point;
    return *this;
  }
  Composition& RotateCM( double angle ){
    if( fabs(angle) < std::numeric_limits<double>::epsilon() )
      return *this;
    PerformOnEach( [this, angle]( auto& p ){ p.Rotate(center_mass_, angle); } );
    auto matrix = MakeRotationMatrix(angle);
    orientation_ *= matrix; 
    return *this;
  }
  Composition& Align( Point2D vector ){
    auto angle = Angle(orientation_, vector);
    if( fabs(angle) < std::numeric_limits<double>::epsilon() )
      return *this;
    if( std::isnan(angle) )
      return *this;
    RotateCM(angle);
    return *this;
  }
  Composition& SetPosition( Point2D pos ){
    PerformOnEach( [pos]( auto& p ){ p.SetPosition(pos); } );
    center_mass_ = pos;
    return *this;
  }
  const std::vector<Polygon>& GetPolygons() const {
    return polygons_;
  }
  std::vector<sf::ConvexShape> GetImages() const {
    std::vector<sf::ConvexShape> images;
    std::for_each( polygons_.begin(), polygons_.end(), [&images]( const auto& p ){ images.push_back( p.GetImage() ); } );
    return images;
  }
  Point2D GetCM( ){ CalculateCM(); return center_mass_; }
private:
  template<typename Func>
  void PerformOnEach( Func function ){
    std::for_each( polygons_.begin(), polygons_.end(), function );
  }
  void CalculateCM(){
    Point2D sum_vec{};
    std::for_each( polygons_.begin(), polygons_.end(), [&sum_vec](auto& p){ sum_vec+=p.GetCenterMass(); } );
    center_mass_ = sum_vec / polygons_.size();
  }
  Point2D center_mass_{};
  Point2D orientation_{0.0, 1.0};
  std::vector<Polygon> polygons_{};
};

inline Composition MakeRocket( const Point2D& pos ){
  auto rocket = Composition();
  auto pointing = Polygon( std::vector<Point2D>{
    {-10, 20}, {0, 30}, {10, 20},
  } );
  auto body = Polygon( 
    Point2D( -10, 20 ), Point2D( 10, -20 )
  );
  rocket.AddPolygon( pointing );
  rocket.AddPolygon( body );
  rocket.SetPosition(pos);
  return rocket;
}

inline Composition MakePlanet( const Point2D& pos, double radius ){
  auto planet = Composition();
  auto body = Polygon( 
    pos, radius, 100
  );
  planet.AddPolygon( std::move(body) );
  return planet;
}

inline Composition MakeEarth( const Point2D& pos ){
  return MakePlanet(pos, 64);
}

inline Composition MakeMoon( const Point2D& pos ){
  return MakePlanet(pos, 10);
}


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
  
  void DrawFrame( Scene& frame ){
    auto lock = std::lock_guard<std::mutex>( WindowManager::window.mutex );
    auto vec_shapes = frame.GetBuffer();
    std::for_each( vec_shapes.begin(), vec_shapes.end(), []( auto& s ){ 
      s.setFillColor( sf::Color::Black );
      WindowManager::window.window.draw( s ); 
    } );
    WindowManager::window.window.setView( frame.GetView() );
    frame.ClearBuffer();
  }
  void DrawData(){
    sf::Text text;
    text.setString( "HelloWorld" );
    text.setPosition( 0, 0 );
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::Red);
  }
  void DrawBackground(){
    auto lock = std::lock_guard<std::mutex>( WindowManager::window.mutex );
    WindowManager::window.window.clear( sf::Color::White );
    auto vec_shapes = std::vector<sf::ConvexShape>{};
    std::for_each( vec_shapes.begin(), vec_shapes.end(), []( auto& s ){ 
      s.setFillColor( sf::Color::Black );
      WindowManager::window.window.draw( s ); 
    } );
  }
  void Display(){
    auto lock = std::lock_guard<std::mutex>( WindowManager::window.mutex );
    WindowManager::window.window.display();
  }
  bool RenderFrame(){
    auto lock = std::lock_guard<std::mutex>( WindowManager::window.mutex );
    if( !WindowManager::window.window.isOpen() )
      return false;
    sf::Event event;
    WindowManager::window.window.pollEvent(event);
    if (event.type == sf::Event::Closed){
      WindowManager::window.window.close();
      return false;
    }
    return true;
  }
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