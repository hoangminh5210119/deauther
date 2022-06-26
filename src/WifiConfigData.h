#ifndef WifiConfigData_h
#define WifiConfigData_h

#include "A_config.h"
#include "Arduino.h"
#include "ArduinoJson.h"
#include "ESP8266WiFi.h"
#include "SimpleList.h"
#include "language.h"

// #include "Settings.h"

extern bool readFile(String path, String &buf);
extern bool appendFile(String path, String &buf);
extern bool writeFile(String path, String &buf);

// extern Settings settings;

struct WIFI {
  bool selected;
  const char *ssid;
  const char *pass;
};

class WifiConfigData {

public:
  WifiConfigData();
  ~WifiConfigData();
  bool smartconfig();
  bool smartconfigNotAlert();
  void save(String ssid, String pass);
  void deleteAll();
  String get();
  void disconnect();
  void toSerial();
  int count();
  String getSSID(int id);
  String getPassword(int id);
  void setSSID(String ssid);
  void select(String ssid);
  void deselect(String ssid);
  void init();
  bool getSelected(int num);
  String getSelectedStr(int num);
  void select(int num);
  void deselect(int num);

  String getSSID();
  String getPass();

private:
  String FILE_PATH = "/wificonfig.json";
  String buf;
  bool isSuccess = false;

  SimpleList<WIFI> *list;

  bool isExist(JsonObject &root, String key);
  void add(bool selected, String ssid, String pass);
  bool internal_check(int num);

  void internal_select(int num);

  void internal_deselect(int num);

  void internal_remove(int num);
  bool check(int num);

  DEAUTHER_DISPLAY;
  void initLCD();

  String _ssid;
};
#endif