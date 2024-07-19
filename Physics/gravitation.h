#ifndef GRAVITATION_H
#define GRAVITATION_H

#include "body.h"
#include "coordinates.h"
#include <limits>
#include <memory>

class Gravitation{
public:
  static constexpr double G = 6371 * 6371; 
  static auto MoveBody( const double dt, Body* body, const std::vector< std::unique_ptr<Body> >& attractors ) -> void{
    std::for_each( attractors.begin(), attractors.end(), [dt, body]( const auto& a ) mutable { 
      if( a.get() == body )
        return;
      MoveBody(dt, body, a.get()); 
    } );
  }
  static auto MoveBody( const double dt, Body* body, Body* attractor ) -> void {
    auto distance = Distance( body->GetPosition(), attractor->GetPosition() );
    if( distance < std::numeric_limits<double>::min() )
      return;
    auto attraction_force = AttractionAcceleration(body, attractor);
    auto dv = attraction_force*dt;
    auto vn = body->GetVelocity();
    auto vnp1 = vn + dv;
    body->SetVelocity( vnp1 );
    auto xn = body->GetPosition();
    auto dx = vnp1 * dt;
    auto xnp1 = xn + dx;
    body->SetPosition(xnp1);
  }
// private:
  static auto AttractionMag(Body* body, Body* attractor) noexcept -> double {
    auto pos1 = body->GetPosition();
    auto pos2 = attractor->GetPosition();
    auto mass = attractor->GetMass();
    auto d = Distance( pos1, pos2 );
    auto force = G * mass / d / d;
    return force;
  }
  static auto AttractionAcceleration( Body* body, Body* attractor ) noexcept -> Point2D {
    auto point_vector = Direction( body->GetPosition(), attractor->GetPosition() );
    auto force_mag = AttractionMag(body, attractor);
    return point_vector * force_mag;
  }
  
};

#endif // GRAVITATION_H