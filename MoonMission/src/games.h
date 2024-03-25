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
#include "input.h"
#include "window.h"
#include "graphics.h"
#include "physics.h"
#include "render.h"

constexpr double dT = 1; 


class Game{
public:
  Game( Physics::Physics _physics ) : 
    game_engine_{_physics},
    render_engine_{},
    input_manager_{} 
  {
    game_engine_.RegisterPhysicsCallback( render_engine_.MakeCallbackPhysics() );
    game_engine_.RegisterComputerCallback( render_engine_.MakeCallbackAnalytics() );

    input_manager_.RegisterCallback( game_engine_.MakeCallbackGameInput() );
    input_manager_.RegisterCallback( render_engine_.MakeCallbackGameInput() );

  }
  Game( const Game& ) = default;
  Game( Game&& ) = default;
  Game& operator=( const Game& ) = default;
  Game& operator=( Game&& ) = default;
  
  void PlayGame(){
    auto thread_input = std::thread( [this](){
      auto closed = false;
      while(!closed){
        input_manager_.Listen();
        closed = WindowManager::exit;
      }
    } );    
    auto thread_physics = std::thread( [this](){
      auto closed = false;
      while(!closed){
        game_engine_.UpdatePhysics(dT);        
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        closed = WindowManager::exit;
      }
    } );
    auto thread_computer = std::thread( [this](){
      auto closed = false;
      while(!closed){
        game_engine_.UpdateAnalysis(dT);        
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        closed = WindowManager::exit;
      }
    } );
    auto thread_render = std::thread( [this](){
      auto closed = false;
      while(!closed){
        render_engine_.Render();
        closed = WindowManager::exit;
      }
    } );
    thread_input.join();
    thread_physics.join();
    thread_render.join();
  }
private:
  Physics::GameControl game_engine_;
  Physics::Physics physics_;
  Render::RenderEngine render_engine_;
  Input::GameInput input_manager_;
};

inline auto MakeInterlanetaryGame(){
  auto physics = Physics::MakePhysics();
  auto game = Game( physics );
  return game;
}

#endif// GAMES_H