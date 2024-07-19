#include "body.h"
#include "gravitation.h"
#include "world.h"
#include <cmath>

int main(){
  auto earth = new Body{ TheEarth };
  auto moon = new Body{ TheMoon };

  earth->SetPosition({0.0, 0.0});
  moon->SetPosition( {380'000, 0.0} );
  moon->SetVelocity({0.0, sqrt(Gravitation::G / 380'000)});

  auto world = World{};
  world.AddBody(earth);
  world.AddBody(moon);

  while( true ){
    world.Propagate(100);

    moon->GetPosition().Print();
    earth->GetPosition().Print();
  }
  return 0;
}