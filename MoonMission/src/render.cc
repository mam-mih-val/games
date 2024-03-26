#include "render.h"

namespace Render {

RenderEngine::RenderEngine() {
  moon_img_ = Graphics::MakeMoon( moon_state_.position );    
  earth_img_ = Graphics::MakeEarth( earth_state_.position );    
  rocket_img_ = Graphics::MakeRocket( rocket_state_.position );
  FollowEarth();
}

void RenderEngine::Render(){
  earth_img_.SetPosition( earth_state_.position );
  moon_img_.SetPosition( moon_state_.position );
  rocket_img_.SetPosition( rocket_state_.position );
  rocket_img_.Align( rocket_state_.velocity / rocket_state_.velocity.Mag() );
  
  UpdateCamera();

  for( const auto& point : rocket_trajectory_ ){
    auto p = Graphics::MakePlanet(point, 2);
    current_frame_.AddToFrame(p);
  }
  for( const auto& point : moon_trajectory_ ){
    auto p = Graphics::MakePlanet(point, 2);
    current_frame_.AddToFrame(p);
  }
  current_frame_.AddToFrame( moon_img_);
  current_frame_.AddToFrame( earth_img_ );
  current_frame_.AddToFrame( rocket_img_ );
  {
    graphics_.SetActive();
    graphics_.DrawBackground();
    graphics_.DrawFrame( current_frame_ );
    graphics_.DrawData();   
    graphics_.Display();
    graphics_.SetInactive();
  }
}

auto RenderEngine::MakeCallbackPhysics() -> std::function<void(Physics::PhysicsState)> {
  return [this]( Physics::PhysicsState state ){
    earth_state_ = state.earth.GetState();
    moon_state_ = state.moon.GetState();
    rocket_state_ = state.rocket.GetState();
  };
}
auto RenderEngine::MakeCallbackAnalytics() -> std::function<void(Physics::ComputerState)> {
  return [this]( Physics::ComputerState computer_state ){
    rocket_trajectory_ = computer_state.rocket_trajectory_;
    moon_trajectory_ = computer_state.moon_trajectory_;
    earth_trajectory_ = computer_state.earth_trajectory_;
  };
}

auto RenderEngine::MakeCallbackGameInput() -> std::function<void(Input::Comands)> {
  return [this]( Input::Comands comands ){
    commands_.push( comands.camera_command );
  };
}

void RenderEngine::UpdateCamera(){ 
  using namespace Input;
  for( ; !commands_.empty() ; commands_.pop() ){
    switch ( commands_.front().command ) {
      case CameraCommand::CommandType::FOCUS_ON_EARTH:
        std::cout << "focus on Earth" << std::endl;
        FollowEarth();
        break;
      case CameraCommand::CommandType::FOCUS_ON_MOON:
        std::cout << "focus on Moon" << std::endl;
        FollowMoon();
        break;
      case CameraCommand::CommandType::FOCUS_ON_ROCKET:
        FollowRocket();
        break;
      case CameraCommand::CommandType::ZOOM_IN:
        Zoom(0.99);
        break;
      case CameraCommand::CommandType::ZOOM_OUT:
        Zoom(1.01);
        break;
      default:
        break;
    }
  }
  camera_.Update();
  current_frame_.SetView( camera_.GetView() );
}

}