#ifndef RENDER_ENGINE_H
#define RENDER_ENGINE_H

#include <algorithm>
#include <functional>
#include <queue>
#include <vector>

#include <SFML/Graphics/ConvexShape.hpp>

#include "polygon.h"
#include "shape.h"
#include "window.h"


class RenderEngine{
public:
  RenderEngine() = default;
  auto AddToQueue( const Polygon& p ){ polygons_.emplace( p ); }
  auto AddToQueue( const Shape& s ){ 
    std::for_each( s.GetPolygons().begin(), s.GetPolygons().end(), [this]( const auto& p ){ AddToQueue(p); } ); 
  }

  auto Render(){
    while( !polygons_.empty() ){
      shapes_.emplace( RenderPolygon(polygons_.front()) );
      polygons_.pop();
    }
  }
  
  auto RegisterWindow(Window& w){
    callback_ = GenerateNotification(w) ;
  }
  
  auto Notify() {
    callback_(shapes_);
  }

private:
  static sf::ConvexShape RenderPolygon( const Polygon& p ){
    auto points = p.GetPoints();
    auto convex_shape = sf::ConvexShape( points.size() );
    for( size_t i=0; i<points.size(); ++i ){
      convex_shape.setPoint(i, points.at(i).SfVector() );
    }
    convex_shape.setFillColor( {p.GetColor().red, p.GetColor().green, p.GetColor().blue });
    return convex_shape;
  }
  static std::function<void( std::queue<sf::ConvexShape>& )> GenerateNotification( Window& w ){
    return 
      [&w]( std::queue<sf::ConvexShape>& shape_queue ) mutable {
        while(!shape_queue.empty()){
          w.AddToQueue(shape_queue.front());
          shape_queue.pop();
        }
      };
  }
  std::queue<Polygon> polygons_{};
  std::queue<sf::ConvexShape> shapes_{};
  std::function<void( std::queue<sf::ConvexShape>& )> callback_;
};

#endif // RENDER_ENGINE_H
