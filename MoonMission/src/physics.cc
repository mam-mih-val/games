
#include "physics.h"
#include "input.h"

namespace Physics{

void Rocket::Accelerate( double rate, double dt ){
  if( Empty() ){
    std::cout << "Empty" << std::endl;
    return;
  }
  auto dir = GetOrientation();
  auto thrust = GetThrust();
  fuel_mass_-=fuel_consumption_*dt;
  auto acceleration = dir*thrust*rate;
  body_.Update(acceleration, dt);
}

void Rocket::Steer( Point2D dir, double dt ){
  if( Empty() ){
    std::cout << "Empty" << std::endl;
    return;
  }
  auto thrust = GetThrust();
  auto acceleration = dir*thrust;
  fuel_mass_-=fuel_consumption_*dt;
  body_.Update(acceleration, dt);
}

void Physics::Update( double dt ){
  auto earth_rocket = GravitationalAcceleration(state_.earth, state_.rocket.GetPosition());
  auto earth_moon = GravitationalAcceleration(state_.earth, state_.moon.GetPosition());
  auto moon_rocket  = GravitationalAcceleration(state_.moon, state_.rocket.GetPosition());
  auto moon_earth = GravitationalAcceleration(state_.moon, state_.earth.GetPosition());

  state_.earth.Update(moon_earth, dt);
  state_.moon.Update(earth_moon, dt);
  state_.rocket.Update(earth_rocket+moon_rocket, dt);

  ExecuteCommand(dt);
}

void Physics::ExecuteCommand(double dt){
  using namespace Input;
  if( Idle() )
    return;
  std::cout << commands_.front().time << "\n";
  switch ( commands_.front().command ) {
    case ComputerCommand::CommandType::ACCELERATE:
      state_.rocket.Accelerate(1, dt);
      break;
    case ComputerCommand::CommandType::DECELERATE:
      state_.rocket.Accelerate(-1, dt);
      break;
    case ComputerCommand::CommandType::STEER_UP:
      state_.rocket.Steer( {0, -1}, dt);
      break;
    case ComputerCommand::CommandType::STEER_DOWN:
      state_.rocket.Steer( {0, 1}, dt);
      break;
    case ComputerCommand::CommandType::STEER_LEFT:
      state_.rocket.Steer( {-1, 0}, dt);
      break;
    case ComputerCommand::CommandType::STEER_RIGHT:
      state_.rocket.Steer( {1, 0}, dt);
      break;
    case ComputerCommand::CommandType::NOTHING:
      break;
    default:
      break;
  }
  commands_.front().time -= dt;
  if( commands_.front().time <= 0 )
    commands_.pop();
}

Physics MakePhysics(){
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

void Computer::Analyze(double dt, std::vector<Input::ComputerCommand> comands){
    ClearTrajectories();
    physics_engine_.SetState(initial_state_);
    for( auto c : comands ){
      physics_engine_.AddCommand( c );
    }
    while( !physics_engine_.Idle() ){
      physics_engine_.Update(dt);
      state_.earth_trajectory_.push_back( physics_engine_.GetEarth().GetPosition());
      state_.moon_trajectory_.push_back( physics_engine_.GetMoon().GetPosition());
      state_.rocket_trajectory_.push_back( physics_engine_.GetRocket().GetPosition());
    }
  }

void Computer::ClearTrajectories(){ 
  state_.earth_trajectory_.clear();
  state_.moon_trajectory_.clear();
  state_.rocket_trajectory_.clear(); 
}

auto GameControl::MakeCallbackGameInput() -> std::function<void(Input::Comands)> {
  return [this](Input::Comands commands){
    if( commands.computer_command.command == Input::ComputerCommand::CommandType::EMPTY_COMMAND ){
      return;
    }
    if( commands.computer_command.command == Input::ComputerCommand::CommandType::SYNC ){
      computer_.Sync( physics_.GetState() );
      return;
    }
    if( commands.computer_command.command == Input::ComputerCommand::CommandType::EXECUTE ){
      Execute();
      computer_commands_.clear();
      return;
    }
    if( commands.computer_command.command == Input::ComputerCommand::CommandType::CLEAR ){
      computer_commands_.clear();
      return;
    }
    if( commands.computer_command.command == Input::ComputerCommand::CommandType::REMOVE_LAST ){
      computer_commands_.pop_back();
      return;
    }
    computer_commands_.push_back(commands.computer_command);
  };
}

}