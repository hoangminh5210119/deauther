#ifndef DeviceSleep_h
#define DeviceSleep_h

#include "A_config.h"
#include "Arduino.h"
#include "ESP8266WiFi.h"

extern "C" {
#include "user_interface.h"
}
extern "C" {
#include "gpio.h"
}

class DeviceSleep {
private:
  /* data */
public:
  DeviceSleep(/* args */);
  ~DeviceSleep();

  void setup();
  void sleep();
};

#endif