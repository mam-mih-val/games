#ifndef MOUSE_H
#define MOUSE_H

#include "coordinates.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>
#include <mutex>
#include <vector>

#include <SFML/Window/Mouse.hpp>

struct MouseEvent{
  enum class EventType{
    RIGHT_CLICK,
    LEFT_CLICK,
    NOTHING
  };
  Point2D position;
  EventType what{MouseEvent::EventType::NOTHING};
  auto Print(){
    switch (what) {
      case EventType::RIGHT_CLICK:
        std::cout << "Right << ";
        break;
      case EventType::LEFT_CLICK:
        std::cout << "Left << ";
        break;
      case EventType::NOTHING:
        std::cout << "Nothing << ";
        break;
    }
    position.Print();
  }
};

class Mouse{
public:
  Mouse( const sf::Window& w ) : window_(w) { mouse_events_.resize(10); };
  auto GetEvents() const noexcept -> const std::vector<MouseEvent>& { return mouse_events_; }
  auto Listen() -> void { 
    if( mouse_events_.size() > 10 )
      mouse_events_.clear();
    auto new_event = MouseEvent{};
    new_event.position = Point2D{ sf::Mouse::getPosition(window_) };
    if( sf::Mouse::isButtonPressed(sf::Mouse::Left) )
      new_event.what = MouseEvent::EventType::LEFT_CLICK;
    if( sf::Mouse::isButtonPressed(sf::Mouse::Right) )
      new_event.what = MouseEvent::EventType::RIGHT_CLICK;
    new_event.Print();
    mouse_events_.push_back(std::move(new_event));
  }
private:
  const sf::Window& window_;
  std::mutex mutex;
  std::vector<MouseEvent> mouse_events_;
};

#endif