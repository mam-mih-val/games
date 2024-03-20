#include <SFML/Graphics.hpp>

#include "graphics.h"
#include "physics.h"
#include "games.h"
#include <memory>
#include <thread>

int main(){
  auto game = MakeInterlanetaryGame();
  game.PlayGame();
  return 0;
}