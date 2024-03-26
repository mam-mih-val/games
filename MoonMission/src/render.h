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
  RenderEngine();
  RenderEngine( const RenderEngine&) = default;
  RenderEngine(RenderEngine&&) noexcept = default;
  RenderEngine& operator=(const RenderEngine&) = default;
  RenderEngine& operator=(RenderEngine&&) noexcept = default;
  
  
  bool UpdateWindow(){ return graphics_.RenderFrame(); }
  
  void Render();
  
  auto MakeCallbackPhysics() -> std::function<void(Physics::PhysicsState)>;
  auto MakeCallbackAnalytics() -> std::function<void(Physics::ComputerState)>;
  auto MakeCallbackGameInput() -> std::function<void(Input::Comands)>;

private:
  void UpdateCamera();
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
  std::vector<Point2D> moon_trajectory_{};
  std::vector<Point2D> earth_trajectory_{};
  
  std::queue<Input::CameraCommand> commands_{};

  Graphics::GraphicEngine graphics_{};
  Graphics::Scene current_frame_{};
  Camera camera_{};
};

}

#endif // RENDER_H

