#ifndef INPUT_COMMANDS_H
#define  INPUT_COMMANDS_H

struct SteerCommand{
  enum class CommandType{
    ACCELERATE,
    DECELERATE,
    STEER_UP,
    STEER_DOWN,
    STEER_LEFT,
    STEER_RIGHT,
    NOTHING
  };
  CommandType command;
};

struct CameraCommand{
  enum class CommandType{
    ZOOM_IN,
    ZOOM_OUT,
    FOCUS_ON_EARTH,
    FOCUS_ON_MOON,
    FOCUS_ON_ROCKET,
    NOTHING
  };
  CommandType command;
};

#endif // INPUT_COMMANDS_H
