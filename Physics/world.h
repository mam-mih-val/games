#ifndef WORLD_H
#define WORLD_H

#include "body.h"
#include "coordinates.h"
#include "gravitation.h"
#include <algorithm>
#include <memory>

class World{
public:
  World() = default;

  auto AddBody( Body* body ) noexcept -> World& { bodies_.emplace_back( body ); return *this; }

  auto Propagate( const double dt ){
    std::for_each( bodies_.begin(), bodies_.end(), [this, dt](auto& p){ Gravitation::MoveBody(dt, p.get(), bodies_); } );
  }

private:
  Body rocket_{};
  std::vector<std::unique_ptr<Body>> bodies_;
};

#endif // WORLD_H