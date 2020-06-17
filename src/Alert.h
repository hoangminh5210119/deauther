#ifndef ALERT_H
#define ALERT_H
#include "A_config.h"
#include "Arduino.h"
#include <SimpleButton.h>

using namespace simplebutton;

class Alert {
  enum ALERT_BUTTON { OK, CANCEL, CHOOSE };

private:
  /* data */
  DEAUTHER_DISPLAY;
  void initLCD();
  void buttonUpdate();

  Button *b_up = NULL;
  Button *b_down = NULL;
  Button *b_a = NULL;
  uint8_t key = ALERT_BUTTON::OK;
  int screenWidth = 128;
  int screenHeight = 64;
  int imageWidth = 32;
  int imageHeight = 32;

public:
  Alert(/* args */);
  ~Alert();
  void setupButton();
  bool alertOptions(String title, String message, String btn1, String btn2);
  bool alertNotify(String title, String message, String btn);
  void showSuccess(String text);
  void showFailure(String text);
};

#endif