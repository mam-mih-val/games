#ifndef SCENE_H
#define SCENE_H

#include <algorithm>
#include <list>
#include <memory>

#include "shape.h"
#include "render_engine.h"
#include "window.h"

class Scene{
public:
  Scene() = default;
  auto RegisterWindow( Window& w ) noexcept -> void { render_engine_.RegisterWindow(w); }
  auto AddShape( Shape* s ){ shapes_.emplace_back(s); }
  auto AddShape( std::unique_ptr<Shape>&& s ){ shapes_.emplace_back( std::move(s) ); }
  auto Update(){
    std::for_each( shapes_.begin(), shapes_.end(), [this]( const auto& s ){ render_engine_.AddToQueue( *s ); } );
    render_engine_.Render();
    render_engine_.Notify();
  }
private:
  std::list<std::unique_ptr<Shape>> shapes_{};
  RenderEngine render_engine_;
};

#endif // SCENE_H