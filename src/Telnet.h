#ifndef Telnet_h
#define Telnet_h

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <FS.h>
extern "C" {
#include "user_interface.h"
}
#include "A_config.h"
// #include "CLI.h"

#define MAX_TELNET_CLIENTS 2
#define TELNET_PORT 23

// extern CLI cli;

class Telnet {
private:
  /* data */
  char did[13];
  String data;

public:
  Telnet(/* args */);
  ~Telnet();

  void setup();
  String update();

  // String value();

  void println(String text);
  void print(String text);

  void println(const char *ptr);
  void print(const char *ptr);

  void println(char c);
  void print(char c);

  void println(int i);
  void print(int i);

  void print_tnet_new_line();
};

#endif