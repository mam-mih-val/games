#ifndef CAMERA_H
#define CAMERA_H

#include <SFML/Graphics/View.hpp>

#include "coordinates.h"
#include "window.h"

class Camera{
public:
  Camera(Window& w) { 
    GenerateNotification( w );
    view_.setCenter( Point2D{0.0, 0.0}.SfVector() );
    view_.setSize( Point2D{(*w).getSize()}.SfVector() );
  };

  auto MoveCamera( const Point2D& where_to ) -> void{ view_.setCenter( where_to.SfVector() ); }
  auto Resize( const Point2D& new_size ) -> void { view_.setSize( new_size.SfVector() ); }
  auto Scale( double scale ){ 
    auto size = GetSize();
    size*= scale;
    Resize(size);
  }
  auto RegisterWindow( Window &w ){ window_notification_ = GenerateNotification(w); }
  auto NotifyWindow(){ window_notification_(); }

  auto GetPoistion() -> Point2D { return Point2D(view_.getCenter()); }
  auto GetSize() -> Point2D { return Point2D(view_.getSize()); }

private:
  auto GenerateNotification( Window& w ) -> std::function<void(void)> { return [&w, this]() mutable{ w.SetView( view_ ); }; }
  sf::View view_{};
  std::function<void(void)> window_notification_{};
};

#endif // CAMERA_H