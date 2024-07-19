#ifndef CAMERA_CONTROL_H
#define CAMERA_CONTROL_H

#include <array>
#include <functional>

#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Window.hpp>
#include <vector>

#include "coordinates.h"
#include "camera.h"
#include "mouse.h"


class CameraControl{
public:
  CameraControl() = default;
  auto Update(){
    mouse_request_();
    if( mouse_events_.size() < 2 )
      return;
    auto ev_1 = mouse_events_.at( mouse_events_.size() - 2 );
    auto ev_2 = mouse_events_.at( mouse_events_.size() - 1 );
    if( ev_1.what != MouseEvent::EventType::LEFT_CLICK || ev_2.what != MouseEvent::EventType::LEFT_CLICK )
      return;
    camera_request_();
    auto translation = ev_2.position - ev_1.position;
    // translation*=0.1;
    auto new_position = camera_position_ - translation;
    camera_position_ = new_position;
    camera_callback_();
  }
  auto RegisterCamera(Camera& cam){ 
    camera_callback_ = GenerateCameraNotification(cam); 
    camera_request_ = GenerateCameraRequest(cam);
  }
  auto RegisterMouse( Mouse& mouse ){
    mouse_request_ = GenerateMouseRequest(mouse);
  }
  auto SetMouseEvents( std::vector<MouseEvent> events ){ mouse_events_.swap(events); }
private:
  auto GenerateCameraNotification(Camera& cam) -> std::function<void(void)> {
    return [&cam, this](){ 
      cam.MoveCamera( camera_position_ );
      cam.Resize( camera_size_ );
      cam.NotifyWindow();
    };
  }
  auto GenerateCameraRequest( Camera& cam ) -> std::function<void(void)> {
    return [&cam, this](){ 
      camera_position_ = cam.GetPoistion();
      camera_size_ = cam.GetSize();
    };
  }
  auto GenerateMouseRequest( Mouse& mouse ) -> std::function<void(void)> {
    return [&mouse, this](){ mouse_events_ = mouse.GetEvents(); };
  }
  Point2D camera_position_;
  Point2D camera_size_;
  std::vector<MouseEvent> mouse_events_{};
  std::function<void(void)> camera_callback_{};
  std::function<void(void)> camera_request_{};
  std::function<void(void)> mouse_request_{};
};

#endif