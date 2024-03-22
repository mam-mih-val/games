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
#include "input_commands.h"

struct Comands{
  SteerCommand steer_command;
  CameraCommand camera_command;
};

class GameInput{
public:
  GameInput() {}
  void Listen(){
    {
      auto lock = std::lock_guard<std::mutex>( WindowManager::window.mutex );
      WindowManager::window.window.pollEvent(event_);
    }
    
    if( event_.type == sf::Event::Closed ){
      WindowManager::exit = true;
      return;
    }

    commands_.steer_command = {SteerCommand::CommandType::NOTHING};
    commands_.camera_command = {CameraCommand::CommandType::NOTHING};

    if( event_.type == sf::Event::KeyPressed ){
      // ***************************************
      // ROCKET CONTROL
      // ***************************************
      switch (event_.key.scancode) {
        case sf::Keyboard::Scancode::Space:
          commands_.steer_command = {SteerCommand::CommandType::ACCELERATE};
          break;
        case sf::Keyboard::Scancode::LShift:
          commands_.steer_command = {SteerCommand::CommandType::DECELERATE};
          break;
        case sf::Keyboard::Scancode::W:
          commands_.steer_command = {SteerCommand::CommandType::STEER_UP};
          break;
        case sf::Keyboard::Scancode::S:
          commands_.steer_command = {SteerCommand::CommandType::STEER_DOWN};
          break;
        case sf::Keyboard::Scancode::D:
          commands_.steer_command = {SteerCommand::CommandType::STEER_RIGHT};
          break;
      // ***************************************
      // CAMERA CONTROL
      // ***************************************
        case sf::Keyboard::Scancode::A:
          commands_.steer_command = {SteerCommand::CommandType::STEER_LEFT};
          break;
        case sf::Keyboard::Scancode::C:
          commands_.camera_command = {CameraCommand::CommandType::ZOOM_IN};
          break;
        case sf::Keyboard::Scancode::V:
          commands_.camera_command = {CameraCommand::CommandType::ZOOM_OUT};
          break;
        case sf::Keyboard::Scancode::M:
          commands_.camera_command = {CameraCommand::CommandType::FOCUS_ON_MOON};
          break;
        case sf::Keyboard::Scancode::E:
          commands_.camera_command = {CameraCommand::CommandType::FOCUS_ON_EARTH};
          break;
        case sf::Keyboard::Scancode::R:
          commands_.camera_command = {CameraCommand::CommandType::FOCUS_ON_ROCKET};
          break;
      // ***************************************
      // GAME CONTROL
      // ***************************************
        case sf::Keyboard::Scancode::Escape:
          WindowManager::pause = true;
          break;
        case sf::Keyboard::Scancode::Enter:
          WindowManager::pause = false;
          break;
        default:
          break;
      }
      Notify();
    }
  }
  void RegisterCallback(const std::function<void(Comands)>& function){ callbacks_.push_back( function ); }

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


#endif // INPUT_H
