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
  Game( Physics::Physics _physics );
  Game( const Game& ) = default;
  Game( Game&& ) = default;
  Game& operator=( const Game& ) = default;
  Game& operator=( Game&& ) = default;
  
  void PlayGame();
  
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