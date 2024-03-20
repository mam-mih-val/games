#ifndef GAMES_H
#define GAMES_H

#include <SFML/System/Thread.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <algorithm>
#include <chrono>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <ratio>
#include <vector>

#include <thread>

#include "coordinates.h"
#include "events.h"
#include "graphics.h"
#include "physics.h"
#include "render.h"

// #include <X11/Xlib.h>

constexpr double dT = 1; 

class InputManager{
public:
  InputManager() {}
  void Listen(){
    {
      auto lock = std::lock_guard<std::mutex>( EventManager::window.mutex );
      EventManager::window.window.pollEvent(event_);
    }
    
    if( event_.type == sf::Event::Closed ){
      auto lock = std::lock_guard< std::mutex >{ EventManager::exit.mutex };
      EventManager::exit.exit = true;
      return;
    }

    {
      auto lock_control = std::lock_guard<std::mutex>{EventManager::control_queue.mutex};
      auto lock_camera = std::lock_guard<std::mutex>{EventManager::camera_queue.mutex};
      if( event_.type == sf::Event::KeyPressed ){
        switch (event_.key.scancode) {
          case sf::Keyboard::Scancode::Space:
            EventManager::control_queue.events.push( {ControlEvent::ControlCommand::ACCELERATE} );
            break;
          case sf::Keyboard::Scancode::LShift:
            EventManager::control_queue.events.push( {ControlEvent::ControlCommand::DECELERATE} );
            break;
          case sf::Keyboard::Scancode::W:
            EventManager::control_queue.events.push( {ControlEvent::ControlCommand::TURN_UP} );
            break;
          case sf::Keyboard::Scancode::S:
            EventManager::control_queue.events.push( {ControlEvent::ControlCommand::TURN_UP} );
            break;
          case sf::Keyboard::Scancode::D:
            EventManager::control_queue.events.push( {ControlEvent::ControlCommand::TURN_RIGHT} );
            break;
          case sf::Keyboard::Scancode::A:
            EventManager::control_queue.events.push( {ControlEvent::ControlCommand::TURN_LEFT} );
            break;
          case sf::Keyboard::Scancode::C:
            EventManager::camera_queue.events.push( {CameraEvent::ControlCommand::ZOOM_OUT} );
            break;
          case sf::Keyboard::Scancode::V:
            EventManager::camera_queue.events.push( {CameraEvent::ControlCommand::ZOOM_IN} );
            break;
          case sf::Keyboard::Scancode::M:
            EventManager::camera_queue.events.push( {CameraEvent::ControlCommand::FOCUS_ON_MOON} );
            break;
          case sf::Keyboard::Scancode::E:
            EventManager::camera_queue.events.push( {CameraEvent::ControlCommand::FOCUS_ON_EARTH} );
            break;
          default:
            break;
        }
      }
    }
  }

private:
  sf::Event event_{};
};

class Game{
public:
  Game( Physics::Physics _physics ) : 
    physics_{_physics},
    render_engine_{},
    input_manager_{} {}
  Game( const Game& ) = default;
  Game( Game&& ) = default;
  Game& operator=( const Game& ) = default;
  Game& operator=( Game&& ) = default;
  
  void PlayGame(){
    auto thread_input = std::thread( [this](){
      auto closed = false;
      while(!closed){
        input_manager_.Listen();
        std::this_thread::sleep_for(std::chrono::milliseconds(5 ));
        auto lock = std::lock_guard(EventManager::exit.mutex);
        closed = EventManager::exit.exit;
      }
    } );    
    auto thread_physics = std::thread( [this](){
      auto closed = false;
      while(!closed){
        physics_.Update(dT);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        auto lock = std::lock_guard(EventManager::exit.mutex);
        closed = EventManager::exit.exit;
      }
    } );
    auto thread_render = std::thread( [this](){
      auto closed = false;
      while(!closed){
        render_engine_.Render();
        auto lock = std::lock_guard(EventManager::exit.mutex);
        closed = EventManager::exit.exit;
      }
    } );
    thread_input.join();
    thread_physics.join();
    thread_render.join();
  }
private:
  Physics::Physics physics_;
  Render::RenderEngine render_engine_;
  InputManager input_manager_;
};

inline auto MakeInterlanetaryGame(){
  auto physics = Physics::MakePhysics();
  auto game = Game( physics );
  return game;
}

#endif// GAMES_H