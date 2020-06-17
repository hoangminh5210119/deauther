#ifndef DisplayUI_h
#define DisplayUI_h

#include "A_config.h"
#include "Alert.h"
#include "Arduino.h"
#include "Attack.h"
// #include "CarRaceGame.h"
#include "Credential.h"
#include "DeviceSleep.h"
// #include "FlappyBirdGame.h"
#include "Keyboard.h"
#include "Names.h"
// #include "PongGame.h"
#include "SSIDs.h"
#include "Scan.h"
#include "Settings.h"
// #include "TRexGame.h"
#include "WifiConfigData.h"
#include "font.h"
#include "images.h"
#include "language.h"
#include <TimeLib.h>

#include <SimpleButton.h>

using namespace simplebutton;

extern Settings settings;
extern Names names;
extern SSIDs ssids;
extern Accesspoints accesspoints;
extern Stations stations;
extern Scan scan;
extern Attack attack;
extern Credential credential;
extern WifiConfigData wifiConfig;

extern Alert alert;
extern Keyboard keyboard;
extern DeviceSleep deviceSleep;
// extern PongGame pongGame;
// extern CarRaceGame carRaceGame;
// extern TRexGame trexGame;
// extern FlappyBirdGame flappyBird;

extern uint32_t currentTime;

extern String leftRight(String a, String b, int len);
extern String center(String a, int len);
extern String left(String a, int len);
extern String right(String a, int len);
extern String leftRight(String a, String b, int len);
extern String replaceUtf8(String str, String r);

// const char D_INTRO_0[] PROGMEM = "";
// const char D_INTRO_1[] PROGMEM = "ESP8266 Deauther";
// const char D_INTRO_2[] PROGMEM = "by @Spacehuhn";
// const char D_INTRO_3[] PROGMEM = "";

// fallback for the buttons
#ifndef BUTTON_UP
#define BUTTON_UP 255
#endif // ifndef BUTTON_UP

#ifndef BUTTON_DOWN
#define BUTTON_DOWN 255
#endif // ifndef BUTTON_DOWN

#ifndef BUTTON_A
#define BUTTON_A 255
#endif // ifndef BUTTON_A

#ifndef BUTTON_B
#define BUTTON_B 255
#endif // ifndef BUTTON_B

// typedef uint8_t pint32[];

#define DISPLAY_GUI true
#define DISPLAY_LIST false

struct MenuNode {
  const uint8_t *image;
  std::function<String()>
      getStr;                  // function used to create the displayed string
  std::function<void()> click; // function that is executed when node is clicked
  std::function<void()>
      hold; // function that is executed when node is pressed for > 800ms
};

struct Menu {
  SimpleList<MenuNode> *list;
  Menu *parentMenu;
  uint8_t selected;
  bool isGUI;
  std::function<void()>
      build; // function that is executed when button is clicked
};

class DisplayUI {
public:
  enum DISPLAY_MODE {
    OFF,
    BUTTON_TEST,
    MENU,
    LOADSCAN,
    PACKETMONITOR,
    INTRO,
    HOME,
    SETCLOCK,
    BRIGHTNESS,
    CAPTIVE,
    CREDENTIAL,
    RESET,
    SMARTCONFIG,
    GAME
  };

  enum SET_TIME {
    SET_HOUR,
    SET_MINUTE,
    SET_WEEK,
    SET_DAY,
    SET_MONTH,
    SET_YEAR
  };

  enum HOME_MODE { HOME_1, HOME_2, HOME_3, HOME_4 };

  enum GAME_TYPE { DINOSAURIA, FLAPPY_BIRD, NONE_GAME, PONG, CAR_RACE };

  // enum KEYBOARD_MODE { LOWERCASE, UPPERCASE, NUMBER, SPECIFIC };

  uint8_t mode = DISPLAY_MODE::MENU;
  uint8_t clock_set = SET_TIME::SET_HOUR;
  uint8_t game_type = GAME_TYPE::DINOSAURIA;

  uint8_t home_mode = HOME_MODE::HOME_1;
  // String keyboard_lowercase = "QWERTYUIOPASDFGHJKLZXCVBNM";
  // String keyboard_uppercase = "QWERTYUIOPASDFGHJKLZXCVBNM";
  // String keyboard_number = "123456789";
  // String keyboard_specific = "~!@#$%^&*()_+-=[]{};':\"\\,./<>?";
  // String text_keyboard = String();

  bool highlightLED = false;

  Button *up = NULL;
  Button *down = NULL;
  Button *a = NULL;
  Button *b = NULL;

  // ===== adjustable ===== //
  DEAUTHER_DISPLAY; // see config.h

