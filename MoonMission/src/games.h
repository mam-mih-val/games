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
    physics_{_physics},
    analytics_{physics_},
    render_engine_{},
    input_manager_{} 
  {
    physics_.RegisterCallback( render_engine_.MakeCallbackPhysics() );
    physics_.RegisterCallback( analytics_.MakeCallbackPhysics() );
    analytics_.RegisterCallback( render_engine_.MakeCallbackAnalytics() );

    input_manager_.RegisterCallback( physics_.MakeCallbackGameInput() );
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
        auto update_time = WindowManager::pause ? 0.0 : dT;
        physics_.Update(update_time);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
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
    auto thread_analytics = std::thread( [this](){
      auto closed = false;
      while(!closed){
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        analytics_.Calculate(10*dT, 50);
        closed = WindowManager::exit;
      }
    } );
    thread_input.join();
    thread_physics.join();
    thread_render.join();
    thread_analytics.join();
  }
private:
  Physics::Physics physics_;
  Physics::Analytics analytics_;
  Render::RenderEngine render_engine_;
  GameInput input_manager_;
};

inline auto MakeInterlanetaryGame(){
  auto physics = Physics::MakePhysics();
  auto game = Game( physics );
  return game;
}

#endif// GAMES_H