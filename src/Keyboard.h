#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "A_config.h"
#include "Arduino.h"
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
#include <SimpleButton.h>
#include <Wire.h> // Only needed for Arduino 1.6.5 and earlier

using namespace simplebutton;

class Keyboard {
private:
  enum KEY_TYPE { LOWERCASE, UPPERCASE, NUMBER1, NUMBER2 };
  enum KEY_FUNC { TYPE_KEY, SPACE, DELETE, ENTER, NONE };

  DEAUTHER_DISPLAY;

  int16_t hight_screen = 64;
  int16_t width_screen = 128;

  int widthBtn = 12;
  int heightBtn = 12;

  Button *up = NULL;
  Button *down = NULL;
  Button *a = NULL;

  int cursor_time_out = 1000;
  unsigned long old_time_cursor = millis();
  bool isShowCursor = true;
  int col = 0;
  int row = 0;
  int maxCharInLine = 9;

  String text = String();

  int key_type = KEY_TYPE::LOWERCASE;
  int funckey = KEY_FUNC::NONE;

  String selectKey = String();

  bool _exit = false;

  void initLCD();
  void buttonUpdate();
  void updateCursor();

public:
  Keyboard(/* args */);
  ~Keyboard();
  void init();
  String show();
  String show(String placeholder);
  bool isNumber(String string);
};

#endif