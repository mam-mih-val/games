#ifndef PHYSICS_H
#define PHYSICS_H

#include <algorithm>
#include <cmath>
#include <functional>
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
#include "window.h"
#include "input_commands.h"
#include "input.h"

namespace Physics{

static constexpr double G = 9.8e-3 * 6.4e3;

struct Body{
  double mass{};
  Point2D position{};
  Point2D velocity{};
  Point2D orientation{0.0, -1.0};

  void Update( const Point2D& external_acceleration, double dt ){
    velocity += external_acceleration*dt;
    position += velocity*dt;
    orientation = velocity/velocity.Mag();
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
  const Point2D& GetOrientation() const { return body_.orientation; }

  Body GetState(){ return body_; }
  
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
  const Point2D& GetOrientation() const { return body_.orientation; }

  double Empty() const { return fuel_mass_ < 0.0; } 

  Point2D GetOrintation() const { return body_.velocity / body_.velocity.Mag(); }
  double GetThrust() const { return !Empty() ? thrust_ / GetMass() : 0.0;   }
  double GetFuelMass() const { return fuel_mass_; }
  
  Body GetState(){ return body_; }

  void SetBodyMass(double mass){ body_.mass = mass; }
  void SetEngineMass( double val ){ engine_mass_ = val; }  
  void SetFuelMass( double val ){ fuel_mass_ = val; }  

  void SetPosition(const Point2D& pos){ body_.position = pos; }
  void SetVelocity(const Point2D& vel){ body_.velocity = vel; }
  void SetThrust( double val ){ thrust_ = val; }  
  void SetFuelConsumption( double val ){ fuel_consumption_ = val; } 


  void Update( const Point2D& external_acceleration, double dt ){
    body_.Update(external_acceleration, dt);
  }
  void Accelerate( double rate, double dt ){
    if( Empty() ){
      std::cout << "Empty" << std::endl;
      return;
    }
    auto dir = GetOrintation();
    auto thrust = GetThrust();
    fuel_mass_-=fuel_consumption_*dt;
    std::cout << fuel_mass_ << std::endl;
    auto acceleration = dir*thrust*rate;
    body_.Update(acceleration, dt);
  }
  void Steer( Point2D dir, double dt ){
    if( Empty() ){
      std::cout << "Empty" << std::endl;
      return;
    }
    auto thrust = GetThrust();
    auto acceleration = dir*thrust;
    fuel_mass_-=fuel_consumption_*dt;
    std::cout << fuel_mass_ << std::endl;
    body_.Update(acceleration, dt);
  }

private:
  Body body_;
  double thrust_{};
  double engine_mass_{};
  double fuel_mass_{};
  double fuel_consumption_{};
};

struct PhysicsState{
  Planet earth;
  Planet moon;
  Rocket rocket;
};

class Physics{
public:
  const Planet& GetEarth() const { return state_.earth; }
  const Planet& GetMoon() const { return state_.moon; }
  const Rocket& GetRocket() const { return state_.rocket; }

  void SetEarth( const Planet& p ) { state_.earth = p; }
  void SetMoon( const Planet& p ) { state_.moon = p; }
  void SetRocket( const Rocket& r ) { state_.rocket = r; }

  void Update( double dt ){
    auto earth_rocket = GravitationalAcceleration(state_.earth, state_.rocket.GetPosition());
    auto earth_moon = GravitationalAcceleration(state_.earth, state_.moon.GetPosition());
    auto moon_rocket  = GravitationalAcceleration(state_.moon, state_.rocket.GetPosition());
    auto moon_earth = GravitationalAcceleration(state_.moon, state_.earth.GetPosition());

    state_.earth.Update(moon_earth, dt);
    state_.moon.Update(earth_moon, dt);
    state_.rocket.Update(earth_rocket+moon_rocket, dt);

    for( ; !commands_.empty(); commands_.pop() ){
      switch ( commands_.front().command ) {
        case SteerCommand::CommandType::ACCELERATE:
          state_.rocket.Accelerate(1, dt);
          break;
        case SteerCommand::CommandType::DECELERATE:
          state_.rocket.Accelerate(-1, dt);
          break;
        case SteerCommand::CommandType::STEER_UP:
          state_.rocket.Steer( {0, -1}, dt);
          break;
        case SteerCommand::CommandType::STEER_DOWN:
          state_.rocket.Steer( {0, 1}, dt);
          break;
        case SteerCommand::CommandType::STEER_LEFT:
          state_.rocket.Steer( {-1, 0}, dt);
          break;
        case SteerCommand::CommandType::STEER_RIGHT:
          state_.rocket.Steer( {1, 0}, dt);
          break;
        default:
          break;
      }
    }

    Notify();
  }
  void RegisterCallback( const std::function<void(PhysicsState state)>& function ) { callbacks_.push_back(function); }

