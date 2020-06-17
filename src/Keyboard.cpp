#include "Keyboard.h"

const char *keys[] = {
    "qwertyuiopasdfghjklzxcvbnm*", "QWERTYUIOPASDFGHJKLZXCVBNM*",
    "1234567890~`!@#$%^&*()_+-=*", "1234567890[]{}\\|;:'\",.<>/?*"};

const char *option[] = {"space", "Aa", "del", "ok"};

Keyboard::Keyboard(/* args */) {}

Keyboard::~Keyboard() {}

void Keyboard::init() {
  selectKey = keys[0];

  up = new ButtonPullup(BUTTON_UP);
  down = new ButtonPullup(BUTTON_DOWN);
  a = new ButtonPullup(BUTTON_A);

  down->setOnClicked([this]() {
    switch (funckey) {
    case KEY_FUNC::NONE:
      col = col + 1;
      if (col > maxCharInLine - 1) { // || col > (maxCharInLine - 1) * 2
        col = 0;
        if (row < 2) {
          row = row + 1;
        } else {
          funckey = KEY_FUNC::TYPE_KEY;
          row = 0;
          col = 0;
        }
      }
      break;
    case KEY_FUNC::TYPE_KEY:
      funckey = KEY_FUNC::SPACE;
      break;
    case KEY_FUNC::SPACE:
      funckey = KEY_FUNC::DELETE;
      break;
    case KEY_FUNC::DELETE:
      funckey = KEY_FUNC::ENTER;
      break;
    case KEY_FUNC::ENTER:
      funckey = KEY_FUNC::NONE;
      row = 0;
      col = 0;
      break;
    }
  });
  up->setOnClicked([this]() {
    switch (funckey) {
    case KEY_FUNC::NONE:
      col = col - 1;
      if (col < 0) {
        if (row <= 2 && row > 0) {
          row = row - 1;
          col = maxCharInLine - 1;
        } else { // col = 0;
          col = maxCharInLine - 1;
          row = 2;
          funckey = KEY_FUNC::ENTER;
        }
      }
      break;
    case KEY_FUNC::TYPE_KEY:
      funckey = KEY_FUNC::NONE;
      row = 2;
      col = maxCharInLine - 1;
      break;
    case KEY_FUNC::SPACE:
      funckey = KEY_FUNC::TYPE_KEY;
      break;
    case KEY_FUNC::DELETE:
      funckey = KEY_FUNC::SPACE;
      break;
    case KEY_FUNC::ENTER:
      funckey = KEY_FUNC::DELETE;
      break;
    }
  });
  a->setOnClicked([this]() {
    switch (funckey) {
    case KEY_FUNC::NONE:
      text = text + selectKey.charAt(col + row * maxCharInLine);
      break;
    case KEY_FUNC::TYPE_KEY:
      switch (key_type) {
      case KEY_TYPE::LOWERCASE:
        selectKey = keys[1];
        key_type = KEY_TYPE::UPPERCASE;
        break;
      case KEY_TYPE::UPPERCASE:
        selectKey = keys[2];
        key_type = KEY_TYPE::NUMBER1;
        break;
      case KEY_TYPE::NUMBER1:
        selectKey = keys[3];
        key_type = KEY_TYPE::NUMBER2;
        break;
      default:
        selectKey = keys[0];
        key_type = KEY_TYPE::LOWERCASE;
        break;
      }
      break;
    case KEY_FUNC::DELETE:
      text.remove(text.length() - 1);
      break;
    case KEY_FUNC::ENTER:
      _exit = true;
      break;
    case KEY_FUNC::SPACE:
      text = text + " ";
      break;
    }
  });
}

void Keyboard::initLCD() {
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setColor(WHITE);
}
void Keyboard::buttonUpdate() {
  up->update();
  down->update();
  a->update();
}

void Keyboard::updateCursor() {
  if (millis() - old_time_cursor > cursor_time_out) {
    old_time_cursor = millis();
    isShowCursor = !isShowCursor;
  }
}

bool Keyboard::isNumber(String string) {
  boolean isNum = false;
  for (byte i = 0; i < string.length(); i++) {
    isNum = isDigit(string.charAt(i)) || string.charAt(i) == '+' ||
            string.charAt(i) == '.' || string.charAt(i) == '-';
    if (!isNum)
      return false;
  }
  return isNum;
}

String Keyboard::show() {
  _exit = false;
  initLCD();
  while (true) {
    buttonUpdate();
    updateCursor();
    display.clear();
    int _indexCol = 0;
    int _indexRow = 0;

    switch (funckey) {
    case KEY_FUNC::NONE:
      display.fillRect(5 + widthBtn * col, 22 + 10 * row, widthBtn, heightBtn);
      break;
    case KEY_FUNC::TYPE_KEY:
      display.drawRect(5, 51, 20, heightBtn);
      break;
    case KEY_FUNC::DELETE:
      display.drawRect(84, 51, 18, heightBtn);
      break;
    case KEY_FUNC::ENTER:
      display.drawRect(104, 51, 15, heightBtn);
      break;
    case KEY_FUNC::SPACE:
      display.drawRect(39, 51, 30, heightBtn);
      break;
    }

    for (int i = 0; i < selectKey.length(); i++) {
      display.drawRect(0, 0, width_screen, 15);
      if (i == maxCharInLine || i == maxCharInLine * 2) {
        _indexCol = 0;
        _indexRow = _indexRow + 1;
      }
      if (_indexCol == col && _indexRow == row && funckey == KEY_FUNC::NONE) {
        display.setColor(BLACK);
      } else {
        display.setColor(WHITE);
      }
      display.drawString(_indexCol * widthBtn + 8, _indexRow * 10 + 20,
                         String(selectKey.charAt(i)));
      display.setColor(WHITE);
      _indexCol = _indexCol + 1;
      display.drawString(40, 50, option[0]);
      display.drawString(8, 50, option[1]);
      display.drawString(85, 50, option[2]);
      display.drawString(105, 50, option[3]);
      if (isShowCursor) {
        display.drawString(1, 0, text + "|");
      } else {
        display.drawString(1, 0, text);
      }
    }
    display.display();
    delay(0);
    if (_exit) {
      break;
    }
  }
  String _text = text;
  text = String();
  return _text;
}

String Keyboard::show(String placeholder) {
  text = placeholder;
  return show();
}