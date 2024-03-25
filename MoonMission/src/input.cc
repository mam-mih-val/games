#include "input.h"

namespace Input{

void GameInput::Listen(){
  {
    auto lock = std::lock_guard<std::mutex>( WindowManager::window.mutex );
    WindowManager::window.window.pollEvent(event_);
  }
  
  if( event_.type == sf::Event::Closed ){
    WindowManager::exit = true;
    return;
  }

  commands_.computer_command = { ComputerCommand::CommandType::NOTHING, 0 };
  commands_.camera_command = {CameraCommand::CommandType::NOTHING};

  if( event_.type == sf::Event::KeyPressed ){
    // ***************************************
    // ROCKET CONTROL
    // ***************************************
    switch (event_.key.scancode) {
      case sf::Keyboard::Scancode::Space:
        commands_.computer_command = {ComputerCommand::CommandType::ACCELERATE, ListenTime()};
        break;
      case sf::Keyboard::Scancode::Z:
        commands_.computer_command = {ComputerCommand::CommandType::DECELERATE, ListenTime()};
        break;
      case sf::Keyboard::Scancode::W:
        commands_.computer_command = {ComputerCommand::CommandType::STEER_UP, ListenTime()};
        break;
      case sf::Keyboard::Scancode::S:
        commands_.computer_command = {ComputerCommand::CommandType::STEER_DOWN, ListenTime()};
        break;
      case sf::Keyboard::Scancode::D:
        commands_.computer_command = {ComputerCommand::CommandType::STEER_RIGHT, ListenTime()};
        break;
      case sf::Keyboard::Scancode::A:
        commands_.computer_command = {ComputerCommand::CommandType::STEER_LEFT, ListenTime()};
        break;
      case sf::Keyboard::Scancode::N:
        commands_.computer_command = {ComputerCommand::CommandType::NOTHING, ListenTime()};
        break;
      case sf::Keyboard::Scancode::LShift:
        commands_.computer_command = {ComputerCommand::CommandType::SYNC, 0};
        break;
      case sf::Keyboard::Scancode::C:
        commands_.computer_command = {ComputerCommand::CommandType::CLEAR, 0};
        break;
      case sf::Keyboard::Scancode::R:
        commands_.computer_command = {ComputerCommand::CommandType::REMOVE_LAST, 0};
        break;
      case sf::Keyboard::Scancode::E:
        commands_.computer_command = {ComputerCommand::CommandType::EXECUTE, 0};
        break;
    // ***************************************
    // CAMERA CONTROL
    // ***************************************
      case sf::Keyboard::Scancode::RBracket:
        commands_.camera_command = {CameraCommand::CommandType::ZOOM_IN};
        break;
      case sf::Keyboard::Scancode::LBracket:
        commands_.camera_command = {CameraCommand::CommandType::ZOOM_OUT};
        break;
      case sf::Keyboard::Scancode::Num1:
        commands_.camera_command = {CameraCommand::CommandType::FOCUS_ON_EARTH};
        break;
      case sf::Keyboard::Scancode::Num2:
        commands_.camera_command = {CameraCommand::CommandType::FOCUS_ON_MOON};
        break;
      case sf::Keyboard::Scancode::Num3:
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

int GameInput::ListenTime(){
  {
    auto lock = std::lock_guard<std::mutex>( WindowManager::window.mutex );
    WindowManager::window.window.pollEvent(event_);
  }
  while( event_.type != sf::Event::KeyPressed ){ 
    auto lock = std::lock_guard<std::mutex>( WindowManager::window.mutex );
    WindowManager::window.window.pollEvent(event_); 
  }
  switch (event_.key.scancode) {
    case sf::Keyboard::Scancode::Num1:
      return 10;
      break;
    case sf::Keyboard::Scancode::Num2:
      return 20;
      break;
    case sf::Keyboard::Scancode::Num3:
      return 30;
      break;
    case sf::Keyboard::Scancode::Num4:
      return 40;
      break;
    case sf::Keyboard::Scancode::Num5:
      return 50;
      break;
    case sf::Keyboard::Scancode::Num6:
      return 60;
      break;
    case sf::Keyboard::Scancode::Num7:
      return 70;
      break;
    case sf::Keyboard::Scancode::Num8:
      return 80;
      break;
    case sf::Keyboard::Scancode::Num9:
      return 90;
      break;
    default:
      return 0;
  }
}

}