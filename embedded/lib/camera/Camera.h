#ifndef camera_h
#define camera_h

#include <Arduino.h>

class Camera
{
  public:
    Camera();
    bytes[][] read();

  protected:
    NewPing *sonar;
};

#endif