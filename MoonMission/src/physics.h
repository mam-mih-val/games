#ifndef PHYSICS_H
#define PHYSICS_H

#include <SFML/Graphics/Color.hpp>
#include <algorithm>
#include <cmath>
#include <functional>
#include <future>
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
  Rocket();
  ~Rocket() = default;

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
  void Accelerate( double rate, double dt );
  void Steer( Point2D dir, double dt );

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
  Physics() = default;
  ~Physics() = default;

  const Planet& GetEarth() const { return state_.earth; }
  const Planet& GetMoon() const { return state_.moon; }
  const Rocket& GetRocket() const { return state_.rocket; }
  const PhysicsState& GetState() const { return state_; }
  
  void SetState( const PhysicsState& state ){ state_ = state; }
  void SetEarth( const Planet& p ) { state_.earth = p; }
  void SetMoon( const Planet& p ) { state_.moon = p; }
  void SetRocket( const Rocket& r ) { state_.rocket = r; }

  void Update( double dt );
  
  void AddCommand(Input::ComputerCommand command){
    commands_.push(command);
  }
  bool Idle(){ return commands_.empty(); }
private:
  static std::unique_ptr<Physics> instance_;
  void ExecuteCommand(double dt);
  PhysicsState state_{};
  std::queue<Input::ComputerCommand> commands_;
};

Physics MakePhysics();

struct ComputerState{
  std::vector<Point2D> rocket_trajectory_{};
  std::vector<Point2D> earth_trajectory_{};
  std::vector<Point2D> moon_trajectory_{};
};

class Computer{
public:
  Computer( const Physics& phys ) : initial_state_{phys.GetState()} {  }
  void AddCommand( Input::ComputerCommand command ){ commands_.push_back(command); }
  void Analyze(double dt, std::vector<Input::ComputerCommand> comands);
  void Sync( const PhysicsState& state ){ initial_state_ = state; }
  void ClearCommands(){ commands_.clear(); }
  const auto& GetState() const { return state_; }
  void RegisterCallback(const std::function<void( ComputerState )>& function){ callbacks_.push_back( function ); }

private:
  void ClearTrajectories();
  PhysicsState initial_state_{};
  Physics physics_engine_{};
  ComputerState state_{};
  std::vector<Input::ComputerCommand> commands_;
  std::vector< std::function<void( ComputerState )> > callbacks_{};
  
  std::function<void( std::vector<Input::ComputerCommand> )> callback_physics_;
};

class GameControl{
public:
  GameControl( const Physics& phys ) : physics_{phys}, computer_(phys) {}
  void UpdatePhysics( double dt ){ physics_.Update(dt); NotifyPhysics(); }
  void UpdateAnalysis(double dt){ computer_.Analyze(dt, computer_commands_); NotifyComputer(); }
  void Execute(){ std::for_each( computer_commands_.begin(), computer_commands_.end(), [this](const auto& c){ physics_.AddCommand( c ); }); computer_commands_.clear(); }
  auto MakeCallbackGameInput() -> std::function<void(Input::Comands)>;
  void RegisterPhysicsCallback( const std::function<void(PhysicsState)>& function ){ physics_callbacks.push_back( function ); }
  void RegisterComputerCallback( const std::function<void(ComputerState)>& function ){ computer_callbacks.push_back( function ); }
private:
  void NotifyPhysics(){
    std::for_each( physics_callbacks.begin(), physics_callbacks.end(), [this]( const auto& c ){ c( physics_.GetState() ); } );
  }
  void NotifyComputer(){
    std::for_each( computer_callbacks.begin(), computer_callbacks.end(), [this]( const auto& c ){ c( computer_.GetState() ); } );
  }
  Physics physics_;
  Computer computer_;
  std::vector<Input::ComputerCommand> computer_commands_{};
  std::vector<std::function<void(PhysicsState)>> physics_callbacks{};
  std::vector<std::function<void(ComputerState)>> computer_callbacks{};
};
 
}
#endif // PHYSICS_H