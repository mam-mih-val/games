#ifndef WINDOW_H
#define WINDOW_H

#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/View.hpp>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <math.h>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Window.hpp>

#include "coordinates.h"

class Window{
public:
  Window( double width, double hight ) : 
    window_{sf::VideoMode(width, hight), "My window" } { }  
  auto Update(){
    auto lock = std::lock_guard{window_mutex_};
    auto event = sf::Event();
    if( window_.pollEvent(event) ){
      if( event.type == sf::Event::Closed ){
        exit_ = true;
        return;
      }
    }
    window_.clear(sf::Color::White);
    while( !draw_queue_.empty() ){
      window_.draw(draw_queue_.front() );
      draw_queue_.pop();
    }
    window_.display();
  }
  auto Exit(){ return exit_; }
  auto AddToQueue( sf::ConvexShape shape ){ 
    auto lock = std::lock_guard{window_mutex_}; 
    draw_queue_.emplace(std::move(shape)); 
  }
  auto SetView( const sf::View& view_point ){ window_.setView(view_point); }
  auto operator*() -> sf::RenderWindow& { return window_; }

private:
  std::mutex window_mutex_{};
  bool exit_{false};
  sf::RenderWindow window_;
  std::queue<sf::ConvexShape> draw_queue_{};
  std::function<void(void)> camera_notification_{};
};

#endif // WINDOW_H
