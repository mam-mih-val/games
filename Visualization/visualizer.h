#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <map>

#include "camera.h"
#include "render_engine.h"
#include "scene.h"
#include "window.h"
#include "body.h"

namespace Visualization{

template<typename Func>
class Visualizer{
public:
  Visualizer(Func function) : coordinate_tranformation_(std::move(function)) {};

  auto RegisterPlanet( Body* body, Shape shape ) -> Visualizer<Func>& { planets_.emplace( std::make_pair(body, shape) ); return *this; }
  auto Visualize(){
    for( auto& pair : planets_ ){
      auto coord = coordinate_tranformation_( pair.first->GetPosition() );
      pair.second.Translate( coord );
      render_engine_.AddToQueue( pair.second );
    }
    render_engine_.Render();
    render_engine_.Notify();
  }
  auto RegisterWindow( Window& w ) -> Visualizer<Func>& { render_engine_.RegisterWindow(w); return *this; }

private:
  RenderEngine render_engine_{};
  std::map<Body*, Shape> planets_{};
  Func coordinate_tranformation_{};
};

}
#endif // VISUALIZER