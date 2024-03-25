#ifndef INPUT_H
#define INPUT_H

#include <atomic>
#include <mutex>
#include <queue>

#include <SFML/System/Thread.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "window.h"

namespace Input{

struct ComputerCommand{
  enum class CommandType{
    ACCELERATE,
    DECELERATE,
    STEER_UP,
    STEER_DOWN,
    STEER_LEFT,
    STEER_RIGHT,
    NOTHING,
    EXECUTE,
    CLEAR,
    REMOVE_LAST,
    SYNC,
  };
  CommandType command;
  int time{0};
};

struct CameraCommand{
  enum class CommandType{
    ZOOM_IN,
    ZOOM_OUT,
    FOCUS_ON_EARTH,
    FOCUS_ON_MOON,
    FOCUS_ON_ROCKET,
    NOTHING
  };
  CommandType command;
};


struct Comands{
  ComputerCommand computer_command;
  CameraCommand camera_command;
};

class GameInput{
public:
  GameInput() {}
  void Listen();
  void RegisterCallback(const std::function<void(Comands)>& function){ callbacks_.push_back( function ); }
  int ListenTime();
private:
  void Notify(){
    std::for_each( callbacks_.begin(), callbacks_.end(), 
      [this](const auto& c){
          c( commands_ );
      }
    );
  }
  Comands commands_;
  sf::Event event_{};
  std::vector< std::function<void(Comands)> > callbacks_{};

};

}
#endif // INPUT_H
