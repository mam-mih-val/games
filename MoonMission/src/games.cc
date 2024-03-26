#include "games.h"

Game::Game( Physics::Physics _physics ) : 
  game_engine_{_physics},
  render_engine_{},
  input_manager_{} 
{
  game_engine_.RegisterPhysicsCallback( render_engine_.MakeCallbackPhysics() );
  game_engine_.RegisterComputerCallback( render_engine_.MakeCallbackAnalytics() );

  input_manager_.RegisterCallback( game_engine_.MakeCallbackGameInput() );
  input_manager_.RegisterCallback( render_engine_.MakeCallbackGameInput() );
}

void Game::PlayGame(){
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
      auto time = !WindowManager::pause ? dT : 0;
      game_engine_.UpdatePhysics(time);        
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
      closed = WindowManager::exit;
    }
  } );
  auto thread_computer = std::thread( [this](){
    auto closed = false;
    while(!closed){
      game_engine_.UpdateAnalysis(dT);        
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
  thread_computer.join();
  thread_render.join();
}