  auto MakeCallbackGameInput(){
    return [this]( Comands comands ) {
      commands_.push( comands.steer_command );
    };
  }

private:
  void Notify(){
    std::for_each( callbacks_.begin(), callbacks_.end(), [this](const auto& c){
        c( state_ );
    } );
  }
  PhysicsState state_{};
  std::queue<SteerCommand> commands_{};
  std::vector< std::function<void(PhysicsState state)> > callbacks_{};
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
  rocket.SetFuelConsumption(0.001);
  rocket.SetPosition({0.0, -100});
  rocket.SetVelocity({ GetOrbitalSpeed(earth, 100), 0.0 });

  auto phys = Physics();
  phys.SetEarth( earth );
  phys.SetMoon( moon );
  phys.SetRocket( rocket );

  return phys;
}

double CalculateV2( const Planet& planet, const Rocket& rocket ){
  auto distance = (rocket.GetPosition() - planet.GetPosition()).Mag();
  auto mass = planet.GetMass();
  auto v2 = sqrt( 2 * G * mass / distance );
  return v2;
}

class Analytics{
public:
  Analytics( const Physics phys ) :
  earth_( phys.GetEarth() ),
  moon_( phys.GetMoon() ),
  rocket_( phys.GetRocket() ) {}
  
  void Calculate( double dt, size_t n_steps ){
    auto distance = (rocket_.GetPosition() - earth_.GetPosition()).Mag();
    auto rocket_trajectory= std::vector<Point2D>{};
    rocket_trajectory.reserve(n_steps);
    for( size_t i=0; i<n_steps; ++i ){
      Propagate(dt);  
      rocket_trajectory.push_back( rocket_.GetPosition() );
    }
    auto v2 = sqrt( 2 * G * earth_.GetMass() / distance );
    std::cout << "v: " << rocket_.GetVelocity().Mag() << " v2: " << v2 << std::endl;
    Notify( rocket_trajectory );
  }
  auto MakeCallbackPhysics(){
    return [this]( PhysicsState state ){
      earth_ = state.earth;
      moon_ = state.moon;
      rocket_ = state.rocket;
    };
  }
  void RegisterCallback( const std::function<void(std::vector<Point2D>)>& function ) { callbacks_.push_back(function); }
private:
  void Notify( std::vector<Point2D> trajectory ){
    std::for_each( callbacks_.begin(), callbacks_.end(), [trajectory](const auto& c){
        c( trajectory );
    });
  }
  void Propagate( double dt ){
    auto earth_rocket = GravitationalAcceleration(earth_, rocket_.GetPosition());
    auto earth_moon = GravitationalAcceleration(earth_, moon_.GetPosition());
    auto moon_rocket  = GravitationalAcceleration(moon_, rocket_.GetPosition());
    auto moon_earth = GravitationalAcceleration(moon_, earth_.GetPosition());

    earth_.Update(moon_earth, dt);
    moon_.Update(earth_moon, dt);
    rocket_.Update(earth_rocket+moon_rocket, dt);
  }

  Planet earth_;
  Planet moon_;
  Rocket rocket_;

  std::vector< std::function<void( std::vector<Point2D> )> > callbacks_{};

};

}
#endif // PHYSICS_H