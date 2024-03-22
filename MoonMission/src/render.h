#ifndef RENDER_H
#define RENDER_H

#include <SFML/Graphics/View.hpp>
#include <algorithm>
#include <cerrno>
#include <mutex>
#include <queue>
#include <utility>
#include <vector>

#include "window.h"
#include "coordinates.h"
#include "graphics.h"
#include "input_commands.h"
#include "physics.h"
#include "input.h"
// #include "graphics.h"


namespace Render {

class Camera{
public:
  Camera() = default;
  ~Camera() = default;
  const Point2D& GetSize() const { return size_; }
  void SetSize( const Point2D& size ) { size_ = size; }
  void SetFocus( std::function<Point2D(void)> function ){ focus_function_ = function; }
  void Update(){ 
    auto [x, y] = focus_function_();
    camera_.setCenter( x, y );
    camera_.setSize( size_.x, size_.y );
  }
  const sf::View& GetView() const { return camera_; }
private:
  sf::View camera_;
  Point2D position_{};
  Point2D size_{800, 800};
  std::function<Point2D(void)> focus_function_;
};

class RenderEngine{
public:
  RenderEngine() {
    moon_img_ = Graphics::MakeMoon( moon_state_.position );    
    earth_img_ = Graphics::MakeEarth( earth_state_.position );    
    rocket_img_ = Graphics::MakeRocket( rocket_state_.position );
    FollowEarth();
  }
  RenderEngine( const RenderEngine&) = default;
  RenderEngine(RenderEngine&&) noexcept = default;
  RenderEngine& operator=(const RenderEngine&) = default;
  RenderEngine& operator=(RenderEngine&&) noexcept = default;
  
  
  bool UpdateWindow(){ 
    return graphics_.RenderFrame(); 
  }
  
  void Render(){
    earth_img_.SetPosition( earth_state_.position );
    moon_img_.SetPosition( moon_state_.position );
    rocket_img_.SetPosition( rocket_state_.position );
    rocket_img_.Align( rocket_state_.velocity / rocket_state_.velocity.Mag() );
    
    UpdateCamera();

    for( const auto& point : rocket_trajectory_ ){
      auto p = Graphics::MakePlanet(point, 5);
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
  
  auto MakeCallbackPhysics(){
    return [this]( Physics::PhysicsState state ){
      earth_state_ = state.earth.GetState();
      moon_state_ = state.moon.GetState();
      rocket_state_ = state.rocket.GetState();
    };
  }
  auto MakeCallbackAnalytics(){
    return [this]( std::vector<Point2D> trajectory ){
      rocket_trajectory_ = trajectory;
    };
  }
  auto MakeCallbackGameInput(){
    return [this]( Comands comands ){
      commands_.push( comands.camera_command );
    };
  }

private:
  void UpdateCamera(){ 
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
  void FollowEarth(){ camera_.SetFocus([this](){ return earth_state_.position; }); }
  void FollowMoon(){ camera_.SetFocus([this](){ return moon_state_.position; }); }
  void FollowRocket(){ camera_.SetFocus([this](){ return rocket_state_.position; }); }
  void Zoom( double val ){ camera_.SetSize( camera_.GetSize()*val ); }

  Graphics::Composition earth_img_;
  Graphics::Composition moon_img_;
  Graphics::Composition rocket_img_;

  Physics::Body earth_state_{};
  Physics::Body moon_state_{};
  Physics::Body rocket_state_{};

  std::vector<Point2D> rocket_trajectory_{};
  
  std::queue<CameraCommand> commands_{};

  Graphics::GraphicEngine graphics_{};
  Graphics::Scene current_frame_{};
  Camera camera_{};
};

}

#endif // RENDER_H

