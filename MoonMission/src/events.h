#ifndef EVENTS_H
#define EVENTS_H

#include <SFML/Graphics/RenderWindow.hpp>
#include <mutex>
#include <queue>

#include "coordinates.h"
#include "graphics.h"

struct ControlEvent{
  enum class ControlCommand{
    TURN_UP,
    TURN_DOWN,
    TURN_LEFT,
    TURN_RIGHT,
    ACCELERATE,
    DECELERATE
  };
  ControlCommand command;
};

struct ControlQueue{
  std::queue<ControlEvent> events{};
  std::mutex mutex{};
};

struct CameraEvent{
  enum class ControlCommand{
    FOCUS_ON_EARTH,
    FOCUS_ON_MOON,
    FOCUS_ON_ROCKET,
    ZOOM_IN,
    ZOOM_OUT,
  };
  ControlCommand command;
};

struct CameraQueue{
  std::queue<CameraEvent> events{};
  std::mutex mutex{};
};

struct WorldState{
  Point2D rocket_position{};
  Point2D moon_position{};
  Point2D earth_position{};
};

struct State{
  WorldState state{};
  std::mutex mutex{};
};

struct Window{
  sf::RenderWindow window{ sf::VideoMode(800, 800), "My window" };
  std::mutex mutex{};
};


struct Exit{
  bool exit{false};
  std::mutex mutex{};
};


class EventManager{
public:
  static ControlQueue control_queue;
  static CameraQueue camera_queue;
  static State physics_state;
  static Window window;
  static Exit exit;
};

ControlQueue EventManager::control_queue = ControlQueue{};
CameraQueue EventManager::camera_queue = CameraQueue{};
State EventManager::physics_state = State{};
Window EventManager::window = Window{};
Exit EventManager::exit = Exit{};


#endif // EVENTS_H
