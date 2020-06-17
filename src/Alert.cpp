#include "Alert.h"
#include "images.h"

Alert::Alert(/* args */) {}

Alert::~Alert() {}

void Alert::initLCD() {
  display.init();
#ifndef FLIP_DIPLAY
  display.flipScreenVertically();
#endif // ifndef FLIP_DIPLAY
  display.clear();
  display.setColor(WHITE);
  display.display();
}

bool Alert::alertOptions(String title, String message, String btn1,
                         String btn2) {
  key = ALERT_BUTTON::OK;
  bool choose = true;
  initLCD();
  while (1) {
    buttonUpdate();
    display.clear();

    int widthBtn =
        std::max(display.getStringWidth(btn1), display.getStringWidth(btn2));
    widthBtn = widthBtn + widthBtn / 2;
    int hightBtn = 14;
    int topBtn = 45;
    display.setColor(WHITE);
    display.drawString(
        display.getWidth() / 2 - display.getStringWidth(title) / 2, 0, title);

    display.drawString(display.getWidth() / 2 -
                           display.getStringWidth(message) / 2,
                       15, message);

    if (key == ALERT_BUTTON::OK) {
      key = ALERT_BUTTON::OK;
      choose = true;

      display.setColor(WHITE);
      display.drawFillRoundRect(display.getWidth() / 4 - widthBtn / 2, topBtn,
                                widthBtn, hightBtn);

      display.drawString(display.getWidth() / 4 + display.getWidth() / 2 -
                             display.getStringWidth(btn2) / 2,
                         topBtn, btn2);

      display.drawRoundRect(display.getWidth() / 4 + display.getWidth() / 2 -
                                widthBtn / 2,
                            topBtn, widthBtn, hightBtn);

      display.setColor(BLACK);
      display.drawString(display.getWidth() / 4 -
                             display.getStringWidth(btn1) / 2,
                         topBtn, btn1);

    } else if (key == ALERT_BUTTON::CANCEL) {
      key = ALERT_BUTTON::CANCEL;
      choose = false;
      display.setColor(WHITE);
      display.drawString(display.getWidth() / 4 -
                             display.getStringWidth(btn1) / 2,
                         topBtn, btn1);

      display.drawRoundRect(display.getWidth() / 4 - widthBtn / 2, topBtn,
                            widthBtn, hightBtn);

      display.drawFillRoundRect(display.getWidth() / 4 +
                                    display.getWidth() / 2 - widthBtn / 2,
                                topBtn, widthBtn, hightBtn);

      display.setColor(BLACK);
      display.drawString(display.getWidth() / 4 + display.getWidth() / 2 -
                             display.getStringWidth(btn2) / 2,
                         topBtn, btn2);
    } else if (key == ALERT_BUTTON::CHOOSE) {
      key = ALERT_BUTTON::OK;
      // display.clear();
      return choose;
    }

    display.display();
    delay(0);
  }
}

bool Alert::alertNotify(String title, String message, String btn) {
  key = ALERT_BUTTON::OK;
  initLCD();
  while (1) {
    buttonUpdate();
    display.clear();
    int widthBtn = 60;
    widthBtn = display.getStringWidth(btn);
    widthBtn = widthBtn + widthBtn / 2;

    if (widthBtn < 60) {
      widthBtn = 60;
    }

    int hightBtn = 14;
    int topBtn = 45;
    display.setColor(WHITE);
    display.drawString(
        display.getWidth() / 2 - display.getStringWidth(title) / 2, 0, title);
    display.drawString(display.getWidth() / 2 -
                           display.getStringWidth(message) / 2,
                       15, message);
    display.drawFillRoundRect(display.getWidth() / 2 - widthBtn / 2, topBtn,
                              widthBtn, hightBtn);
    display.setColor(BLACK);
    display.drawString(display.getWidth() / 2 - display.getStringWidth(btn) / 2,
                       topBtn, btn);

    if (key == ALERT_BUTTON::CHOOSE) {
      key = ALERT_BUTTON::OK;
      return true;
    }
    display.display();
    delay(0);
  }
}

void Alert::showSuccess(String text) {
  initLCD();
  for (int i = 0; i < sizeof(checkmark_ok_bits) / sizeof(checkmark_ok_bits[0]);
       i++) {
    display.clear();
    display.drawXbm(screenWidth / 2 - imageWidth / 2,
                    screenHeight / 2 - imageHeight / 2, imageWidth, imageHeight,
                    checkmark_ok_bits[i]);

    display.drawString(
        display.getWidth() / 2 - display.getStringWidth(text) / 2, 50, text);
    display.display();
    delay(50);
  }
}

void Alert::showFailure(String text) {
  initLCD();
  for (int i = 0; i < sizeof(failure_bit) / sizeof(failure_bit[0]);
       i++) {
    display.clear();
    display.drawXbm(screenWidth / 2 - imageWidth / 2,
                    screenHeight / 2 - imageHeight / 2, imageWidth, imageHeight,
                    failure_bit[i]);

    display.drawString(
        display.getWidth() / 2 - display.getStringWidth(text) / 2, 50, text);
    display.display();
    delay(50);
  }
}

void Alert::buttonUpdate() {
  b_up->update();
  b_down->update();
  b_a->update();
}

void Alert::setupButton() {
  b_up = new ButtonPullup(BUTTON_UP);
  b_down = new ButtonPullup(BUTTON_DOWN);
  b_a = new ButtonPullup(BUTTON_A);

  b_up->setOnClicked([this]() { key = ALERT_BUTTON::OK; });
  b_down->setOnClicked([this]() { key = ALERT_BUTTON::CANCEL; });
  b_a->setOnClicked([this]() { key = ALERT_BUTTON::CHOOSE; });
}