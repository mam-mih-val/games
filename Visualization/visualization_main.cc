#include "body.h"
#include "camera.h"
#include "camera_control.h"
#include "coordinates.h"
#include "gravitation.h"
#include "pipeline.h"
#include "polygon.h"
#include "shape.h"
#include "window.h"
#include "world.h"
#include <chrono>
#include <thread>

auto main() -> int {
  using namespace Visualization;
  auto physics_engine = World();
  auto earth = new Body{TheEarth};
  auto moon = new Body{TheMoon};
  physics_engine.AddBody(earth);
  physics_engine.AddBody(moon);
  earth->SetPosition( {0.0, 0.0} );
  moon->SetPosition( {380'000., 0} );
  auto moon_speed = sqrt(Gravitation::G / 380'000 );
  moon->SetVelocity( {0., moon_speed} );

  auto earth_shape = Shape().AddPolygon( Polygon{{0.0, 0.0}, size_t(100), 6.4} );
  auto moon_shape = Shape().AddPolygon( Polygon{{0.0, 0.0}, size_t(100), 1.7} );

  auto pipeline = Pipeline{ []( const Point2D& pos ){ return pos/3800; } };
  pipeline.GetVisualizer().RegisterPlanet(earth, earth_shape );
  pipeline.GetVisualizer().RegisterPlanet(moon, moon_shape );

  Camera cam{pipeline.GetWindow()};
  cam.Scale( 1 );
  cam.RegisterWindow( pipeline.GetWindow() );
  cam.NotifyWindow();

  auto mouse = Mouse( *pipeline.GetWindow() );
  auto cam_control = CameraControl();
  cam_control.RegisterCamera(cam);
  cam_control.RegisterMouse(mouse);

  auto thread_phys = std::thread{ [&physics_engine](){
    while (true) {
      physics_engine.Propagate(10);
      std::this_thread::sleep_for(std::chrono::milliseconds{20} );
    }
  } };
  auto thread_vis = std::thread{ [&pipeline, &cam_control, &mouse](){
    while (true) {
      pipeline.Draw();
      mouse.Listen();
      cam_control.Update();
      std::this_thread::sleep_for(std::chrono::milliseconds{20} );
    }
  } };
  
  thread_phys.join();
  thread_vis.join();
  return 0;
}