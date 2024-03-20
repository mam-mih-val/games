#ifndef PHYSICS_H
#define PHYSICS_H

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <mutex>
#include <queue>
#include <regex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "coordinates.h"
#include "events.h"

namespace Physics{

static constexpr double G = 9.8e-3 * 6.4e3;

struct Body{
  double mass{};
  Point2D position{};
  Point2D velocity{};

  void Update( const Point2D& external_acceleration, double dt ){
    velocity += external_acceleration*dt;
    position += velocity*dt;
  }
};

class Planet{
public:
  Planet() = default;
  ~Planet() = default;

  double GetMass() const { return body_.mass; }
  double GetRadius() const { return radius_; }
  const Point2D& GetPosition() const { return body_.position; }
  const Point2D& GetVelocity() const { return body_.velocity; }
  
  void SetMass(double mass){ body_.mass = mass; }
  void SetRadius( double radius ){ radius_ = radius; }
  void SetPosition(const Point2D& pos){ body_.position = pos; }
  void SetVelocity(const Point2D& vel){ body_.velocity = vel; }

  void Update( const Point2D& external_acceleration, double dt ){
    body_.Update(external_acceleration, dt);
  }

private:
  Body body_{};
  double radius_{};
};

inline double GetOrbitalSpeed( const Planet& p, double orbit ){
  auto p_M = p.GetMass();
  auto speed = sqrt( G*p_M / orbit );
  return speed;
}

inline Point2D GravitationalAcceleration( const Planet& p, const Point2D& position ){
  auto rel_position = position - p.GetPosition();
  auto distance = rel_position.Mag();
  auto direction = rel_position / distance;
  auto accel_mag = G * p.GetMass() / ( distance * distance );
  auto acceleration = direction * accel_mag * -1;
  return acceleration;
}

inline bool CheckCollision(const Planet& p,const Point2D& position ){
  auto rel_pos = ( p.GetPosition() - position ).Mag();
  return rel_pos < p.GetRadius();
}

class Rocket{
public:
  double GetMass() const { return body_.mass + engine_mass_ + fuel_mass_; }
  const Point2D& GetPosition() const { return body_.position; }
  const Point2D& GetVelocity() const { return body_.velocity; }
  double Empty() const { return fuel_mass_ > 0.0; } 

  Point2D GetOrintation() const { return body_.velocity / body_.velocity.Mag(); }
  double GetThrust() const { return !Empty() ? thrust_ / GetMass() : 0.0;   }
  
  void SetBodyMass(double mass){ body_.mass = mass; }
  void SetEngineMass( double val ){ engine_mass_ = val; }  
  void SetFuelMass( double val ){ fuel_mass_ = val; }  

  void SetPosition(const Point2D& pos){ body_.position = pos; }
  void SetVelocity(const Point2D& vel){ body_.velocity = vel; }
  void SetThrust( double val ){ thrust_ = val; }  
  void SetFuelConsumption( double val ){ fuel_consumption_ = val; }  

  void Update( const Point2D& external_acceleration, double dt ){
    body_.Update(external_acceleration, dt);
    auto lock = std::lock_guard<std::mutex>{ EventManager::control_queue.mutex };
    for( ; !EventManager::control_queue.events.empty(); EventManager::control_queue.events.pop() ){
      switch ( EventManager::control_queue.events.front().command ) {
        case ControlEvent::ControlCommand::ACCELERATE:
          Accelerate(1, dt);
          break;
        case ControlEvent::ControlCommand::DECELERATE:
          Accelerate(-1, dt);
          break;
        case ControlEvent::ControlCommand::TURN_UP:
          Steer( {0, -1}, dt);
          break;
        case ControlEvent::ControlCommand::TURN_DOWN:
          Steer( {0, 1}, dt);
          break;
        case ControlEvent::ControlCommand::TURN_LEFT:
          Steer( {-1, 0}, dt);
          break;
        case ControlEvent::ControlCommand::TURN_RIGHT:
          Steer( {1, 0}, dt);
          break;
        default:
          break;
      }
    }
  }
private:
  void Accelerate( double rate, double dt ){
    auto dir = GetOrintation();
    auto thrust = GetThrust();
    fuel_mass_-=fuel_consumption_*dt;
    body_.velocity+=dir*rate*thrust*dt;
    body_.position+=body_.velocity*dt;
  }
  void Steer( Point2D dir, double dt ){
    auto thrust = GetThrust();
    fuel_mass_-=fuel_consumption_*dt;
    body_.velocity+=dir*thrust*dt;
    body_.position+=body_.velocity*dt;
  }

  Body body_;
  double thrust_{};
  double engine_mass_{};
  double fuel_mass_{};
  double fuel_consumption_{};
};

class Physics{
public:
  const Planet& GetEarth() const { return earth_; }
  const Planet& GetMoon() const { return moon_; }
  const Rocket& GetRocket() const { return rocket_; }

  void SetEarth( const Planet& p ) { earth_ = p; }
  void SetMoon( const Planet& p ) { moon_ = p; }
  void SetRocket( const Rocket& r ) { rocket_ = r; }

  void Update( double dt ){
    auto earth_rocket = GravitationalAcceleration(earth_, rocket_.GetPosition());
    auto earth_moon = GravitationalAcceleration(earth_, moon_.GetPosition());
    auto moon_rocket  = GravitationalAcceleration(moon_, rocket_.GetPosition());
    auto moon_earth = GravitationalAcceleration(moon_, earth_.GetPosition());

    earth_.Update(moon_earth, dt);
    moon_.Update(earth_moon, dt);
    rocket_.Update(earth_rocket+moon_rocket, dt);

    auto lock = std::lock_guard<std::mutex>{EventManager::physics_state.mutex};
    EventManager::physics_state.state.earth_position = earth_.GetPosition();
    EventManager::physics_state.state.moon_position = moon_.GetPosition();
    EventManager::physics_state.state.rocket_position = rocket_.GetPosition();
  }
private:
  Planet earth_;
  Planet moon_;
  Rocket rocket_;
};

static Physics MakePhysics(){
  auto earth = Planet();
  earth.SetMass(1);
  earth.SetRadius(64);
  earth.SetPosition( {0.0, 0.0} );
  earth.SetVelocity( {0.0, 0.0} );

  auto moon = Planet();
  moon.SetMass(0.012);
  moon.SetRadius(10);
  moon.SetPosition({3800, 0.0});
  moon.SetVelocity( {0.0, GetOrbitalSpeed(earth, 3800)} );

  auto rocket = Rocket();
  rocket.SetBodyMass(1);
  rocket.SetEngineMass(1);
  rocket.SetThrust(0.01);
  rocket.SetFuelMass(10);
  rocket.SetFuelConsumption(0.01);
  rocket.SetPosition({0.0, -100});
  rocket.SetVelocity({ GetOrbitalSpeed(earth, 100), 0.0 });

  auto phys = Physics();
  phys.SetEarth( earth );
  phys.SetMoon( moon );
  phys.SetRocket( rocket );

  return phys;
}

}
#endif // PHYSICS_H