  const uint8_t maxLen = 18;
  const uint8_t lineHeight = 12;
  const uint8_t buttonDelay = 250;
  const uint8_t drawInterval = 100; // 100ms = 10 FPS
  const uint16_t scrollSpeed = 500; // time interval in ms
  const uint16_t screenIntroTime = 5000;
  const uint16_t screenWidth = 128;
  const uint16_t sreenHeight = 64;

  void configInit();
  void configOn();
  void configOff();
  void updatePrefix();
  void updateSuffix();
  void drawString(int x, int y, String str);
  void drawString(int row, String str);
  void drawLine(int x1, int y1, int x2, int y2);
  void drawImage(int16_t xMove, int16_t yMove, int16_t width, int16_t height,
                 const uint8_t *xbm);
  void drawImageCenter(int16_t width, int16_t height, const uint8_t *xbm);
  void drawImageCenterHorizontal(int16_t x, int16_t width, int16_t height,
                                 const uint8_t *xbm);
  void drawImageCenterVertical(int16_t y, int16_t width, int16_t height,
                               const uint8_t *xbm);
  // void drawSetBrightness();
  // ====================== //

  DisplayUI();
  ~DisplayUI();

  void setup();
#ifdef HIGHLIGHT_LED
  void setupLED();
#endif // ifdef HIGHLIGHT_LED

  void update();
  void on();
  void off();

private:
  int16_t selectedID = 0;    // i.e. access point ID to draw the apMenu
  uint8_t scrollCounter = 0; // for horizontal scrolling

  uint32_t scrollTime = 0; // last time a character was moved
  uint32_t drawTime = 0;   // last time a frame was drawn
  uint32_t startTime = 0;  // when the screen was enabled
  uint32_t buttonTime = 0; // last time a button was pressed

  bool enabled = false; // display enabled
  bool tempOff = false;

  // selected attack modes
  bool beaconSelected = false;
  bool deauthSelected = false;
  bool probeSelected = false;

  // menus
  Menu *currentMenu;

  Menu mainMenu;

  Menu scanMenu;
  Menu showMenu;
  Menu attackMenu;
  Menu captiveMenu;
  Menu captiveSelectMenu;
  Menu credentialMenu;
  Menu wifiMenu;
  Menu wifiHackMenu;
  // Menu gameMenu;
  Menu timeOnScreenMenu;

  Menu apListMenu;
  Menu stationListMenu;
  Menu nameListMenu;
  Menu ssidListMenu;
  Menu wifiListCaptiveSmartconfigMenu;
  Menu wifiListCaptiveScanMenu;
  Menu wifiListCaptiveScanHackMenu;
  Menu wifiCaptiveMenu;

  Menu listPhoneCredential;
  Menu listFacebookCredential;
  Menu listZaloCredential;
  Menu listTwitterCredential;
  Menu listWifiCredential;

  Menu apMenu;
  Menu stationMenu;
  Menu nameMenu;
  Menu ssidMenu;

  void setupButtons();

  String getChannel();

  // draw functions
  void draw();
  void drawButtonTest();
  void drawMenu();
  void drawLoadingScan();

  void drawPacketMonitor();
  void drawIntro();
  void clearMenu(Menu *menu);
  void drawSmartconfig();
  void drawList();
  void drawGUI();
  void drawHome();
  void drawHome2();
  void drawHome3();
  void drawHome4();
  void drawCntAccount();
  void drawDeviceInfo();
  void drawSystemInfo();

  void buttonUpdate();

  // menu functions
  void changeMenu(Menu *menu);
  void goBack();
  void createMenu(Menu *menu, Menu *parent, bool isGUI,
                  std::function<void()> build);
  // void drawMenu(Menu *currentMenu);

  // FOR GUI DISPLAY
  void addMenuNode(Menu *menu, const uint8_t image[], const char *ptr,
                   std::function<void()> click, std::function<void()> hold);
  void addMenuNode(Menu *menu, const uint8_t image[], const char *ptr,
                   std::function<void()> click);
  void addMenuNode(Menu *menu, const uint8_t image[], const char *ptr,
                   Menu *next);
  // void addMenuNode(Menu *menu, const uint8_t image[], const char *ptr,
  // std::function<void()> click);

  // FOR TEXT DISPLAY
  void addMenuNode(Menu *menu, std::function<String()> getStr,
                   std::function<void()> click, std::function<void()> hold);
  void addMenuNode(Menu *menu, std::function<String()> getStr,
                   std::function<void()> click);
  void addMenuNode(Menu *menu, std::function<String()> getStr, Menu *next);
  void addMenuNode(Menu *menu, const char *ptr, std::function<void()> click);
  void addMenuNode(Menu *menu, const char *ptr, Menu *next);

  // fake clock
  // void setClock();
  String getStringWeek();

  uint32_t clockTime = 0;
  int oldBrighness = 50;
  bool isFistOnScreen = true;
  int frame = 0;
};

#endif // ifndef DisplayUI_h
