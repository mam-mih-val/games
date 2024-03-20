#ifndef RENDER_H
#define RENDER_H

#include <algorithm>
#include <mutex>
#include <utility>
#include <vector>

#include "events.h"
#include "coordinates.h"
#include "graphics.h"
// #include "graphics.h"


namespace Render {

class RenderEngine{
public:
  RenderEngine() {}
  RenderEngine( const RenderEngine&) = default;
  RenderEngine(RenderEngine&&) noexcept = default;
  RenderEngine& operator=(const RenderEngine&) = default;
  RenderEngine& operator=(RenderEngine&&) noexcept = default;
  
  
  bool UpdateWindow(){ 
    return graphics_.RenderFrame(); 
  }
  
  void Render(){
    {
      auto lock = std::lock_guard<std::mutex>( EventManager::physics_state.mutex );
      state_ = EventManager::physics_state.state;
    }
    auto moon = Graphics::MakeMoon( state_.moon_position );    
    auto earth = Graphics::MakeEarth( state_.earth_position );    
    auto rocket = Graphics::MakeRocket( state_.rocket_position );
    
    UpdateCamera();

    current_frame_.AddToFrame( moon );
    current_frame_.AddToFrame( earth );
    current_frame_.AddToFrame( rocket );

    {
      graphics_.SetActive();
      graphics_.DrawBackground();
      graphics_.DrawFrame( current_frame_ );
      graphics_.DrawData();   
      graphics_.Display();
      graphics_.SetInactive();
    }
  }
  
private:
  void UpdateCamera(){ 
    auto lock = std::lock_guard<std::mutex>{EventManager::camera_queue.mutex};
    for( ; !EventManager::camera_queue.events.empty() ; EventManager::camera_queue.events.pop() ){
      switch (EventManager::camera_queue.events.front().command) {
        case CameraEvent::ControlCommand::FOCUS_ON_EARTH:
          SetCameraPosition(state_.earth_position);
          break;
        case CameraEvent::ControlCommand::FOCUS_ON_MOON:
          SetCameraPosition(state_.moon_position);
          break;
        case CameraEvent::ControlCommand::FOCUS_ON_ROCKET:
          SetCameraPosition(state_.rocket_position);
          break;
        case CameraEvent::ControlCommand::ZOOM_IN:
          Zoom(1.01);
          break;
        case CameraEvent::ControlCommand::ZOOM_OUT:
          Zoom(0.99);
          break;
      }
    }
  }
  void Zoom( double val ){ 
    auto [curr_x, curr_y] = current_frame_.GetCamera().getSize();
    auto new_x = static_cast<float>(curr_x*val);
    auto new_y = static_cast<float>(curr_y*val);
    current_frame_.GetCamera().setSize( sf::Vector2<float>{new_x, new_y } );
  }
  void SetCameraPosition( const Point2D& p ){
    auto c_x = static_cast<float>(p.x);
    auto c_y = static_cast<float>(p.y);
    current_frame_.GetCamera().setCenter(c_x, c_y); 
  }
  Graphics::GraphicEngine graphics_{};
  WorldState state_;
  Graphics::Scene current_frame_{};
};

}

#endif // RENDER_H

